#ifndef SCOPIC_FLOW_INVOKEMETHODHELPER_P_H
#define SCOPIC_FLOW_INVOKEMETHODHELPER_P_H

#include <array>
#include <memory>
#include <type_traits>
#include <utility>

#include <QMetaMethod>
#include <QMetaProperty>
#include <QMetaType>
#include <QObject>
#include <QQmlInfo>
#include <QVariant>

namespace sflow {

    class InvokeMethodHelper {
    public:
        // Resolves the unique overload named `name` on `obj` that can be invoked through invokeWithoutReturn
        // with the given argument types. Candidates are ranked by the number of exactly matching parameter
        // types; parameters that only match after an automatic conversion rank lower. The overload must be
        // the unique best candidate, otherwise an invalid QMetaMethod is returned and a warning is printed.
        template <typename... Args>
        static QMetaMethod resolveMethod(QObject *context, QObject *obj, const char *name) {
            if (!obj)
                return {};
            const auto metaObject = obj->metaObject();
            QMetaMethod resolvedMethod;
            int bestScore = -1;
            int bestCount = 0;
            for (int i = 0; i < metaObject->methodCount(); ++i) {
                const auto method = metaObject->method(i);
                if (method.name() != name || method.parameterCount() != int(sizeof...(Args)))
                    continue;
                const int score = matchParameters<Args...>(method);
                if (score < 0 || score < bestScore)
                    continue;
                if (score > bestScore) {
                    resolvedMethod = method;
                    bestScore = score;
                    bestCount = 1;
                } else {
                    ++bestCount;
                }
            }
            if (bestCount != 1) {
                if (bestCount == 0) {
                    qmlWarning(context) << "InvokeMethodHelper: object of type " << metaObject->className()
                                        << " does not declare a method named " << name << " with "
                                        << int(sizeof...(Args)) << " parameters that accept the given argument types";
                } else {
                    qmlWarning(context) << "InvokeMethodHelper: object of type " << metaObject->className()
                                        << " declares ambiguous overloads of a method named " << name
                                        << " for the given argument types";
                }
                return {};
            }
            return resolvedMethod;
        }

        // Resolves the property named `name` on `obj`. Returns an invalid QMetaProperty and, if
        // `warnOnMissing` is true, prints a warning when the property does not exist.
        static QMetaProperty resolveProperty(QObject *context, QObject *obj, const char *name,
                                             bool warnOnMissing = true) {
            if (!obj)
                return {};
            const auto metaObject = obj->metaObject();
            const int index = metaObject->indexOfProperty(name);
            if (index < 0) {
                if (warnOnMissing) {
                    qmlWarning(context) << "InvokeMethodHelper: object of type " << metaObject->className()
                                        << " does not declare a property named " << name;
                }
                return {};
            }
            return metaObject->property(index);
        }

        template <typename... Args>
        static bool invokeWithoutReturn(QObject *context, QObject *obj, const QMetaMethod &method, Args &&...args) {
            constexpr int argumentCount = int(sizeof...(Args));
            static_assert(argumentCount <= 10, "InvokeMethodHelper::invokeWithoutReturn supports at most 10 arguments");

            if (!obj || !method.isValid() || method.parameterCount() != argumentCount) {
                qmlWarning(context) << "InvokeMethodHelper: cannot invoke method " << method.name() << " with "
                                    << argumentCount << " arguments";
                return false;
            }
            return doInvoke(context, obj, method, QGenericReturnArgument(),
                            std::make_index_sequence<sizeof...(Args)>(), std::forward<Args>(args)...);
        }

        template <typename Return, typename... Args>
        static Return invokeWithReturn(QObject *context, QObject *obj, const QMetaMethod &method, Args &&...args) {
            constexpr int argumentCount = int(sizeof...(Args));
            static_assert(argumentCount <= 10, "InvokeMethodHelper::invokeWithReturn supports at most 10 arguments");

            if (!obj || !method.isValid() || method.parameterCount() != argumentCount) {
                qmlWarning(context) << "InvokeMethodHelper: cannot invoke method " << method.name() << " with "
                                    << argumentCount << " arguments";
                return {};
            }
            const QMetaType returnType = method.returnMetaType();
            void *returnData = returnType.isValid() ? returnType.create() : nullptr;
            if (!returnData) {
                qmlWarning(context) << "InvokeMethodHelper: cannot create the return value for method "
                                    << method.name();
                return {};
            }
            Return result{};
            const bool succeeded =
                doInvoke(context, obj, method, QGenericReturnArgument(method.typeName(), returnData),
                         std::make_index_sequence<sizeof...(Args)>(), std::forward<Args>(args)...);
            if (succeeded) {
                QVariant resultVariant(returnType, returnData);
                if (resultVariant.canConvert<Return>()) {
                    result = resultVariant.value<Return>();
                } else {
                    qmlWarning(context) << "InvokeMethodHelper: return value of method " << method.name()
                                        << " of type " << returnType.name() << " cannot be converted to "
                                        << typeNameForWarning(QMetaType::fromType<Return>());
                }
            }
            returnType.destroy(returnData);
            return result;
        }

    private:
        enum class ParameterTypeMatch : int {
            None = 0,
            Convertible = 1,
            Exact = 2,
        };

        template <typename... Args>
        static int matchParameters(const QMetaMethod &method) {
            int score = 0;
            bool rejected = false;
            int index = 0;
            (matchParameter<Args>(method, index++, score, rejected), ...);
            return rejected ? -1 : score;
        }

