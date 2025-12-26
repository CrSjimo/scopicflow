#include "ListViewModel.h"
#include "ListViewModel_p.h"

#include <algorithm>

namespace sflow {


	ListViewModel::ListViewModel(QObject *parent) : QObject(parent), d_ptr(new ListViewModelPrivate) {
		Q_D(ListViewModel);
		d->q_ptr = this;
	}

	ListViewModel::~ListViewModel() = default;

	int ListViewModel::count() const {
		Q_D(const ListViewModel);
		return d->items.size();
	}

	QObjectList ListViewModel::items() const {
		Q_D(const ListViewModel);
		return d->items;
	}

	void ListViewModel::insertItem(int index, QObject *item) {
		Q_D(ListViewModel);

		Q_ASSERT(item);
		Q_ASSERT(index >= 0 && index <= d->items.size());

		emit itemAboutToInsert(index, item);

		d->items.insert(index, item);

		emit itemInserted(index, item);
		emit countChanged(d->items.size());
		emit itemsChanged();
	}

	void ListViewModel::removeItem(int index) {
		Q_D(ListViewModel);

		Q_ASSERT(index >= 0 && index < d->items.size());

		auto item = d->items.at(index);

		emit itemAboutToRemove(index, item);

		d->items.removeAt(index);

		emit itemRemoved(index, item);
		emit countChanged(d->items.size());
		emit itemsChanged();
	}

	void ListViewModel::rotate(int leftIndex, int middleIndex, int rightIndex) {
		Q_D(ListViewModel);

		int size = d->items.size();
		Q_ASSERT(leftIndex >= 0);
		Q_ASSERT(middleIndex >= leftIndex);
		Q_ASSERT(rightIndex >= middleIndex);
		Q_ASSERT(rightIndex <= size);

		if (leftIndex == middleIndex || middleIndex == rightIndex) {
			return;
		}

		auto begin = d->items.begin() + leftIndex;
		auto middle = d->items.begin() + middleIndex;
		auto end = d->items.begin() + rightIndex;

		std::rotate(begin, middle, end);

		emit rotated(leftIndex, middleIndex, rightIndex);
		emit itemsChanged();
	}


}

#include "moc_ListViewModel.cpp"
