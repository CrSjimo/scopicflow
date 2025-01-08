#ifndef SCOPIC_FLOW_CLAVIERHELPER_P_H
#define SCOPIC_FLOW_CLAVIERHELPER_P_H

#include <QQuickItem>

namespace sflow {

    // TODO replace this with SVSCraft value type later
    class ClavierHelper : public QObject {
        Q_OBJECT
    public:
        Q_INVOKABLE QString keyNameImpl(int key, int accidentalType) const;
    };

} // sflow

#endif //SCOPIC_FLOW_CLAVIERHELPER_P_H
