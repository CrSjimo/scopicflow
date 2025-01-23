#ifndef SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_H
#define SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_H

#include <QQuickItem>

namespace sflow {

    class TimeViewModel;
    class TimeLayoutViewModel;

    class TimelineScaleQuickItemPrivate;

    class TimelineScaleQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(TimelineScale)
        Q_PROPERTY(TimeViewModel *timeViewModel READ timeViewModel WRITE setTimeViewModel NOTIFY timeViewModelChanged)
        Q_PROPERTY(TimeLayoutViewModel *timeLayoutViewModel READ timeLayoutViewModel WRITE setTimeLayoutViewModel NOTIFY timeLayoutViewModelChanged)
        Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
        Q_DECLARE_PRIVATE(TimelineScaleQuickItem)
    public:
        explicit TimelineScaleQuickItem(QQuickItem *parent = nullptr);
        ~TimelineScaleQuickItem() override;

        TimeViewModel *timeViewModel() const;
        void setTimeViewModel(TimeViewModel *timeViewModel);

        TimeLayoutViewModel *timeLayoutViewModel() const;
        void setTimeLayoutViewModel(TimeLayoutViewModel *timeLayoutViewModel);

        QColor color() const;
        void setColor(const QColor &color);

    signals:
        void timeViewModelChanged();
        void timeLayoutViewModelChanged();

        void colorChanged(const QColor &foregroundColor);

    protected:
        QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<TimelineScaleQuickItemPrivate> d_ptr;
    };
}

#endif //SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_H
