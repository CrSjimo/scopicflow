#ifndef SCOPIC_FLOW_EMITHELPER_P_H
#define SCOPIC_FLOW_EMITHELPER_P_H

#include <QQmlPropertyMap>
#include <QMetaObject>
#include <QMetaMethod>

namespace sflow {
    class ViewModelHelper {
    public:
        static void connectPropertyNotify(QObject *sender, const char *propertyName, QObject *receiver, const QMetaMethod &slotMethod) {
            auto index = sender->metaObject()->indexOfProperty(propertyName);
            auto property = sender->metaObject()->property(index);
            if (!property.hasNotifySignal())
                return;
            QObject::connect(sender, property.notifySignal(), receiver, slotMethod);

        }
    };

}

#endif //SCOPIC_FLOW_EMITHELPER_P_H
