#ifndef SCOPIC_FLOW_WHEELMODIFIERVIEWMODEL_H
#define SCOPIC_FLOW_WHEELMODIFIERVIEWMODEL_H

#include <QObject>
#include <QHash>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT WheelModifierViewModel : public QObject {
        Q_OBJECT
    public:
        explicit WheelModifierViewModel(QObject *parent = nullptr);
        ~WheelModifierViewModel() override;

        enum WheelAction {
            AlternateAxis,
            Zoom,
            Page,
        };
        Q_ENUM(WheelAction)

        Qt::KeyboardModifier modifier(WheelAction action);
        void setModifier(WheelAction action, Qt::KeyboardModifier modifier);

    signals:
        void modifierChanged(WheelAction action, Qt::KeyboardModifier modifier);

    private:
        QHash<WheelAction, Qt::KeyboardModifier> m_modifiers;
    };

} // sflow

#endif //SCOPIC_FLOW_WHEELMODIFIERVIEWMODEL_H
