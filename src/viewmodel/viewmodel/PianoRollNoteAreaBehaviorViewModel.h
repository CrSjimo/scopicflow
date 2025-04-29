#ifndef SCOPIC_FLOW_PIANOROLLNOTEAREABEHAVIORVIEWMODEL_H
#define SCOPIC_FLOW_PIANOROLLNOTEAREABEHAVIORVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowNamespace.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT PianoRollNoteAreaBehaviorViewModel : public QQmlPropertyMap {
        Q_OBJECT

    public:
        explicit PianoRollNoteAreaBehaviorViewModel(QObject *parent = nullptr);
        ~PianoRollNoteAreaBehaviorViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, editing, isEditing, setEditing, resetEditing)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, unitedExtend, isUnitedExtend, setUnitedExtend, resetUnitedExtend)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(QColor, color, color, setColor, resetColor)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, compactDisplay, isCompactDisplay, setCompactDisplay, resetCompactDisplay)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, offset, offset, setOffset, resetOffset)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, lengthHint, lengthHint, setLengthHint, resetLengthHint)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(ScopicFlow::MouseBehavior, mouseBehavior, mouseBehavior, setMouseBehavior, resetMouseBehavior)

    };

}

#endif //SCOPIC_FLOW_PIANOROLLNOTEAREABEHAVIORVIEWMODEL_H
