#include "ClavierQuickItem_p.h"
#include "ClavierQuickItem_p_p.h"

#include <SVSCraftCore/musicpitch.h>

#include <ScopicFlow/ClavierViewModel.h>

namespace sflow {

    void ClavierQuickItemPrivate::handlePixelDensityAnimation(double centerY, double newPixelDensity) const {
        Q_Q(const ClavierQuickItem);
        auto newStart = currentAnimationFixStartToZero ? 0.0 : currentAnimationEnsureEnd ? 128 - q->height() / newPixelDensity : std::max(0.0, clavierViewModel->start() + centerY / clavierViewModel->pixelDensity() - centerY / newPixelDensity);
        newStart = std::min(newStart, 128 - q->height() / newPixelDensity);
        clavierViewModel->setStart(newStart);
        clavierViewModel->setPixelDensity(newPixelDensity);
    }

    ClavierPalette::ClavierPalette(QObject *parent) : QObject(parent) {
    }
    ClavierPalette::~ClavierPalette() = default;

    QColor ClavierPalette::whiteKeyBackgroundColor() const {
        return m_whiteKeyBackgroundColor;
    }
    void ClavierPalette::setWhiteKeyBackgroundColor(const QColor &color) {
        if (m_whiteKeyBackgroundColor != color) {
            m_whiteKeyBackgroundColor = color;
            emit whiteKeyBackgroundColorChanged(color);
        }
    }
    QColor ClavierPalette::whiteKeyForegroundColor() const {
        return m_whiteKeyForegroundColor;
    }
    void ClavierPalette::setWhiteKeyForegroundColor(const QColor &color) {
        if (m_whiteKeyForegroundColor != color) {
            m_whiteKeyForegroundColor = color;
            emit whiteKeyForegroundColorChanged(color);
        }
    }
    QColor ClavierPalette::whiteKeyTextColor() const {
        return m_whiteKeyTextColor;
    }
    void ClavierPalette::setWhiteKeyTextColor(const QColor &color) {
        if (m_whiteKeyTextColor != color) {
            m_whiteKeyTextColor = color;
            emit whiteKeyTextColorChanged(color);
        }
    }
    QColor ClavierPalette::blackKeyBackgroundColor() const {
        return m_blackKeyBackgroundColor;
    }
    void ClavierPalette::setBlackKeyBackgroundColor(const QColor &color) {
        if (m_blackKeyBackgroundColor != color) {
            m_blackKeyBackgroundColor = color;
            emit blackKeyBackgroundColorChanged(color);
        }
    }
    QColor ClavierPalette::blackKeyForegroundColor() const {
        return m_blackKeyForegroundColor;
    }
    void ClavierPalette::setBlackKeyForegroundColor(const QColor &color) {
        if (m_blackKeyForegroundColor != color) {
            m_blackKeyForegroundColor = color;
            emit blackKeyForegroundColorChanged(color);
        }
    }
    QColor ClavierPalette::blackKeyTextColor() const {
        return m_blackKeyTextColor;
    }
    void ClavierPalette::setBlackKeyTextColor(const QColor &color) {
        if (m_blackKeyTextColor != color) {
            m_blackKeyTextColor = color;
            emit blackKeyTextColorChanged(color);
        }
    }
    QColor ClavierPalette::borderColor() const {
        return m_borderColor;
    }
    void ClavierPalette::setBorderColor(const QColor &color) {
        if (m_borderColor != color) {
            m_borderColor = color;
            emit borderColorChanged(color);
        }
    }

