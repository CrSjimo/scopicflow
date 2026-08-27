#ifndef SCOPIC_FLOW_RUBBERBANDSELECTOR_P_H
#define SCOPIC_FLOW_RUBBERBANDSELECTOR_P_H

#include <QObject>
#include <QRectF>
#include <QScopedPointer>
#include <qqmlintegration.h>

namespace sflow {

    class RubberBandSelectorPrivate;
    class SelectionController;

    class RubberBandSelector : public QObject {
        Q_OBJECT
        QML_NAMED_ELEMENT(RubberBandSelector)
        Q_DECLARE_PRIVATE(RubberBandSelector)
        Q_PROPERTY(SelectionController *selectionController READ selectionController WRITE setSelectionController NOTIFY selectionControllerChanged)
        Q_PROPERTY(ItemHint itemHint READ itemHint WRITE setItemHint NOTIFY itemHintChanged)

    public:
        enum ItemHint {
            None,
            IgnoreDimensionX,
            IgnoreDimensionY,
            SameWidth,
            SameHeight,
        };
        Q_ENUM(ItemHint)

        explicit RubberBandSelector(QObject *parent = nullptr);
        ~RubberBandSelector() override;

        SelectionController *selectionController() const;
        void setSelectionController(SelectionController *selectionController);

        ItemHint itemHint() const;
        void setItemHint(ItemHint itemHint);

        Q_INVOKABLE void insertItem(QObject *item, const QRectF &rect);
        Q_INVOKABLE void removeItem(QObject *item);
        Q_INVOKABLE void select(const QRectF &rect);

    Q_SIGNALS:
        void selectionControllerChanged();
        void itemHintChanged();
        void selectionAboutToEnd(const QRectF &rect);

    private:
        QScopedPointer<RubberBandSelectorPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_RUBBERBANDSELECTOR_P_H
