#ifndef SCOPIC_FLOW_LABELSEQUENCEVIEWMODEL_H
#define SCOPIC_FLOW_LABELSEQUENCEVIEWMODEL_H

#include <map>

#include <QObject>
#include <QSet>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class LabelViewModel;

    class SCOPIC_FLOW_EXPORT LabelSequenceViewModel : public QObject {
        Q_OBJECT
    public:
        explicit LabelSequenceViewModel(QObject *parent = nullptr);
        ~LabelSequenceViewModel() override;

        LabelViewModel *currentItem() const;
        void setCurrentItem(LabelViewModel *item);

        int count() const;

        void insertLabels(const QList<LabelViewModel *> &labels);
        void removeLabels(const QList<LabelViewModel *> &labels);

        QList<LabelViewModel *> labels() const;

        QList<LabelViewModel *> selection() const;

        LabelViewModel *nextItem(LabelViewModel *item) const;
        LabelViewModel *previousItem(LabelViewModel *item) const;

    signals:
        void currentItemChanged(LabelViewModel *currentItem);
        void countChanged(int count);
        void selectionChanged();

    private:
        std::multimap<int, LabelViewModel *> m_labels;
        QHash<LabelViewModel *, int> m_positions;
        QSet<LabelViewModel *> m_selection;
        LabelViewModel *m_currentItem;

    };

}

#endif //SCOPIC_FLOW_LABELSEQUENCEVIEWMODEL_H
