#ifndef SCOPIC_FLOW_ANCHORVIEWMODEL_H
#define SCOPIC_FLOW_ANCHORVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowNamespace.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT AnchorViewModel : public QQmlPropertyMap {
        Q_OBJECT

    public:
        explicit AnchorViewModel(QObject *parent = nullptr);
        ~AnchorViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, position, position, setPosition, resetPosition)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, anchorValue, anchorValue, setAnchorValue, resetAnchorValue)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(ScopicFlow::AnchorType, anchorType, anchorType, setAnchorType, resetAnchorType)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, styleFlag, styleFlag, setStyleFlag, resetStyleFlag)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(ScopicFlow::AnchorNode, anchorNode, anchorNode, setAnchorNode, resetAnchorNode)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, free, isFree, setFree, resetFree)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, selected, selected, setSelected, resetSelected)

    };

}

#endif //SCOPIC_FLOW_ANCHORVIEWMODEL_H
