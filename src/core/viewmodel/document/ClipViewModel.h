#ifndef SCOPIC_FLOW_CLIPVIEWMODEL_H
#define SCOPIC_FLOW_CLIPVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT ClipViewModel : public QQmlPropertyMap {
        Q_OBJECT
    public:
        explicit ClipViewModel(QObject *parent = nullptr);
        ~ClipViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, position, position, setPosition, resetPosition)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, length, length, setLength, resetLength)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, clipStart, clipStart, setClipStart, resetClipStart)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, maxLength, maxLength, setMaxLength, resetMaxLength)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, trackNumber, trackNumber, setTrackNumber, resetTrackNumber)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(QString, name, name, setName, resetName)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, selected, isSelected, setSelected, resetSelected)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, intermediate, isIntermediate, setIntermediate, resetIntermediate)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(bool, overlapped, isOverlapped, setOverlapped, resetOverlapped)
    };

}

#endif //SCOPIC_FLOW_CLIPVIEWMODEL_H
