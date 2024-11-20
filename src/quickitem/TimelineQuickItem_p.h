#ifndef SCOPIC_FLOW_TIMELINEQUICKITEM_P_H
#define SCOPIC_FLOW_TIMELINEQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <ScopicFlow/ScrollBehaviorViewModel.h>
#include <ScopicFlow/AnimationViewModel.h>
#include <ScopicFlow/PaletteViewModel.h>

namespace sflow {

    class PlaybackViewModel;

    class TimelineQuickItemPrivate;

    class TimelineQuickItem : public QQuickItem {
        Q_OBJECT
        Q_PROPERTY(TimeAlignmentViewModel *timeAlignmentViewModel READ timeAlignmentViewModel NOTIFY timeAlignmentViewModelChanged)
        Q_PROPERTY(ScrollBehaviorViewModel *scrollBehaviorViewModel READ scrollBehaviorViewModel NOTIFY scrollBehaviorViewModelChanged)
        Q_PROPERTY(AnimationViewModel *animationViewModel READ animationViewModel NOTIFY animationViewModelChanged)
        Q_PROPERTY(PaletteViewModel *paletteViewModel READ paletteViewModel NOTIFY paletteViewModelChanged)
        Q_PROPERTY(double primaryIndicatorX READ primaryIndicatorX WRITE setPrimaryIndicatorX NOTIFY primaryIndicatorXChanged)
        Q_PROPERTY(double secondaryIndicatorX READ secondaryIndicatorX NOTIFY secondaryIndicatorXChanged)
        Q_PROPERTY(double cursorIndicatorX READ cursorIndicatorX NOTIFY cursorIndicatorXChanged)
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
        Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor NOTIFY foregroundColorChanged)
        Q_DECLARE_PRIVATE(TimelineQuickItem)
    public:
        explicit TimelineQuickItem(QQuickItem *parent = nullptr);
        ~TimelineQuickItem() override;

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *timeAlignmentViewModel);

        PlaybackViewModel *playbackViewModel() const;
        void setPlaybackViewModel(PlaybackViewModel *playbackViewModel);

        ScrollBehaviorViewModel *scrollBehaviorViewModel() const;
        void setScrollBehaviorViewModel(ScrollBehaviorViewModel *scrollBehaviorViewModel);

        AnimationViewModel *animationViewModel() const;
        void setAnimationViewModel(AnimationViewModel *animationViewModel);

        PaletteViewModel *paletteViewModel() const;
        void setPaletteViewModel(PaletteViewModel *paletteViewModel);

        double primaryIndicatorX() const;
        void setPrimaryIndicatorX(double primaryIndicatorX);

        double secondaryIndicatorX() const;

        double cursorIndicatorX() const;

        QColor backgroundColor() const;
        void setBackgroundColor(const QColor &backgroundColor);

        QColor foregroundColor() const;
        void setForegroundColor(const QColor &foregroundColor);

        Q_INVOKABLE int mapToTick(double x) const;
        Q_INVOKABLE double mapToX(int tick) const;

        Q_INVOKABLE double getAlignedX(double x) const;
        Q_INVOKABLE void setZoomedRange(double selectionX, double selectionWidth);
        Q_INVOKABLE void moveViewOnDraggingPositionIndicator(double deltaX);

    signals:
        void timeAlignmentViewModelChanged();
        void primaryIndicatorXChanged(double x);
        void secondaryIndicatorXChanged(double x);
        void cursorIndicatorXChanged(double x);

        void positionIndicatorDoubleClicked();
        void timelineDoubleClicked(int tick);

        void contextMenuRequestedForTimeline(int tick);
        void contextMenuRequestedForPositionIndicator();

        void scrollBehaviorViewModelChanged(ScrollBehaviorViewModel *scrollBehaviorViewModel);
        void animationViewModelChanged(AnimationViewModel *animationViewModel);
        void paletteViewModelChanged(PaletteViewModel *paletteViewModel);

        void backgroundColorChanged(const QColor &backgroundColor);
        void foregroundColorChanged(const QColor &foregroundColor);

    protected:
        QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<TimelineQuickItemPrivate> d_ptr;
    };
}

#endif //SCOPIC_FLOW_TIMELINEQUICKITEM_P_H
