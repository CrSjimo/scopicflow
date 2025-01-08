#ifndef SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_H
#define SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/TimeAlignmentViewModel.h>

namespace sflow {

    class TimelineScaleQuickItemPrivate;

    class TimelineScaleQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(TimelineScale)
        Q_PROPERTY(TimeAlignmentViewModel *timeAlignmentViewModel READ timeAlignmentViewModel WRITE setTimeAlignmentViewModel NOTIFY timeAlignmentViewModelChanged)
        Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
        Q_DECLARE_PRIVATE(TimelineScaleQuickItem)
    public:
        explicit TimelineScaleQuickItem(QQuickItem *parent = nullptr);
        ~TimelineScaleQuickItem() override;

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *timeAlignmentViewModel);

        QColor color() const;
        void setColor(const QColor &color);

    signals:
        void timeAlignmentViewModelChanged();

        void colorChanged(const QColor &foregroundColor);

    protected:
        QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<TimelineScaleQuickItemPrivate> d_ptr;
    };
}

#endif //SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_H
