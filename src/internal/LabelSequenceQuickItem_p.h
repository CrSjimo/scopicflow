#ifndef SCOPIC_FLOW_LABELSEQUENCEQUICKITEM_P_H
#define SCOPIC_FLOW_LABELSEQUENCEQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/LabelSequenceViewModel.h>
#include <ScopicFlow/LabelViewModel.h>

namespace sflow {

    class LabelSequenceQuickItemPrivate;

    class LabelSequenceQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(LabelSequenceInternal)
        Q_DECLARE_PRIVATE(LabelSequenceQuickItem)
        Q_PROPERTY(LabelSequenceViewModel *labelSequenceViewModel READ labelSequenceViewModel WRITE setLabelSequenceViewModel NOTIFY labelSequenceViewModelChanged)
        Q_PROPERTY(QList<LabelViewModel *> model READ model NOTIFY modelChanged)
        Q_PROPERTY(QList<LabelViewModel *> selection READ selection NOTIFY selectionChanged)
        Q_PROPERTY(LabelViewModel *currentItem READ currentItem WRITE setCurrentItem NOTIFY currentItemChanged)
    public:
        explicit LabelSequenceQuickItem(QQuickItem *parent = nullptr);
        ~LabelSequenceQuickItem() override;

        LabelSequenceViewModel *labelSequenceViewModel() const;
        void setLabelSequenceViewModel(LabelSequenceViewModel *viewModel);

        QList<LabelViewModel *> model() const;
        QList<LabelViewModel *> selection() const;

        Q_INVOKABLE int deselectAll();
        Q_INVOKABLE void extendSelection(LabelViewModel *viewModel);
        Q_INVOKABLE LabelViewModel *nextItem(LabelViewModel *viewModel) const;
        Q_INVOKABLE LabelViewModel *previousItem(LabelViewModel *viewModel) const;
        Q_INVOKABLE LabelViewModel *insertLabelTo(int position, const QVariant &initialValue);
        Q_INVOKABLE void removeLabel(LabelViewModel *viewModel);
        Q_INVOKABLE void setSelectionIntermediate(bool intermediate);

        LabelViewModel *currentItem() const;
        void setCurrentItem(LabelViewModel *viewModel);

    signals:
        void labelSequenceViewModelChanged(LabelSequenceViewModel *viewModel);
        void modelChanged();
        void selectionChanged();
        void currentItemChanged(LabelViewModel *viewModel);


    private:
        QScopedPointer<LabelSequenceQuickItemPrivate> d_ptr;

    };

} // sflow

#endif //SCOPIC_FLOW_LABELSEQUENCEQUICKITEM_P_H
