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
    WheelModifierViewModel *ClavierWidget::wheelModifierViewModel() const {
        return d->wheelModifierViewModel();
    }
    void ClavierWidget::setWheelModifierViewModel(WheelModifierViewModel *viewModel) {
        d->setWheelModifierViewModel(viewModel);
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
    QColor ClavierWidget::whiteKeyBackgroundColor() const {
        return d->palette()->whiteKeyBackgroundColor();
    }
    void ClavierWidget::setWhiteKeyBackgroundColor(const QColor &color) {
        d->palette()->setWhiteKeyBackgroundColor(color);
    }
    QColor ClavierWidget::whiteKeyForegroundColor() const {
        return d->palette()->whiteKeyForegroundColor();
    }
    void ClavierWidget::setWhiteKeyForegroundColor(const QColor &color) {
        d->palette()->setWhiteKeyForegroundColor(color);
    }
    QColor ClavierWidget::whiteKeyTextColor() const {
        return d->palette()->whiteKeyTextColor();
    }
    void ClavierWidget::setWhiteKeyTextColor(const QColor &color) {
        d->palette()->setWhiteKeyTextColor(color);
    }
    QColor ClavierWidget::blackKeyBackgroundColor() const {
        return d->palette()->blackKeyBackgroundColor();
    }
    void ClavierWidget::setBlackKeyBackgroundColor(const QColor &color) {
        d->palette()->setBlackKeyBackgroundColor(color);
    }
    QColor ClavierWidget::blackKeyForegroundColor() const {
        return d->palette()->blackKeyForegroundColor();
    }
    void ClavierWidget::setBlackKeyForegroundColor(const QColor &color) {
        d->palette()->setBlackKeyForegroundColor(color);
    }
    QColor ClavierWidget::blackKeyTextColor() const {
        return d->palette()->blackKeyTextColor();
    }
    void ClavierWidget::setBlackKeyTextColor(const QColor &color) {
        d->palette()->setBlackKeyTextColor(color);
    }
    QColor ClavierWidget::borderColor() const {
        return d->palette()->borderColor();
    }
    void ClavierWidget::setBorderColor(const QColor &color) {
        d->palette()->setBorderColor(color);
    }
}