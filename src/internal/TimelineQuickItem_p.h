#ifndef SCOPIC_FLOW_TIMELINEQUICKITEM_P_H
#define SCOPIC_FLOW_TIMELINEQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/TimeAlignmentViewModel.h>

namespace sflow {

    class TimelineQuickItemPrivate;

    class TimelineQuickItem : public QQuickItem {
        Q_OBJECT
        Q_PROPERTY(TimeAlignmentViewModel *timeAlignmentViewModel READ timeAlignmentViewModel WRITE setTimeAlignmentViewModel NOTIFY timeAlignmentViewModelChanged)
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
        Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor NOTIFY foregroundColorChanged)
        Q_DECLARE_PRIVATE(TimelineQuickItem)
    public:
        explicit TimelineQuickItem(QQuickItem *parent = nullptr);
        ~TimelineQuickItem() override;

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *timeAlignmentViewModel);

        void setPrimaryIndicatorX(double primaryIndicatorX);

        QColor backgroundColor() const;
        void setBackgroundColor(const QColor &backgroundColor);

        QColor foregroundColor() const;
        void setForegroundColor(const QColor &foregroundColor);

    signals:
        void timeAlignmentViewModelChanged();

        void backgroundColorChanged(const QColor &backgroundColor);
        void foregroundColorChanged(const QColor &foregroundColor);

    protected:
        QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<TimelineQuickItemPrivate> d_ptr;
    };
}

#endif //SCOPIC_FLOW_TIMELINEQUICKITEM_P_H
