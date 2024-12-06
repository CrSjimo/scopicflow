#ifndef SCOPIC_FLOW_NOTECOLLECTIONVIEWMODEL_H
#define SCOPIC_FLOW_NOTECOLLECTIONVIEWMODEL_H

#include <QObject>

namespace sflow {

    class NoteViewModel;

    class NoteCollectionViewModelPrivate;

    class NoteCollectionViewModel : public QObject {
        Q_OBJECT
        Q_DECLARE_PRIVATE(NoteCollectionViewModel)
    public:
        explicit NoteCollectionViewModel(QObject *parent = nullptr);
        ~NoteCollectionViewModel() override;

        NoteViewModel *currentItem() const;
        void setCurrentItem(NoteViewModel *item);

        int count() const;

        void insertNotes(const QList<NoteViewModel *> &notes);
        void removeNotes(const QList<NoteViewModel *> &notes);

        QList<NoteViewModel *> notes() const;
        QList<NoteViewModel *> selection() const;

        NoteViewModel *nextItem(NoteViewModel *item) const;
        NoteViewModel *previousItem(NoteViewModel *item) const;
        QList<NoteViewModel *> findItems(int pos, int length);

    signals:
        void noteInserted(const QList<NoteViewModel *> &notes);
        void noteRemoved(const QList<NoteViewModel *> &notes);
        void countChanged(int count);
        void currentItemChanged(NoteViewModel *item);
        void selectionChanged();

    private:
        QScopedPointer<NoteCollectionViewModelPrivate> d_ptr;

    };

} // sflow

#endif //SCOPIC_FLOW_NOTECOLLECTIONVIEWMODEL_H
