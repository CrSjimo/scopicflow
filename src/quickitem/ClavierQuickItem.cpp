#include "ClavierQuickItem_p.h"
#include "ClavierQuickItem_p_p.h"

#include <SVSCraftCore/musicpitch.h>

#include <ScopicFlow/ClavierViewModel.h>

namespace sflow {

    ClavierQuickItem::ClavierQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new ClavierQuickItemPrivate) {
        Q_D(ClavierQuickItem);
        d->q_ptr = this;
    }
    ClavierQuickItem::~ClavierQuickItem() = default;

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
    PaletteViewModel *ClavierQuickItem::paletteViewModel() const {
        Q_D(const ClavierQuickItem);
        return d->paletteViewModel;
    }
    void ClavierQuickItem::setPaletteViewModel(PaletteViewModel *paletteViewModel) {
        Q_D(ClavierQuickItem);
        if (d->paletteViewModel == paletteViewModel)
            return;
        d->paletteViewModel = paletteViewModel;
        emit paletteViewModelChanged(paletteViewModel);
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