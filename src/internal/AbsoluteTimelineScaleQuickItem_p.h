#ifndef SCOPIC_FLOW_ABSOLUTETIMELINESCALEQUICKITEM_P_H
#define SCOPIC_FLOW_ABSOLUTETIMELINESCALEQUICKITEM_P_H

#include <QQuickItem>

namespace sflow {

    class TimeLayoutViewModel;
    class TimeViewModel;

    class AbsoluteTimelineScaleQuickItemPrivate;

    class AbsoluteTimelineScaleQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(AbsoluteTimelineScale)
        Q_PROPERTY(TimeViewModel *timeViewModel READ timeViewModel WRITE setTimeViewModel NOTIFY timeViewModelChanged)
        Q_PROPERTY(TimeLayoutViewModel *timeLayoutViewModel READ timeLayoutViewModel WRITE setTimeLayoutViewModel NOTIFY timeLayoutViewModelChanged)
        Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
        Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
        Q_DECLARE_PRIVATE(AbsoluteTimelineScaleQuickItem)
        Q_PRIVATE_SLOT(d_func(), void updateTimeline())
    public:
        explicit AbsoluteTimelineScaleQuickItem(QQuickItem *parent = nullptr);
        ~AbsoluteTimelineScaleQuickItem() override;

        TimeViewModel *timeViewModel() const;
        void setTimeViewModel(TimeViewModel *viewModel);
        TimeLayoutViewModel *timeLayoutViewModel() const;
        void setTimeLayoutViewModel(TimeLayoutViewModel *viewModel);
        QColor color() const;
        void setColor(const QColor &color);
        QFont font() const;
        void setFont(const QFont &font);

    Q_SIGNALS:
        void timeViewModelChanged();
        void timeLayoutViewModelChanged();
        void colorChanged();
        void fontChanged();

    protected:
        QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<AbsoluteTimelineScaleQuickItemPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_ABSOLUTETIMELINESCALEQUICKITEM_P_H
