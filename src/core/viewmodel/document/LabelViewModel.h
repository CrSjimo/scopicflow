#ifndef SCOPIC_FLOW_LABELVIEWMODEL_H
#define SCOPIC_FLOW_LABELVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT LabelViewModel : public QQmlPropertyMap {
        Q_OBJECT
    public:
        explicit LabelViewModel(QObject *parent = nullptr);
        ~LabelViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, position, position, setPosition, resetPosition)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(QString, content, content, setContent, resetContent)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, selected, isSelected, setSelected, resetSelected)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, intermediate, isIntermediate, setIntermediate, resetIntermediate)

    };

}

#endif //SCOPIC_FLOW_LABELVIEWMODEL_H
