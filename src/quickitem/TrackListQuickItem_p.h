#ifndef SCOPIC_FLOW_TRACKLISTQUICKITEM_P_H
#define SCOPIC_FLOW_TRACKLISTQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/TrackListViewModel.h>
#include <ScopicFlow/WheelModifierViewModel.h>
#include <ScopicFlow/AnimationViewModel.h>

namespace sflow {

    class TrackListPalette : public QObject {
        Q_OBJECT
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
        Q_PROPERTY(QColor selectedBackgroundColor READ selectedBackgroundColor WRITE setSelectedBackgroundColor NOTIFY selectedBackgroundColorChanged)
        Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor NOTIFY foregroundColorChanged)
        Q_PROPERTY(QColor primaryColor READ primaryColor WRITE setPrimaryColor NOTIFY primaryColorChanged)
        Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
        Q_PROPERTY(QColor muteColor READ muteColor WRITE setMuteColor NOTIFY muteColorChanged)
        Q_PROPERTY(QColor soloColor READ soloColor WRITE setSoloColor NOTIFY soloColorChanged)
        Q_PROPERTY(QColor recordColor READ recordColor WRITE setRecordColor NOTIFY recordColorChanged)
        Q_PROPERTY(QColor levelLowColor READ levelLowColor WRITE setLevelLowColor NOTIFY levelLowColorChanged)
        Q_PROPERTY(QColor levelMiddleColor READ levelMiddleColor WRITE setLevelMiddleColor NOTIFY levelMiddleColorChanged)
        Q_PROPERTY(QColor levelHighColor READ levelHighColor WRITE setLevelHighColor NOTIFY levelHighColorChanged)
        Q_PROPERTY(QColor levelBackgroundColor READ levelBackgroundColor WRITE setLevelBackgroundColor NOTIFY levelBackgroundColorChanged)
        Q_PROPERTY(QColor levelBorderColor READ levelBorderColor WRITE setLevelBorderColor NOTIFY levelBorderColorChanged)
    public:
        explicit TrackListPalette(QObject *parent = nullptr);
        ~TrackListPalette() override;

        QColor backgroundColor() const;
        void setBackgroundColor(const QColor &color);

        QColor selectedBackgroundColor() const;
        void setSelectedBackgroundColor(const QColor &color);

        QColor foregroundColor() const;
        void setForegroundColor(const QColor &color);

        QColor primaryColor() const;
        void setPrimaryColor(const QColor &color);

        QColor borderColor() const;
        void setBorderColor(const QColor &color);

        QColor muteColor() const;
        void setMuteColor(const QColor &color);

        QColor soloColor() const;
        void setSoloColor(const QColor &color);

        QColor recordColor() const;
        void setRecordColor(const QColor &color);

        QColor levelLowColor() const;
        void setLevelLowColor(const QColor &color);

        QColor levelMiddleColor() const;
        void setLevelMiddleColor(const QColor &color);

        QColor levelHighColor() const;
        void setLevelHighColor(const QColor &color);

        QColor levelBackgroundColor() const;
        void setLevelBackgroundColor(const QColor &color);

        QColor levelBorderColor() const;
        void setLevelBorderColor(const QColor &color);

    signals:
        void backgroundColorChanged(const QColor &color);
        void selectedBackgroundColorChanged(const QColor &color);
        void foregroundColorChanged(const QColor &color);
        void primaryColorChanged(const QColor &color);
        void borderColorChanged(const QColor &color);
        void muteColorChanged(const QColor &color);
        void soloColorChanged(const QColor &color);
        void recordColorChanged(const QColor &color);
        void levelLowColorChanged(const QColor &color);
        void levelMiddleColorChanged(const QColor &color);
        void levelHighColorChanged(const QColor &color);
        void levelBackgroundColorChanged(const QColor &color);
        void levelBorderColorChanged(const QColor &color);

    private:
        QColor m_backgroundColor;
        QColor m_selectedBackgroundColor;
        QColor m_foregroundColor;
        QColor m_primaryColor;
        QColor m_borderColor;
        QColor m_muteColor;
        QColor m_soloColor;
        QColor m_recordColor;
        QColor m_levelLowColor;
        QColor m_levelMiddleColor;
        QColor m_levelHighColor;
        QColor m_levelBackgroundColor;
        QColor m_levelBorderColor;
    };

    class TrackListQuickItemPrivate;

    class SCOPIC_FLOW_EXPORT TrackListQuickItem : public QQuickItem {
        Q_OBJECT
        Q_DECLARE_PRIVATE(TrackListQuickItem)
        Q_PROPERTY(TrackListPalette *palette READ palette CONSTANT)
        Q_PROPERTY(TrackListViewModel *trackListViewModel READ trackListViewModel NOTIFY trackListViewModelChanged)
        Q_PROPERTY(WheelModifierViewModel *wheelModifierViewModel READ wheelModifierViewModel NOTIFY wheelModifierViewModelChanged)
        Q_PROPERTY(AnimationViewModel *animationViewModel READ animationViewModel NOTIFY animationViewModelChanged)
    public:
        explicit TrackListQuickItem(QQuickItem *parent = nullptr);
        ~TrackListQuickItem() override;

        TrackListPalette *palette() const;

        TrackListViewModel *trackListViewModel() const;
        void setTrackListViewModel(TrackListViewModel *trackListViewModel);

        WheelModifierViewModel *wheelModifierViewModel() const;
        void setWheelModifierViewModel(WheelModifierViewModel *wheelModifierViewModel);

        AnimationViewModel *animationViewModel() const;
        void setAnimationViewModel(AnimationViewModel *animationViewModel);

        Q_INVOKABLE QObject *trackAt(int index) const;
        Q_INVOKABLE double handlePositionAt(int index) const;
        Q_INVOKABLE void handleTrackMoved(int index, int target);

    signals:
        void trackListViewModelChanged(TrackListViewModel *trackListViewModel);
        void wheelModifierViewModelChanged(WheelModifierViewModel *wheelModifierViewModel);
        void animationViewModelChanged(AnimationViewModel *animationViewModel);
        void layoutRequired();
        void trackDoubleClicked(int index);
        void contextMenuRequestedForTrack(int index);
        void contextMenuRequestedForTrackDragging(int index, int target);

    private:
        QScopedPointer<TrackListQuickItemPrivate> d_ptr;

    };

} // sflow

#endif //SCOPIC_FLOW_TRACKLISTQUICKITEM_P_H
