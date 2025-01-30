#ifndef SCOPIC_FLOW_PIANOROLLNOTEAREABEHAVIORVIEWMODEL_H
#define SCOPIC_FLOW_PIANOROLLNOTEAREABEHAVIORVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT PianoRollNoteAreaBehaviorViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(bool editing READ editing WRITE setEditing NOTIFY editingChanged)
        Q_PROPERTY(bool unitedExtend READ unitedExtend WRITE setUnitedExtend NOTIFY unitedExtendChanged)
        Q_PROPERTY(sflow::PianoRollNoteAreaBehaviorViewModel::MouseBehavior mouseBehavior READ mouseBehavior WRITE setMouseBehavior NOTIFY mouseBehaviorChanged)

    public:
        explicit PianoRollNoteAreaBehaviorViewModel(QObject *parent = nullptr);
        ~PianoRollNoteAreaBehaviorViewModel() override;

        bool editing() const;
        void setEditing(bool editing);

        bool unitedExtend() const;
        void setUnitedExtend(bool unitedExtend);

        enum MouseBehavior {
            Pointer,
            Pen,
            Eraser,
            Scissor,
        };
        Q_ENUM(MouseBehavior)
        MouseBehavior mouseBehavior() const;
        void setMouseBehavior(MouseBehavior mouseBehavior);

    signals:
        void editingChanged(bool editing);
        void unitedExtendChanged(bool unitedExtend);
        void mouseBehaviorChanged(sflow::PianoRollNoteAreaBehaviorViewModel::MouseBehavior mouseBehavior);

    private:
        bool m_editing;
        bool m_unitedExtend;
        MouseBehavior m_mouseBehavior;
    };

} // sflow

#endif //SCOPIC_FLOW_PIANOROLLNOTEAREABEHAVIORVIEWMODEL_H
