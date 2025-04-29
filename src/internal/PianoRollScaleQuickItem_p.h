#ifndef SCOPIC_FLOW_PIANOROLLSCALEQUICKITEM_P_H
#define SCOPIC_FLOW_PIANOROLLSCALEQUICKITEM_P_H

#include <QQuickItem>

namespace sflow {

    class TimeViewModel;
    class TimeLayoutViewModel;

    class PianoRollScaleQuickItemPrivate;

    class PianoRollScaleQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(PianoRollScale)
        Q_DECLARE_PRIVATE(PianoRollScaleQuickItem)
        Q_PROPERTY(TimeViewModel *timeViewModel READ timeViewModel WRITE setTimeViewModel NOTIFY timeViewModelChanged)
        Q_PROPERTY(TimeLayoutViewModel *timeLayoutViewModel READ timeLayoutViewModel WRITE setTimeLayoutViewModel NOTIFY timeLayoutViewModelChanged)
        Q_PROPERTY(QColor beatScaleColor READ beatScaleColor WRITE setBeatScaleColor NOTIFY beatScaleColorChanged)
        Q_PROPERTY(QColor barScaleColor READ barScaleColor WRITE setBarScaleColor NOTIFY barScaleColorChanged)
        Q_PROPERTY(QColor segmentScaleColor READ segmentScaleColor WRITE setSegmentScaleColor NOTIFY segmentScaleColorChanged)
        Q_PRIVATE_SLOT(d_func(), void updateTimeline())
    public:
        explicit PianoRollScaleQuickItem(QQuickItem *parent = nullptr);
        ~PianoRollScaleQuickItem() override;

        TimeViewModel *timeViewModel() const;
        void setTimeViewModel(TimeViewModel *timeViewModel);

        TimeLayoutViewModel *timeLayoutViewModel() const;
        void setTimeLayoutViewModel(TimeLayoutViewModel *timeLayoutViewModel);

        QColor beatScaleColor() const;
        void setBeatScaleColor(const QColor &color);

        QColor barScaleColor() const;
        void setBarScaleColor(const QColor &color);

        QColor segmentScaleColor() const;
        void setSegmentScaleColor(const QColor &color);

    protected:
        QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

    signals:
        void timeViewModelChanged();
        void timeLayoutViewModelChanged();
        void beatScaleColorChanged();
        void barScaleColorChanged();
        void segmentScaleColorChanged();

    private:
        QScopedPointer<PianoRollScaleQuickItemPrivate> d_ptr;
    };

} // sflow

#endif //SCOPIC_FLOW_PIANOROLLSCALEQUICKITEM_P_H
