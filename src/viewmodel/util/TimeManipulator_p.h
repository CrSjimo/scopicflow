#ifndef SCOPIC_FLOW_TIMEMANIPULATOR_P_H
#define SCOPIC_FLOW_TIMEMANIPULATOR_P_H

#include <ScopicFlow/TimeManipulator.h>

#include <QVariantAnimation>

namespace sflow {
    class TimeManipulatorPrivate {
        Q_DECLARE_PUBLIC(TimeManipulator)
    public:
        TimeManipulator *q_ptr;

        TimeViewModel *timeViewModel{};
        TimeLayoutViewModel *timeLayoutViewModel{};
        double viewSize{};

        QVariantAnimation startAnimation;
        QVariantAnimation pixelDensityAnimation;
        bool currentAnimationFixStartToZero{};
        double animationCenter{};
    };
}

#endif //SCOPIC_FLOW_TIMEMANIPULATOR_P_H
