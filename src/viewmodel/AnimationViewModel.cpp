#include "AnimationViewModel.h"

namespace sflow {
    AnimationViewModel::AnimationViewModel(QObject *parent) : QObject(parent), m_visualEffectAnimationRatio(1.0), m_scrollAnimationRatio(1.0) {
    }
    AnimationViewModel::~AnimationViewModel() = default;
    double AnimationViewModel::visualEffectAnimationRatio() const {
        return m_visualEffectAnimationRatio;
    }
    void AnimationViewModel::setVisualEffectAnimationRatio(double value) {
        if (m_visualEffectAnimationRatio != value) {
            m_visualEffectAnimationRatio = value;
            emit visualEffectAnimationRatioChanged(value);
        }
    }
    double AnimationViewModel::scrollAnimationRatio() const {
        return m_scrollAnimationRatio;
    }
    void AnimationViewModel::setScrollAnimationRatio(double value) {
        if (m_scrollAnimationRatio != value) {
            m_scrollAnimationRatio = value;
            emit scrollAnimationRatioChanged(value);
        }
    }
}