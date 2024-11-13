#ifndef SCOPIC_FLOW_CLAVIERWIDGET_H
#define SCOPIC_FLOW_CLAVIERWIDGET_H

#include <QWidget>

#include <ScopicFlow/ScopicFlowGlobal.h>

class QQuickItem;

namespace sflow {

    class ClavierViewModel;
    class ScrollBehaviorViewModel;
    class AnimationViewModel;

    class ClavierQuickItem;

    class SCOPIC_FLOW_EXPORT ClavierWidget : public QWidget {
        Q_OBJECT

    public:
        explicit ClavierWidget(QWidget *parent = nullptr);
        ~ClavierWidget() override;

        ClavierViewModel *clavierViewModel() const;
        void setClavierViewModel(ClavierViewModel *viewModel);

        ScrollBehaviorViewModel *scrollBehaviorViewModel() const;
        void setScrollBehaviorViewModel(ScrollBehaviorViewModel *viewModel);

        AnimationViewModel *animationViewModel() const;
        void setAnimationViewModel(AnimationViewModel *viewModel);

        enum LabelStrategy {
            None,
            C,
            All,
        };
        Q_ENUM(LabelStrategy)
        LabelStrategy labelStrategy() const;
        void setLabelStrategy(LabelStrategy strategy);

        int mapToKey(double y) const;

    private:
        ClavierQuickItem *d;
    };

} // sflow

#endif //SCOPIC_FLOW_CLAVIERWIDGET_H
