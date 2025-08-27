#ifndef SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H
#define SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT TrackListLayoutViewModel : public QQmlPropertyMap {
        Q_OBJECT
    public:
        explicit TrackListLayoutViewModel(QObject *parent = nullptr);
        ~TrackListLayoutViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, viewportOffset, viewportOffset, setViewportOffset, resetViewportOffset)

    };

}

#endif //SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H
