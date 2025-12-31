#ifndef SCOPIC_FLOW_TRACKLISTMANIPULATOR_P_H
#define SCOPIC_FLOW_TRACKLISTMANIPULATOR_P_H

#include <ScopicFlowCore/TrackListManipulator.h>

#include <QVariantAnimation>

namespace sflow {
    class TrackListManipulatorPrivate {
        Q_DECLARE_PUBLIC(TrackListManipulator)
    public:
        TrackListManipulator *q_ptr;

        TrackListLayoutViewModel *trackListLayoutViewModel{};
        ListViewModel *trackListViewModel{};
        QQuickItem *target{};
        double viewSize{};
        bool isViewSizeExplicitSet{};
        double viewportHeight{};

        QVariantAnimation viewportOffsetAnimation;

        void setViewSize(double size);
        void updateParent();
    };
}

#endif //SCOPIC_FLOW_TRACKLISTMANIPULATOR_P_H
