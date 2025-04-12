#ifndef SCOPIC_FLOW_TRACKVIEWMODEL_H
#define SCOPIC_FLOW_TRACKVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT TrackViewModel : public QQmlPropertyMap {
        Q_OBJECT

    public:
        explicit TrackViewModel(QObject *parent = nullptr);
        ~TrackViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(QString, name, name, setName, resetName)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, mute, mute, setMute, resetMute)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, solo, solo, setSolo, resetSolo)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, record, record, setRecord, resetRecord)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, gain, gain, setGain, resetGain)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, pan, pan, setPan, resetPan)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, intermediate, intermediate, setIntermediate, resetIntermediate)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, leftLevel, leftLevel, setLeftLevel, resetLeftLevel)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, rightLevel, rightLevel, setRightLevel, resetRightLevel)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, selected, selected, setSelected, resetSelected)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, rowHeight, rowHeight, setRowHeight, resetRowHeight)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(QColor, color, color, setColor, resetColor)

    };

}

#endif //SCOPIC_FLOW_TRACKVIEWMODEL_H
