#ifndef SCOPIC_FLOW_TIMELINEQUICKITEM_P_H
#define SCOPIC_FLOW_TIMELINEQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <ScopicFlow/ScrollBehaviorViewModel.h>
#include <ScopicFlow/AnimationViewModel.h>

namespace sflow {

    class PlaybackViewModel;

    class TimelinePalette : public QObject {
        Q_OBJECT
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
        Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor NOTIFY foregroundColorChanged)
        Q_PROPERTY(QColor positionIndicatorColor READ positionIndicatorColor WRITE setPositionIndicatorColor NOTIFY positionIndicatorColorChanged)
        Q_PROPERTY(QColor cursorIndicatorColor READ cursorIndicatorColor WRITE setCursorIndicatorColor NOTIFY cursorIndicatorColorChanged)

    public:
        explicit TimelinePalette(QObject *parent = nullptr);
        ~TimelinePalette() override;

        QColor backgroundColor() const;
        void setBackgroundColor(const QColor &color);

        QColor foregroundColor() const;
        void setForegroundColor(const QColor &color);

        QColor positionIndicatorColor() const;
        void setPositionIndicatorColor(const QColor &color);

        QColor cursorIndicatorColor() const;
        void setCursorIndicatorColor(const QColor &color);

    signals:
        void backgroundColorChanged(const QColor &color);
        void foregroundColorChanged(const QColor &color);
        void positionIndicatorColorChanged(const QColor &color);
        void cursorIndicatorColorChanged(const QColor &color);

    private:
        QColor m_backgroundColor;
        QColor m_foregroundColor;
        QColor m_positionIndicatorColor;
        QColor m_cursorIndicatorColor;
    };

    class TimelineQuickItemPrivate;

    class TimelineQuickItem : public QQuickItem {
        Q_OBJECT
        Q_PROPERTY(TimelinePalette *palette READ palette CONSTANT)
        Q_PROPERTY(TimeAlignmentViewModel *timeAlignmentViewModel READ timeAlignmentViewModel NOTIFY timeAlignmentViewModelChanged)
        Q_PROPERTY(ScrollBehaviorViewModel *scrollBehaviorViewModel READ scrollBehaviorViewModel NOTIFY scrollBehaviorViewModelChanged)
        Q_PROPERTY(AnimationViewModel *animationViewModel READ animationViewModel NOTIFY animationViewModelChanged)
        Q_PROPERTY(double primaryIndicatorX READ primaryIndicatorX WRITE setPrimaryIndicatorX NOTIFY primaryIndicatorXChanged)
        Q_PROPERTY(double secondaryIndicatorX READ secondaryIndicatorX NOTIFY secondaryIndicatorXChanged)
        Q_PROPERTY(double cursorIndicatorX READ cursorIndicatorX NOTIFY cursorIndicatorXChanged)
        Q_DECLARE_PRIVATE(TimelineQuickItem)
    public:
        explicit TimelineQuickItem(QQuickItem *parent = nullptr);
        ~TimelineQuickItem() override;

        TimelinePalette *palette() const;

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *timeAlignmentViewModel);

        PlaybackViewModel *playbackViewModel() const;
        void setPlaybackViewModel(PlaybackViewModel *playbackViewModel);

        ScrollBehaviorViewModel *scrollBehaviorViewModel() const;
        void setScrollBehaviorViewModel(ScrollBehaviorViewModel *scrollBehaviorViewModel);

        AnimationViewModel *animationViewModel() const;
        void setAnimationViewModel(AnimationViewModel *animationViewModel);

        double primaryIndicatorX() const;
        void setPrimaryIndicatorX(double primaryIndicatorX);

        double secondaryIndicatorX() const;

        double cursorIndicatorX() const;

        int mapToTick(double x) const;
        double mapToX(int tick) const;

        Q_INVOKABLE void handleContextMenuRequest(double x);
        Q_INVOKABLE double getAlignedX(double x) const;
        Q_INVOKABLE void setZoomedRange(double selectionX, double selectionWidth);
        Q_INVOKABLE void moveViewOnDraggingPositionIndicator(double deltaX);

    signals:
        void timeAlignmentViewModelChanged();
        void primaryIndicatorXChanged(double x);
        void secondaryIndicatorXChanged(double x);
        void cursorIndicatorXChanged(double x);

        void positionIndicatorDoubleClicked();

        void contextMenuRequestedForTimeline(int tick);
        void contextMenuRequestedForPositionIndicator();

        void scrollBehaviorViewModelChanged(ScrollBehaviorViewModel *scrollBehaviorViewModel);
        void animationViewModelChanged(AnimationViewModel *animationViewModel);

    protected:
        QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<TimelineQuickItemPrivate> d_ptr;
    };
}

#endif //SCOPIC_FLOW_TIMELINEQUICKITEM_P_H
