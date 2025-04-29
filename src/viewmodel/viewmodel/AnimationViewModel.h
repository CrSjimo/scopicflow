#ifndef SCOPIC_FLOW_ANIMATIONVIEWMODEL_H
#define SCOPIC_FLOW_ANIMATIONVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT AnimationViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(double visualEffectAnimationRatio READ visualEffectAnimationRatio WRITE setVisualEffectAnimationRatio NOTIFY visualEffectAnimationRatioChanged)
        Q_PROPERTY(double scrollAnimationRatio READ scrollAnimationRatio WRITE setScrollAnimationRatio NOTIFY scrollAnimationRatioChanged)
        Q_PROPERTY(double colorAnimationRatio READ colorAnimationRatio WRITE setColorAnimationRatio NOTIFY colorAnimationRatioChanged)

    public:
        explicit AnimationViewModel(QObject *parent = nullptr);
        ~AnimationViewModel() override;

        double visualEffectAnimationRatio() const;
        void setVisualEffectAnimationRatio(double value);

        double scrollAnimationRatio() const;
        void setScrollAnimationRatio(double value);

        double colorAnimationRatio() const;
        void setColorAnimationRatio(double value);

    signals:
        void visualEffectAnimationRatioChanged(double value);
        void scrollAnimationRatioChanged(double value);
        void colorAnimationRatioChanged(double value);

    private:
        double m_visualEffectAnimationRatio;
        double m_scrollAnimationRatio;
        double m_colorAnimationRatio;

    };

}
#endif //SCOPIC_FLOW_ANIMATIONVIEWMODEL_H
