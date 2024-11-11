#ifndef SCOPIC_FLOW_TRACKLISTWIDGET_H
#define SCOPIC_FLOW_TRACKLISTWIDGET_H

#include <QWidget>

#include <ScopicFlow/ScopicFlowGlobal.h>

class QQuickItem;

namespace sflow {

    class TrackListViewModel;
    class ScrollBehaviorViewModel;
    class AnimationViewModel;

    class TrackListQuickItem;

    class SCOPIC_FLOW_EXPORT TrackListWidget : public QWidget {
        Q_OBJECT
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
        Q_PROPERTY(QColor selectedBackgroundColor READ selectedBackgroundColor WRITE setSelectedBackgroundColor)
        Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor)
        Q_PROPERTY(QColor primaryColor READ primaryColor WRITE setPrimaryColor)
        Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
        Q_PROPERTY(QColor muteColor READ muteColor WRITE setMuteColor)
        Q_PROPERTY(QColor soloColor READ soloColor WRITE setSoloColor)
        Q_PROPERTY(QColor recordColor READ recordColor WRITE setRecordColor)
        Q_PROPERTY(QColor levelLowColor READ levelLowColor WRITE setLevelLowColor)
        Q_PROPERTY(QColor levelMiddleColor READ levelMiddleColor WRITE setLevelMiddleColor)
        Q_PROPERTY(QColor levelHighColor READ levelHighColor WRITE setLevelHighColor)
        Q_PROPERTY(QColor levelBackgroundColor READ levelBackgroundColor WRITE setLevelBackgroundColor)
        Q_PROPERTY(QColor levelBorderColor READ levelBorderColor WRITE setLevelBorderColor)
    public:
        explicit TrackListWidget(QWidget *parent = nullptr);
        ~TrackListWidget() override;

        TrackListViewModel *trackListViewModel() const;
        void setTrackListViewModel(TrackListViewModel *trackListViewModel);

        ScrollBehaviorViewModel *scrollBehaviorViewModel() const;
        void setScrollBehaviorViewModel(ScrollBehaviorViewModel *scrollBehaviorViewModel);

        AnimationViewModel *animationViewModel() const;
        void setAnimationViewModel(AnimationViewModel *animationViewModel);

    signals:
        void trackDoubleClicked(int index);
        void contextMenuRequestedForTrack(int index);
        void contextMenuRequestedForTrackDragging(int index, int target);

    private:
        TrackListQuickItem *d;

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

    };

}

#endif //SCOPIC_FLOW_TRACKLISTWIDGET_H
