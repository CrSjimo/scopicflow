#ifndef SCOPIC_FLOW_TRACKLISTMANIPULATOR_P_H
#define SCOPIC_FLOW_TRACKLISTMANIPULATOR_P_H

#include <ScopicFlowCore/TrackListManipulator.h>

#include <QHash>
#include <QMetaObject>
#include <QVariantAnimation>
#include <QVector>

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

        mutable QVector<double> prefixSums;
        mutable bool prefixDirty{true};
        mutable QHash<QObject *, double> itemHeights;
        QVector<QMetaObject::Connection> itemConnections;
        QMetaObject::Connection itemsChangedConnection;

        QVariantAnimation viewportOffsetAnimation;

        void setViewSize(double size);
        void updateParent();
        void setViewportHeight(double height);
        double itemHeight(QObject *object) const;
        void clearItemConnections();
        void rebuildFromModel();
        void handleRowHeightChanged(QObject *object);
        void ensurePrefixReady() const;
    };
}

#endif //SCOPIC_FLOW_TRACKLISTMANIPULATOR_P_H
