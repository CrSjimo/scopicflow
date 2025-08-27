#ifndef SCOPIC_FLOW_EMITHELPER_P_H
#define SCOPIC_FLOW_EMITHELPER_P_H

#include <QQmlPropertyMap>
#include <QMetaObject>
#include <QMetaMethod>

namespace sflow {
    class ViewModelHelper {
    public:
        static void initializeProperties(QQmlPropertyMap *o) {
            auto metaObject = o->metaObject();
            for (int i = 0; i < metaObject->methodCount(); i++) {
                auto method = metaObject->method(i);
                if (method.tag() != QStringLiteral("SFLOW_VIEWMODEL_RESET"))
                    continue;
                method.invoke(o);
            }
        }

        static void connectValueChanged(QQmlPropertyMap *o) {
            auto metaObject = o->metaObject();
            QHash<QString, QMetaMethod> targetSignals;
            for (int i = 0; i < metaObject->methodCount(); i++) {
                auto method = metaObject->method(i);
                if (method.tag() != QStringLiteral("SFLOW_VIEWMODEL_SIGNAL"))
                    continue;
                auto propertyName = method.name();
                propertyName.remove(propertyName.length() - 7, 7); // suppose signal name to be [property name]Changed
                targetSignals.insert(propertyName, method);
            }
            QObject::connect(o, &QQmlPropertyMap::valueChanged, [o, targetSignals](const QString &key, const QVariant &value) {
                if (!targetSignals.contains(key))
                    return;
                auto method = targetSignals.value(key);
                auto argValue = value;
                argValue.convert(method.parameterMetaType(0));
                QGenericArgument arg(QMetaType(argValue.userType()).name(), argValue.constData());
                method.invoke(o, arg);
            });
        }

        static void connectPropertyNotify(QObject *sender, const char *propertyName, QObject *receiver, const QMetaMethod &slotMethod) {
            auto index = sender->metaObject()->indexOfProperty(propertyName);
            auto property = sender->metaObject()->property(index);
            if (!property.hasNotifySignal())
                return;
            QObject::connect(sender, property.notifySignal(), receiver, slotMethod);

        }
    };

    namespace impl {

        template <typename>
        struct ReturnTypeHelper;
        template <typename ClassType, typename ReturnType>
        struct ReturnTypeHelper<ReturnType (ClassType::*)() const> {
            using type = ReturnType;
        };

        template <typename>
        struct ParameterTypeHelper;
        template <typename ClassType, typename ReturnType, typename Arg>
        struct ParameterTypeHelper<ReturnType (ClassType::*)(Arg)> {
            using type = Arg;
        };

    }

#define SFLOW_INITIALIZE_VIEWMODEL() \
    do { \
        sflow::ViewModelHelper::initializeProperties(this); \
        sflow::ViewModelHelper::connectValueChanged(this); \
    } while(0)

#define SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(clazz, name, defaultValue, read, write, reset) \
    sflow::impl::ReturnTypeHelper<decltype(&clazz::read)>::type clazz::read() const { \
        using type = sflow::impl::ReturnTypeHelper<decltype(&clazz::read)>::type; \
        return value(#name).value<type>(); \
    } \
    void clazz::write(sflow::impl::ParameterTypeHelper<decltype(&clazz::write)>::type a) { \
        insert(#name, QVariant::fromValue(a)); \
    } \
    void clazz::reset() { \
        insert(#name, QVariant::fromValue(defaultValue)); \
    }

#define SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(clazz, name, read, write, reset) \
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(clazz, name, sflow::impl::ReturnTypeHelper<decltype(&clazz::read)>::type{}, read, write, reset)

}

#endif //SCOPIC_FLOW_EMITHELPER_P_H
