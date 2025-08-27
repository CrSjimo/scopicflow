#ifndef SCOPIC_FLOW_PLAYBACKVIEWMODEL_H
#define SCOPIC_FLOW_PLAYBACKVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT PlaybackViewModel : public QQmlPropertyMap {
        Q_OBJECT

    public:
        explicit PlaybackViewModel(QObject *parent = nullptr);
        ~PlaybackViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, primaryPosition, primaryPosition, setPrimaryPosition, resetPrimaryPosition)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, secondaryPosition, secondaryPosition, setSecondaryPosition, resetSecondaryPosition)
    };

}

#endif //SCOPIC_FLOW_PLAYBACKVIEWMODEL_H
