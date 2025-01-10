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

        double viewportOffset() const;

    public slots:
        SFLOW_VIEWMODEL_SET_FUNC void setViewportOffset(double value);

    signals:
        SFLOW_VIEWMODEL_SIGNAL void viewportOffsetChanged(double value);

    };

}

#endif //SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H
