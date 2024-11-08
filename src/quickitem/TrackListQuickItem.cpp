#include "TrackListQuickItem_p.h"
#include "TrackListQuickItem_p_p.h"

#include <ScopicFlow/TrackViewModel.h>

namespace sflow {
    void TrackListQuickItemPrivate::calculateTotalHeight() {
        totalHeight = 0.0;
        handlePositionList.resize(trackListViewModel->count());
        for (int i = 0; i < trackListViewModel->count(); i++) {
            auto track = trackListViewModel->trackAt(i);
            totalHeight += track->rowHeight();
            handlePositionList[i] = totalHeight;
        }
    }
    TrackListPalette::TrackListPalette(QObject *parent) : QObject(parent) {
    }
    TrackListPalette::~TrackListPalette() = default;
    QColor TrackListPalette::backgroundColor() const {
        return m_backgroundColor;
    }
    void TrackListPalette::setBackgroundColor(const QColor &color) {
        if (m_backgroundColor != color) {
            m_backgroundColor = color;
            emit backgroundColorChanged(color);
        }
    }
    QColor TrackListPalette::selectedBackgroundColor() const {
        return m_selectedBackgroundColor;
    }
    void TrackListPalette::setSelectedBackgroundColor(const QColor &color) {
        if (m_selectedBackgroundColor != color) {
            m_selectedBackgroundColor = color;
            emit selectedBackgroundColorChanged(color);
        }
    }
    QColor TrackListPalette::foregroundColor() const {
        return m_foregroundColor;
    }
    void TrackListPalette::setForegroundColor(const QColor &color) {
        if (m_foregroundColor != color) {
            m_foregroundColor = color;
            emit foregroundColorChanged(color);
        }
    }
    QColor TrackListPalette::primaryColor() const {
        return m_primaryColor;
    }
    void TrackListPalette::setPrimaryColor(const QColor &color) {
        if (m_primaryColor != color) {
            m_primaryColor = color;
            emit primaryColorChanged(color);
        }
    }
    QColor TrackListPalette::borderColor() const {
        return m_borderColor;
    }
    void TrackListPalette::setBorderColor(const QColor &color) {
        if (m_borderColor != color) {
            m_borderColor = color;
            emit borderColorChanged(color);
        }
    }
    QColor TrackListPalette::muteColor() const {
        return m_muteColor;
    }
    void TrackListPalette::setMuteColor(const QColor &color) {
        if (m_muteColor != color) {
            m_muteColor = color;
            emit muteColorChanged(color);
        }
    }
    QColor TrackListPalette::soloColor() const {
        return m_soloColor;
    }
    void TrackListPalette::setSoloColor(const QColor &color) {
        if (m_soloColor != color) {
            m_soloColor = color;
            emit soloColorChanged(color);
        }
    }
    QColor TrackListPalette::recordColor() const {
        return m_recordColor;
    }
    void TrackListPalette::setRecordColor(const QColor &color) {
        if (m_recordColor != color) {
            m_recordColor = color;
            emit recordColorChanged(color);
        }
    }
    QColor TrackListPalette::levelLowColor() const {
        return m_levelLowColor;
    }
    void TrackListPalette::setLevelLowColor(const QColor &color) {
        if (m_levelLowColor != color) {
            m_levelLowColor = color;
            emit levelLowColorChanged(color);
        }
    }
    QColor TrackListPalette::levelMiddleColor() const {
        return m_levelMiddleColor;
    }
    void TrackListPalette::setLevelMiddleColor(const QColor &color) {
        if (m_levelMiddleColor != color) {
            m_levelMiddleColor = color;
            emit levelMiddleColorChanged(color);
        }
    }
    QColor TrackListPalette::levelHighColor() const {
        return m_levelHighColor;
    }
    void TrackListPalette::setLevelHighColor(const QColor &color) {
        if (m_levelHighColor != color) {
            m_levelHighColor = color;
            emit levelHighColorChanged(color);
        }
    }
    QColor TrackListPalette::levelBackgroundColor() const {
        return m_levelBackgroundColor;
    }
    void TrackListPalette::setLevelBackgroundColor(const QColor &color) {
        if (m_levelBackgroundColor != color) {
            m_levelBackgroundColor = color;
            emit levelBackgroundColorChanged(color);
        }
    }
    QColor TrackListPalette::levelBorderColor() const {
        return m_levelBorderColor;
    }
    void TrackListPalette::setLevelBorderColor(const QColor &color) {
        if (m_levelBorderColor != color) {
            m_levelBorderColor = color;
            emit levelBorderColorChanged(color);
        }
    }
    TrackListQuickItem::TrackListQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new TrackListQuickItemPrivate) {
        Q_D(TrackListQuickItem);
        d->q_ptr = this;
        auto defaultPalette = new TrackListPalette;
        defaultPalette->setBackgroundColor(QColor(0x333333));
        defaultPalette->setSelectedBackgroundColor(QColor(0x334444));
        defaultPalette->setForegroundColor(Qt::white);
        defaultPalette->setPrimaryColor(QColor(0x00FFFF));
        defaultPalette->setBorderColor(QColor(0x7F7F7F));
        defaultPalette->setMuteColor(QColor(0xCC6600));
        defaultPalette->setSoloColor(QColor(0x00CC00));
        defaultPalette->setRecordColor(QColor(0xCC3333));
        defaultPalette->setLevelLowColor(QColor(0x33CC33));
        defaultPalette->setLevelMiddleColor(QColor(0xFFCC33));
        defaultPalette->setLevelHighColor(QColor(0xFF3333));
        defaultPalette->setLevelBackgroundColor(QColor(0x222222));
        defaultPalette->setLevelBorderColor(Qt::black);
        d->palette = defaultPalette;
    }
    TrackListQuickItem::~TrackListQuickItem() {
    }
    TrackListPalette *TrackListQuickItem::palette() const {
        Q_D(const TrackListQuickItem);
        return d->palette;
    }
    TrackListViewModel *TrackListQuickItem::trackListViewModel() const {
        Q_D(const TrackListQuickItem);
        return d->trackListViewModel;
    }
    void TrackListQuickItem::setTrackListViewModel(TrackListViewModel *trackListViewModel) {
        Q_D(TrackListQuickItem);
        if (d->trackListViewModel == trackListViewModel)
            return;
        if (d->trackListViewModel) {
            disconnect(d->trackListViewModel, nullptr, this, nullptr);
        }
        d->trackListViewModel = trackListViewModel;
        d->calculateTotalHeight();
        connect(d->trackListViewModel, &TrackListViewModel::tracksInserted, this, [=] {
            d->calculateTotalHeight();
            emit totalHeightChanged();
            emit layoutRequired();
        });
        connect(d->trackListViewModel, &TrackListViewModel::tracksRemoved, this, [=] {
            d->calculateTotalHeight();
            emit totalHeightChanged();
            emit layoutRequired();
        });
        connect(d->trackListViewModel, &TrackListViewModel::tracksRotated, this, &TrackListQuickItem::layoutRequired);
        emit totalHeightChanged();
        emit trackListViewModelChanged();
    }
    QObject *TrackListQuickItem::trackAt(int index) const {
        Q_D(const TrackListQuickItem);
        if (!d->trackListViewModel)
            return nullptr;
        return d->trackListViewModel->trackAt(index);
    }
    double TrackListQuickItem::handlePositionAt(int index) const {
        Q_D(const TrackListQuickItem);
        return d->handlePositionList[index];
    }
} // sflow