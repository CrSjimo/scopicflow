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
                if (method.tag() != QStringLiteral("SFLOW_VIEWMODEL_SET_FUNC"))
                    continue;
                auto paramType = method.parameterMetaType(0);
                auto d = paramType.create();
                QGenericArgument arg(paramType.name(), d);
                method.invoke(o, arg);
                paramType.destroy(d);
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
                propertyName.remove(propertyName.length() - 7, 7);
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
    };
}

#endif //SCOPIC_FLOW_EMITHELPER_P_H