        template <typename Arg>
        static void matchParameter(const QMetaMethod &method, int index, int &score, bool &rejected) {
            using DecayedArg = std::remove_cv_t<std::remove_reference_t<Arg>>;
            const QMetaType parameterType = method.parameterMetaType(index);
            if (!parameterType.isValid() || !parameterType.name()) {
                rejected = true;
                return;
            }
            const QMetaType argumentType = QMetaType::fromType<DecayedArg>();
            if (argumentType == parameterType) {
                score += int(ParameterTypeMatch::Exact);
                return;
            }
            if constexpr (std::is_pointer_v<DecayedArg> &&
                          std::is_base_of_v<QObject, std::remove_pointer_t<DecayedArg>>) {
                if (!(parameterType.flags() & QMetaType::PointerToQObject) || !parameterType.metaObject()) {
                    rejected = true;
                    return;
                }
            } else {
                if (!QMetaType::canConvert(argumentType, parameterType)) {
                    rejected = true;
                    return;
                }
            }
            score += int(ParameterTypeMatch::Convertible);
        }

        template <std::size_t... I, typename... Args>
        static bool doInvoke(QObject *context, QObject *obj, const QMetaMethod &method,
                             QGenericReturnArgument returnArgument, std::index_sequence<I...>, Args &&...args) {
            std::array<QGenericArgument, sizeof...(Args)> genericArguments{};
            std::array<void *, sizeof...(Args)> convertedObjects{};
            std::array<QMetaType, sizeof...(Args)> ownedTypes{};
            // QMetaMethod::invoke() compares the argument type names against the strings stored by moc for the
            // formal parameters, so use QMetaMethod::parameterTypes() verbatim instead of QMetaType::name()
            // (which may be scopes-qualified, unlike the names recorded by moc)
            const QByteArrayList parameterNames = method.parameterTypes();
            bool succeeded = (prepareArgument(context, method, I, parameterNames.at(int(I)).constData(),
                                              std::forward<Args>(args), &genericArguments[I],
                                              &convertedObjects[I], &ownedTypes[I]) &&
                              ...);
            if (succeeded && !method.invoke(obj, Qt::AutoConnection, returnArgument, genericArguments[I]...)) {
                qmlWarning(context) << "InvokeMethodHelper: failed to invoke method " << method.name() << " on "
                                    << obj->metaObject()->className();
                succeeded = false;
            }
            (destroyOwnedObject(ownedTypes[I], convertedObjects[I]), ...);
            return succeeded;
        }

        static void destroyOwnedObject(const QMetaType &type, void *data) {
            if (type.isValid() && data)
                type.destroy(data);
        }

        template <typename Arg>
        static bool prepareArgument(QObject *context, const QMetaMethod &method, std::size_t index,
                                    const char *parameterName, Arg &&arg, QGenericArgument *genericArgument,
                                    void **convertedObject, QMetaType *ownedType) {
            using DecayedArg = std::remove_cv_t<std::remove_reference_t<Arg>>;
            const QMetaType parameterType = method.parameterMetaType(int(index));
            if (!parameterType.isValid() || !parameterType.name()) {
                qmlWarning(context) << "InvokeMethodHelper: parameter " << index << " of method " << method.name()
                                    << " has an unknown type";
                return false;
            }
            if constexpr (std::is_pointer_v<DecayedArg> &&
                          std::is_base_of_v<QObject, std::remove_pointer_t<DecayedArg>>) {
                const QMetaObject *parameterMetaObject = parameterType.metaObject();
                if (!(parameterType.flags() & QMetaType::PointerToQObject) || !parameterMetaObject) {
                    qmlWarning(context) << "InvokeMethodHelper: argument " << index << " of method " << method.name()
                                        << " is expected to be " << parameterType.name() << " but "
                                        << typeNameForWarning(QMetaType::fromType<DecayedArg>()) << " was given";
                    return false;
                }
                QObject *object = const_cast<QObject *>(static_cast<const QObject *>(arg));
                void *converted = object ? parameterMetaObject->cast(object) : nullptr;
                if (object && !converted) {
                    qmlWarning(context) << "InvokeMethodHelper: argument " << index << " of method " << method.name()
                                        << " of type " << typeNameForWarning(QMetaType::fromType<DecayedArg>())
                                        << " cannot be converted to " << parameterType.name();
                    return false;
                }
                *convertedObject = converted;
                *genericArgument = QGenericArgument(parameterName, convertedObject);
                return true;
            } else {
                const QMetaType argumentType = QMetaType::fromType<DecayedArg>();
                if (argumentType == parameterType) {
                    *genericArgument = QGenericArgument(parameterName, &arg);
                    return true;
                }
                // Types differ: attempt an automatic conversion, for example from an integral value to an
                // enumeration type
                if (void *converted = parameterType.create()) {
                    if (QMetaType::convert(argumentType, std::addressof(arg), parameterType, converted)) {
                        *ownedType = parameterType;
                        *convertedObject = converted;
                        *genericArgument = QGenericArgument(parameterName, converted);
                        return true;
                    }
                    parameterType.destroy(converted);
                }
                qmlWarning(context) << "InvokeMethodHelper: argument " << index << " of method " << method.name()
                                    << " is expected to be " << parameterType.name() << " but "
                                    << typeNameForWarning(argumentType) << " was given";
                return false;
            }
        }

        static const char *typeNameForWarning(const QMetaType &type) {
            const char *name = type.name();
            return name ? name : "(unknown)";
        }
    };

}

#endif // SCOPIC_FLOW_INVOKEMETHODHELPER_P_H