    ClavierQuickItem::ClavierQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new ClavierQuickItemPrivate) {
        Q_D(ClavierQuickItem);
        d->q_ptr = this;
        auto defaultPalette = new ClavierPalette;
        defaultPalette->setWhiteKeyBackgroundColor(Qt::white);
        defaultPalette->setWhiteKeyForegroundColor(Qt::cyan);
        defaultPalette->setWhiteKeyTextColor(Qt::black);
        defaultPalette->setBlackKeyBackgroundColor(Qt::black);
        defaultPalette->setBlackKeyForegroundColor(Qt::cyan);
        defaultPalette->setBlackKeyTextColor(Qt::white);
        defaultPalette->setBorderColor(Qt::darkGray);
        d->palette = defaultPalette;

        d->startAnimation = new QVariantAnimation;
        d->startAnimation->setEasingCurve(QEasingCurve::OutCubic);
        d->pixelDensityAnimation = new QVariantAnimation;
        d->pixelDensityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    }
    ClavierQuickItem::~ClavierQuickItem() = default;

    ClavierPalette *ClavierQuickItem::palette() const {
        Q_D(const ClavierQuickItem);
        return d->palette;
    }
    ClavierViewModel *ClavierQuickItem::clavierViewModel() const {
        Q_D(const ClavierQuickItem);
        return d->clavierViewModel;
    }
    void ClavierQuickItem::setClavierViewModel(ClavierViewModel *clavierViewModel) {
        Q_D(ClavierQuickItem);
        if (d->clavierViewModel) {
            disconnect(d->clavierViewModel, nullptr, this, nullptr);
            disconnect(d->startAnimation, nullptr, d->clavierViewModel, nullptr);
            disconnect(d->pixelDensityAnimation, nullptr, d->clavierViewModel, nullptr);
        }
        d->clavierViewModel = clavierViewModel;
        if (clavierViewModel) {
            connect(clavierViewModel, &ClavierViewModel::startChanged, this, [=] {
                emit viewportYChanged(viewportY());
            });
            connect(clavierViewModel, &ClavierViewModel::pixelDensityChanged, this, [=] {
                emit keyHeightChanged(keyHeight());
                emit viewportYChanged(viewportY());
            });
            connect(clavierViewModel, &ClavierViewModel::cursorPositionChanged, this, &ClavierQuickItem::cursorNoteIndexChanged);
            connect(clavierViewModel, &ClavierViewModel::accidentalTypeChanged, this, &ClavierQuickItem::keyNameUpdated);
            connect(d->startAnimation, &QVariantAnimation::valueChanged, d->clavierViewModel, [=](const QVariant &value) {
                d->clavierViewModel->setStart(value.toDouble());
            });
            connect(d->pixelDensityAnimation, &QVariantAnimation::valueChanged, d->clavierViewModel, [=](const QVariant &value) {
                auto [centerY, pixelDensity] = value.toSizeF();
                d->handlePixelDensityAnimation(centerY, pixelDensity);
            });
        }
        emit keyHeightChanged(keyHeight());
        emit viewportYChanged(viewportY());
        emit cursorNoteIndexChanged(cursorNoteIndex());
        emit keyNameUpdated();
    }
    WheelModifierViewModel *ClavierQuickItem::wheelModifierViewModel() const {
        Q_D(const ClavierQuickItem);
        return d->wheelModifierViewModel;
    }
    void ClavierQuickItem::setWheelModifierViewModel(WheelModifierViewModel *wheelModifierViewModel) {
        Q_D(ClavierQuickItem);
        if (d->wheelModifierViewModel != wheelModifierViewModel) {
            d->wheelModifierViewModel = wheelModifierViewModel;
            emit wheelModifierViewModelChanged(wheelModifierViewModel);
        }
    }
    double ClavierQuickItem::keyHeight() const {
        Q_D(const ClavierQuickItem);
        if (!d->clavierViewModel)
            return 24;
        return d->clavierViewModel->pixelDensity();
    }
    double ClavierQuickItem::viewportY() const {
        Q_D(const ClavierQuickItem);
        if (!d->clavierViewModel)
            return 0;
        return height() - (128 - d->clavierViewModel->start()) * d->clavierViewModel->pixelDensity();
    }
    int ClavierQuickItem::cursorNoteIndex() const {
        Q_D(const ClavierQuickItem);
        if (!d->clavierViewModel)
            return -1;
        return d->clavierViewModel->cursorPosition();
    }
    ClavierQuickItem::LabelStrategy ClavierQuickItem::labelStrategy() const {
        Q_D(const ClavierQuickItem);
        return d->labelStrategy;
    }
    void ClavierQuickItem::setLabelStrategy(LabelStrategy strategy) {
        Q_D(ClavierQuickItem);
        if (d->labelStrategy != strategy) {
            d->labelStrategy = strategy;
            emit labelStrategyChanged(strategy);
        }
    }
    int ClavierQuickItem::mapToKey(double y) const {
        Q_D(const ClavierQuickItem);
        return 127 - static_cast<int>(std::floor((y - viewportY()) / keyHeight()));
    }
    QString ClavierQuickItem::dummyKeyName() const {
        return {};
    }
    void ClavierQuickItem::moveViewBy(double deltaY, bool isAnimated) {
        Q_D(ClavierQuickItem);
        if (!d->clavierViewModel)
            return;
        d->startAnimation->stop();
        auto newStart = std::max(0.0, d->clavierViewModel->start() -
                                          deltaY / d->clavierViewModel->pixelDensity());
        newStart = std::min(newStart, 128 - height() / d->clavierViewModel->pixelDensity());

        if (!isAnimated) {
            d->clavierViewModel->setStart(newStart);
        } else {
            d->startAnimation->setStartValue(d->clavierViewModel->start());
            d->startAnimation->setEndValue(newStart);
            d->startAnimation->start();
        }
    }
    void ClavierQuickItem::zoomOnWheel(double ratio, double centerY, bool animated) {
        Q_D(ClavierQuickItem);
        if (!d->clavierViewModel)
            return;
        d->startAnimation->stop();
        d->pixelDensityAnimation->stop();
        auto newPixelDensity = qBound(d->clavierViewModel->minimumPixelDensity(),
                                      d->clavierViewModel->pixelDensity() * ratio,
                                      d->clavierViewModel->maximumPixelDensity());
        auto newStart = std::max(0.0, d->clavierViewModel->start() +
                                          centerY / d->clavierViewModel->pixelDensity() -
                                          centerY / newPixelDensity);
        newStart = std::min(newStart, 128 - height() / newPixelDensity);
        if (!animated) {
            d->clavierViewModel->setStart(newStart);
            d->clavierViewModel->setPixelDensity(newPixelDensity);
        } else {
            d->currentAnimationFixStartToZero = qFuzzyIsNull(d->clavierViewModel->start());
            d->currentAnimationEnsureEnd = qFuzzyCompare(
                d->clavierViewModel->start(), 128 - height() / d->clavierViewModel->pixelDensity());
            d->pixelDensityAnimation->setStartValue(
                QSizeF(centerY, d->clavierViewModel->pixelDensity()));
            d->pixelDensityAnimation->setEndValue(QSizeF(centerY, newPixelDensity));
            d->pixelDensityAnimation->start();
        }
    }
    QString ClavierQuickItem::keyName(int key) const {
        Q_D(const ClavierQuickItem);
        if (!d->clavierViewModel)
            return {};
        SVS::MusicPitch musicPitch(key);
        return musicPitch.toString(static_cast<SVS::MusicPitch::Accidental>(d->clavierViewModel->accidentalType()));
    }

}