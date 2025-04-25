#ifndef SCOPIC_FLOW_PARAMETERRANGEVIEWMODEL_H
#define SCOPIC_FLOW_PARAMETERRANGEVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT ParameterRangeViewModel : public QQmlPropertyMap {
        Q_OBJECT

    public:
        explicit ParameterRangeViewModel(QObject *parent = nullptr);
        ~ParameterRangeViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, topValue, topValue, setTopValue, resetTopValue);
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, bottomValue, bottomValue, setBottomValue, resetBottomValue);

    };

} // sflow

#endif //SCOPIC_FLOW_PARAMETERRANGEVIEWMODEL_H
