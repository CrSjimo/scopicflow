#ifndef SCOPIC_FLOW_PIANOROLLNOTEAREABEHAVIORVIEWMODEL_H
#define SCOPIC_FLOW_PIANOROLLNOTEAREABEHAVIORVIEWMODEL_H

#include <QObject>
#include <QColor>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT PianoRollNoteAreaBehaviorViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(bool editing READ editing WRITE setEditing NOTIFY editingChanged)
        Q_PROPERTY(bool unitedExtend READ unitedExtend WRITE setUnitedExtend NOTIFY unitedExtendChanged)
        Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
        Q_PROPERTY(bool compactDisplay READ compactDisplay WRITE setCompactDisplay NOTIFY compactDisplayChanged)
        Q_PROPERTY(int lengthHint READ lengthHint WRITE setLengthHint NOTIFY lengthHintChanged)
        Q_PROPERTY(sflow::PianoRollNoteAreaBehaviorViewModel::MouseBehavior mouseBehavior READ mouseBehavior WRITE setMouseBehavior NOTIFY mouseBehaviorChanged)

    public:
        explicit PianoRollNoteAreaBehaviorViewModel(QObject *parent = nullptr);
        ~PianoRollNoteAreaBehaviorViewModel() override;

        bool editing() const;
        void setEditing(bool editing);

        bool unitedExtend() const;
        void setUnitedExtend(bool unitedExtend);

        QColor color() const;
        void setColor(const QColor &color);

        bool compactDisplay() const;
        void setCompactDisplay(bool compactDisplay);

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
        void editingChanged(bool editing);
        void unitedExtendChanged(bool unitedExtend);
        void colorChanged(const QColor &color);
        void compactDisplayChanged(bool compactDisplay);
        void lengthHintChanged(int lengthHint);
        void mouseBehaviorChanged(sflow::PianoRollNoteAreaBehaviorViewModel::MouseBehavior mouseBehavior);

    private:
        bool m_editing;
        bool m_unitedExtend;
        bool m_compactDisplay;
        QColor m_color;
        int m_lengthHint;
        MouseBehavior m_mouseBehavior;
    };

} // sflow

#endif //SCOPIC_FLOW_PIANOROLLNOTEAREABEHAVIORVIEWMODEL_H
