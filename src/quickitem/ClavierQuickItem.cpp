#include "ClavierQuickItem_p.h"
#include "ClavierQuickItem_p_p.h"

#include <SVSCraftCore/musicpitch.h>

#include <ScopicFlow/ClavierViewModel.h>

namespace sflow {

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
        if (d->clavierViewModel == clavierViewModel)
            return;
        if (d->clavierViewModel) {
            disconnect(d->clavierViewModel, nullptr, this, nullptr);
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
        }
        emit clavierViewModelChanged(clavierViewModel);
        emit keyHeightChanged(keyHeight());
        emit viewportYChanged(viewportY());
        emit cursorNoteIndexChanged(cursorNoteIndex());
        emit keyNameUpdated();
    }
    ScrollBehaviorViewModel *ClavierQuickItem::scrollBehaviorViewModel() const {
        Q_D(const ClavierQuickItem);
        return d->scrollBehaviorViewModel;
    }
    void ClavierQuickItem::setScrollBehaviorViewModel(ScrollBehaviorViewModel *scrollBehaviorViewModel) {
        Q_D(ClavierQuickItem);
        if (d->scrollBehaviorViewModel != scrollBehaviorViewModel) {
            d->scrollBehaviorViewModel = scrollBehaviorViewModel;
            emit scrollBehaviorViewModelChanged(scrollBehaviorViewModel);
        }
    }
    AnimationViewModel *ClavierQuickItem::animationViewModel() const {
        Q_D(const ClavierQuickItem);
        return d->animationViewModel;
    }
    void ClavierQuickItem::setAnimationViewModel(AnimationViewModel *animationViewModel) {
        Q_D(ClavierQuickItem);
        if (d->animationViewModel == animationViewModel)
            return;
        d->animationViewModel = animationViewModel;
        emit animationViewModelChanged(animationViewModel);
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
    QString ClavierQuickItem::keyName(int key) const {
        Q_D(const ClavierQuickItem);
        if (!d->clavierViewModel)
            return {};
        SVS::MusicPitch musicPitch(key);
        return musicPitch.toString(static_cast<SVS::MusicPitch::Accidental>(d->clavierViewModel->accidentalType()));
    }

}