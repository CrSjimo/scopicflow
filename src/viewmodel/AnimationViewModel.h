#ifndef SCOPIC_FLOW_ANIMATIONVIEWMODEL_H
#define SCOPIC_FLOW_ANIMATIONVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT AnimationViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(double visualEffectAnimationRatio READ visualEffectAnimationRatio WRITE setVisualEffectAnimationRatio NOTIFY visualEffectAnimationRatioChanged)
        Q_PROPERTY(double scrollAnimationRatio READ scrollAnimationRatio WRITE setScrollAnimationRatio NOTIFY scrollAnimationRatioChanged)

    public:
        explicit AnimationViewModel(QObject *parent = nullptr);
        ~AnimationViewModel() override;

        double visualEffectAnimationRatio() const;
        void setVisualEffectAnimationRatio(double value);

        double scrollAnimationRatio() const;
        void setScrollAnimationRatio(double value);

    signals:
        void visualEffectAnimationRatioChanged(double value);
        void scrollAnimationRatioChanged(double value);

    private:
        double m_visualEffectAnimationRatio;
        double m_scrollAnimationRatio;

    };

}
#endif //SCOPIC_FLOW_ANIMATIONVIEWMODEL_H
