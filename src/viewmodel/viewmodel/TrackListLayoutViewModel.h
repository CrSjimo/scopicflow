#ifndef SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H
#define SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT TrackListLayoutViewModel : public QQmlPropertyMap {
        Q_OBJECT
    public:
        explicit TrackListLayoutViewModel(QObject *parent = nullptr);
        ~TrackListLayoutViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, viewportOffset, viewportOffset, setViewportOffset, resetViewportOffset)

    };

}

#endif //SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H
