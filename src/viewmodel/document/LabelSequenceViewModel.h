#ifndef SCOPIC_FLOW_LABELSEQUENCEVIEWMODEL_H
#define SCOPIC_FLOW_LABELSEQUENCEVIEWMODEL_H

#include <QObject>
#include <QMap>
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

    signals:
        void currentItemChanged(LabelViewModel *currentItem);
        void countChanged(int count);
        void selectionChanged();

    private:
        QMap<int, LabelViewModel *> m_labels;
        QSet<LabelViewModel *> m_selection;
        LabelViewModel *m_currentItem;

    };

}

#endif //SCOPIC_FLOW_LABELSEQUENCEVIEWMODEL_H
