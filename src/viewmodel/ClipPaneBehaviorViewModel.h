#ifndef SCOPIC_FLOW_CLIPPANEBEHAVIORVIEWMODEL_H
#define SCOPIC_FLOW_CLIPPANEBEHAVIORVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT ClipPaneBehaviorViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(int lengthHint READ lengthHint WRITE setLengthHint NOTIFY lengthHintChanged)
        Q_PROPERTY(sflow::ClipPaneBehaviorViewModel::MouseBehavior mouseBehavior READ mouseBehavior WRITE setMouseBehavior NOTIFY mouseBehaviorChanged)

    public:
        explicit ClipPaneBehaviorViewModel(QObject *parent = nullptr);
        ~ClipPaneBehaviorViewModel() override;

        int lengthHint() const;
        void setLengthHint(int lengthHint);

        enum MouseBehavior {
            None,
            Pointer,
            Pen,
            Eraser,
            Scissor,
        };
        Q_ENUM(MouseBehavior)
        MouseBehavior mouseBehavior() const;
        void setMouseBehavior(MouseBehavior mouseBehavior);

    signals:
        void lengthHintChanged(int lengthHint);
        void mouseBehaviorChanged(sflow::ClipPaneBehaviorViewModel::MouseBehavior mouseBehavior);

    private:
        int m_lengthHint;
        MouseBehavior m_mouseBehavior;
    };

}

#endif //SCOPIC_FLOW_CLIPPANEBEHAVIORVIEWMODEL_H
