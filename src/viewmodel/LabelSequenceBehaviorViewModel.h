#ifndef SCOPIC_FLOW_LABELSEQUENCEBEHAVIORVIEWMODEL_H
#define SCOPIC_FLOW_LABELSEQUENCEBEHAVIORVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT LabelSequenceBehaviorViewModel : public QQmlPropertyMap {
        Q_OBJECT

    public:
        explicit LabelSequenceBehaviorViewModel(QObject *parent = nullptr);
        ~LabelSequenceBehaviorViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, editing, isEditing, setEditing, resetEditing)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(QColor, color, color, setColor, resetColor)

    };

}

#endif //SCOPIC_FLOW_LABELSEQUENCEBEHAVIORVIEWMODEL_H
