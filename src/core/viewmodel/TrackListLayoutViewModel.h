#ifndef SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H
#define SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT TrackListLayoutViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(double viewportOffset READ viewportOffset WRITE setViewportOffset NOTIFY viewportOffsetChanged)
    public:
        explicit TrackListLayoutViewModel(QObject *parent = nullptr);
        ~TrackListLayoutViewModel() override;

        double viewportOffset() const;
        void setViewportOffset(double viewportOffset);

    Q_SIGNALS:
        void viewportOffsetChanged();

    private:
        double m_viewportOffset;
    };

}

#endif //SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H
