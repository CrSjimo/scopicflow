#ifndef SCOPIC_FLOW_CLAVIERWIDGET_H
#define SCOPIC_FLOW_CLAVIERWIDGET_H

#include <QWidget>

#include <ScopicFlow/ScopicFlowGlobal.h>

class QQuickItem;

namespace sflow {

    class ClavierViewModel;
    class ScrollBehaviorViewModel;
    class AnimationViewModel;
    class PaletteViewModel;

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

        PaletteViewModel *paletteViewModel() const;
        void setPaletteViewModel(PaletteViewModel *viewModel);

        enum LabelStrategy {
            None,
            C,
            All,
        };
        Q_ENUM(LabelStrategy)
        LabelStrategy labelStrategy() const;
        void setLabelStrategy(LabelStrategy strategy);

        int mapToKey(double y) const;

    signals:
        void noteOn(int key);
        void noteOff(int key);
        void noteDoubleClicked(int key);
        void contextMenuRequestedForNote(int key);

    private:
        ClavierQuickItem *d;
    };

} // sflow

#endif //SCOPIC_FLOW_CLAVIERWIDGET_H
