#ifndef SCOPIC_FLOW_CLAVIERMANIPULATOR_P_H
#define SCOPIC_FLOW_CLAVIERMANIPULATOR_P_H

#include <ScopicFlowCore/ClavierManipulator.h>

#include <QVariantAnimation>

namespace sflow {
    class ClavierManipulatorPrivate {
        Q_DECLARE_PUBLIC(ClavierManipulator)
    public:
        ClavierManipulator *q_ptr;

        ClavierViewModel *clavierViewModel{};
        QQuickItem *target{};
        double viewSize{};
        bool isViewSizeExplicitSet{};

        QVariantAnimation startAnimation;
        QVariantAnimation pixelDensityAnimation;
        double animationCenter{};

        void setViewSize(double size);
        void updateParent();
    };
}

#endif //SCOPIC_FLOW_CLAVIERMANIPULATOR_P_H
