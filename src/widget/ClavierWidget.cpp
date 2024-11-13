#include "ClavierWidget.h"

#include <QVBoxLayout>

#include <ScopicFlow/private/ClavierQuickItem_p.h>

#include "QuickWrapperHelper_p.h"

namespace sflow {
    ClavierWidget::ClavierWidget(QWidget *parent) : QWidget(parent) {
        static int _ = qmlRegisterType<ClavierQuickItem>("ScopicFlowPrivate", 1, 0, "Clavier");
        auto wrapper = QuickWrapperHelper::wrap("Clavier");
        auto layout = new QVBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(wrapper.first);
        setMinimumWidth(96);
        setLayout(layout);
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

        d = static_cast<ClavierQuickItem *>(wrapper.second);

    }
    ClavierWidget::~ClavierWidget() = default;

    ClavierViewModel *ClavierWidget::clavierViewModel() const {
        return d->clavierViewModel();
    }
    void ClavierWidget::setClavierViewModel(ClavierViewModel *viewModel) {
        d->setClavierViewModel(viewModel);
    }
    ScrollBehaviorViewModel *ClavierWidget::scrollBehaviorViewModel() const {
        return d->scrollBehaviorViewModel();
    }
    void ClavierWidget::setScrollBehaviorViewModel(ScrollBehaviorViewModel *viewModel) {
        d->setScrollBehaviorViewModel(viewModel);
    }
    AnimationViewModel *ClavierWidget::animationViewModel() const {
        return d->animationViewModel();
    }
    void ClavierWidget::setAnimationViewModel(AnimationViewModel *viewModel) {
        d->setAnimationViewModel(viewModel);
    }
    ClavierWidget::LabelStrategy ClavierWidget::labelStrategy() const {
        return static_cast<LabelStrategy>(d->labelStrategy());
    }
    void ClavierWidget::setLabelStrategy(LabelStrategy strategy) {
        d->setLabelStrategy(static_cast<ClavierQuickItem::LabelStrategy>(strategy));
    }
    int ClavierWidget::mapToKey(double y) const {
        return d->mapToKey(y);
    }
}