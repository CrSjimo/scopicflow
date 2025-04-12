#ifndef SCOPIC_FLOW_BUSTRACKVIEWMODEL_H
#define SCOPIC_FLOW_BUSTRACKVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT BusTrackViewModel : public QQmlPropertyMap {
        Q_OBJECT
        Q_PROPERTY(bool bus READ isBus CONSTANT)

    public:
        explicit BusTrackViewModel(QObject *parent = nullptr);
        ~BusTrackViewModel() override;

        static constexpr bool isBus() { return true; }

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(QString, name, name, setName, resetName)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, mute, mute, setMute, resetMute)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(QVariant, route, route, setRoute, resetRoute)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, gain, gain, setGain, resetGain)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, pan, pan, setPan, resetPan)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, intermediate, intermediate, setIntermediate, resetIntermediate)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, leftLevel, leftLevel, setLeftLevel, resetLeftLevel)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, rightLevel, rightLevel, setRightLevel, resetRightLevel)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, selected, selected, setSelected, resetSelected)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(QColor, color, color, setColor, resetColor)
    };

}

#endif //SCOPIC_FLOW_BUSTRACKVIEWMODEL_H
