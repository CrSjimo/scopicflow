#ifndef SCOPIC_FLOW_PIANOROLLBACKGROUNDQUICKITEM_P_H
#define SCOPIC_FLOW_PIANOROLLBACKGROUNDQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/TimeAlignmentViewModel.h>

namespace sflow {

    class PianoRollBackgroundQuickItemPrivate;

    class SCOPIC_FLOW_EXPORT PianoRollBackgroundQuickItem : public QQuickItem {
        Q_OBJECT
        Q_DECLARE_PRIVATE(PianoRollBackgroundQuickItem)
        Q_PROPERTY(TimeAlignmentViewModel *timeAlignmentViewModel READ timeAlignmentViewModel WRITE setTimeAlignmentViewModel NOTIFY timeAlignmentViewModelChanged)
        Q_PROPERTY(QColor beatScaleColor READ beatScaleColor WRITE setBeatScaleColor NOTIFY beatScaleColorChanged)
        Q_PROPERTY(QColor barScaleColor READ barScaleColor WRITE setBarScaleColor NOTIFY barScaleColorChanged)
        Q_PROPERTY(QColor segmentScaleColor READ segmentScaleColor WRITE setSegmentScaleColor NOTIFY segmentScaleColorChanged)
    public:
        explicit PianoRollBackgroundQuickItem(QQuickItem *parent = nullptr);
        ~PianoRollBackgroundQuickItem() override;

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel);

        QColor beatScaleColor() const;
        void setBeatScaleColor(const QColor &color);

        QColor barScaleColor() const;
        void setBarScaleColor(const QColor &color);

        QColor segmentScaleColor() const;
        void setSegmentScaleColor(const QColor &color);

    protected:
        QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

    signals:
        void timeAlignmentViewModelChanged(TimeAlignmentViewModel *viewModel);
        void beatScaleColorChanged(const QColor &color);
        void barScaleColorChanged(const QColor &color);
        void segmentScaleColorChanged(const QColor &color);

    private:
        QScopedPointer<PianoRollBackgroundQuickItemPrivate> d_ptr;
    };

} // sflow

#endif //SCOPIC_FLOW_PIANOROLLBACKGROUNDQUICKITEM_P_H
