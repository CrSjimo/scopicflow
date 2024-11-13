#ifndef SCOPIC_FLOW_PIANOROLLBACKGROUNDQUICKITEM_P_H
#define SCOPIC_FLOW_PIANOROLLBACKGROUNDQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <ScopicFlow/ClavierViewModel.h>

namespace sflow {

    class PianoRollBackgroundQuickItemPrivate;

    class PianoRollBackgroundQuickItem : public QQuickItem {
        Q_OBJECT
        Q_DECLARE_PRIVATE(PianoRollBackgroundQuickItem)
        Q_PROPERTY(TimeAlignmentViewModel *timeAlignmentViewModel READ timeAlignmentViewModel WRITE setTimeAlignmentViewModel NOTIFY timeAlignmentViewModelChanged)
        Q_PROPERTY(QColor scaleColor READ scaleColor WRITE setScaleColor NOTIFY scaleColorChanged)
        Q_PROPERTY(QColor barScaleColor READ barScaleColor WRITE setBarScaleColor NOTIFY barScaleColorChanged)
        Q_PROPERTY(QColor segmentScaleColor READ segmentScaleColor WRITE setSegmentScaleColor NOTIFY segmentScaleColorChanged)
    public:
        explicit PianoRollBackgroundQuickItem(QQuickItem *parent = nullptr);
        ~PianoRollBackgroundQuickItem() override;

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel);

        QColor scaleColor() const;
        void setScaleColor(const QColor &color);

        QColor barScaleColor() const;
        void setBarScaleColor(const QColor &color);

        QColor segmentScaleColor() const;
        void setSegmentScaleColor(const QColor &color);

    protected:
        QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

    signals:
        void timeAlignmentViewModelChanged(TimeAlignmentViewModel *viewModel);
        void scaleColorChanged(const QColor &color);
        void barScaleColorChanged(const QColor &color);
        void segmentScaleColorChanged(const QColor &color);

    private:
        QScopedPointer<PianoRollBackgroundQuickItemPrivate> d_ptr;
    };

} // sflow

#endif //SCOPIC_FLOW_PIANOROLLBACKGROUNDQUICKITEM_P_H
