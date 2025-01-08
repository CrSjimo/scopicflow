#include "NoteCollectionViewModel.h"
#include "NoteCollectionViewModel_p.h"

namespace sflow {
    NoteCollectionViewModel::NoteCollectionViewModel(QObject *parent) : QObject(parent), d_ptr(new NoteCollectionViewModelPrivate) {
    }
    NoteCollectionViewModel::~NoteCollectionViewModel() {
    }
    NoteViewModel *NoteCollectionViewModel::currentItem() const {
        Q_D(const NoteCollectionViewModel);
        return d->currentItem;
    }
    void NoteCollectionViewModel::setCurrentItem(NoteViewModel *item) {
        Q_D(NoteCollectionViewModel);
        if (d->currentItem == item)
            return;
        d->currentItem = item;
        emit currentItemChanged(item);
    }
}