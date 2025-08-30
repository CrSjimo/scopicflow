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
        double viewSize{};
        bool isViewSizeExplicitSet{};
        double bottomExpansion{};
        double topExpansion{};

        QVariantAnimation startAnimation;
        QVariantAnimation pixelDensityAnimation;
        bool currentAnimationFixStartToZero{};
        bool currentAnimationEnsureEnd{};
        double animationCenterY{};

        void setViewSize(double size);
        void updateParent();
    };
}

#endif //SCOPIC_FLOW_CLAVIERMANIPULATOR_P_H
