#ifndef SCOPIC_FLOW_NOTETHUMBNAILQUICKITEM_P_H
#define SCOPIC_FLOW_NOTETHUMBNAILQUICKITEM_P_H

#include <QColor>
#include <QQuickItem>

class QSGNode;

namespace sflow {

    class NoteThumbnailQuickItemPrivate;
    class RangeSequenceViewModel;

    class NoteThumbnailQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(NoteThumbnail)
        Q_DECLARE_PRIVATE(NoteThumbnailQuickItem)
        Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
        Q_PROPERTY(sflow::RangeSequenceViewModel *noteSequenceViewModel READ noteSequenceViewModel WRITE setNoteSequenceViewModel NOTIFY noteSequenceViewModelChanged)
        Q_PROPERTY(double horizontalFactor READ horizontalFactor CONSTANT)
        Q_PROPERTY(int maxKey READ maxKey NOTIFY maxKeyChanged)
        Q_PROPERTY(int minKey READ minKey NOTIFY minKeyChanged)

    public:
        explicit NoteThumbnailQuickItem(QQuickItem *parent = nullptr);
        ~NoteThumbnailQuickItem() override;

        QColor color() const;
        void setColor(const QColor &color);

        RangeSequenceViewModel *noteSequenceViewModel() const;
        void setNoteSequenceViewModel(RangeSequenceViewModel *noteSequenceViewModel);

        double horizontalFactor() const;

        int maxKey() const;
        int minKey() const;

    protected:
        QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

    Q_SIGNALS:
        void colorChanged();
        void noteSequenceViewModelChanged();
        void maxKeyChanged();
        void minKeyChanged();

    private:
        QScopedPointer<NoteThumbnailQuickItemPrivate> d_ptr;
    };

}

#endif //SCOPIC_FLOW_NOTETHUMBNAILQUICKITEM_P_H
