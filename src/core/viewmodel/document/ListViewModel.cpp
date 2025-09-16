#include "ListViewModel.h"
#include "ListViewModel_p.h"

#include <algorithm>

#include <ScopicFlowCore/private/SelectableViewModelManipulatorInterface_p.h>
#include <ScopicFlowCore/private/ViewModelHelper_p.h>

namespace sflow {

    class ListViewModelManipulatorInterface : public SelectableViewModelManipulatorInterface {
        Q_OBJECT
    public:
        Q_INVOKABLE explicit ListViewModelManipulatorInterface(QObject *viewModel, QObject *parent = nullptr) : SelectableViewModelManipulatorInterface(parent) {
            handle = static_cast<ListViewModel *>(viewModel)->d_func()->handle;
        }
        void setSelected(const QVariant &item, bool selected) override {
            if (auto obj = handle->itemAt(item.toInt())) {
                obj->setProperty(handle->d_func()->selectedProperty, selected);
            }
        }
        bool isSelected(const QVariant &item) const override {
            if (auto obj = handle->itemAt(item.toInt())) {
                return obj->property(handle->d_func()->selectedProperty).toBool();
            }
            return false;
        }
        QVariant nextItem(const QVariant &item) const override {
            return item.toInt() + 1;
        }
        QVariant previousItem(const QVariant &item) const override {
            return item.toInt() - 1;
        }
        QVariant firstItem() const override {
            return 0;
        }
        QVariant lastItem() const override {
            return handle->count() - 1;
        }
        QVariant currentItem() const override {
            return handle->currentIndex();
        }
        void setCurrentItem(const QVariant &item) override {
            handle->setCurrentIndex(item.toInt());
        }
        QVariantList selection() const override {
            QVariantList list;
            for (int i = 0; i < handle->count(); i++) {
                auto obj = handle->itemAt(i);
                if (obj->property(handle->d_func()->selectedProperty).toBool()) {
                    list.append(i);
                }
            }
            return list;
        }
        int compareOrder(const QVariant &item1, const QVariant &item2) const override {
            auto index1 = item1.toInt();
            auto index2 = item2.toInt();
            return index1 - index2;
        }
        bool isValidItem(const QVariant &item) const override {
            if (!item.isValid() || item.isNull())
                return false;
            auto index = item.toInt();
            return index >= 0 && index < handle->count();
        }
        qsizetype getId(const QVariant &item) const override {
            return item.toInt();
        }
        QVariant fromId(qsizetype id) const override {
            return static_cast<int>(id);
        }
        QObject *viewModel() const override {
            return handle->d_func()->q_ptr;
        }

    private:
        ListViewModelQmlHandle *handle;
    };

    namespace {
        SelectableViewModelManipulatorInterfaceRegistry<ListViewModel, ListViewModelManipulatorInterface> _;
    }

    ListViewModelQmlHandle::ListViewModelQmlHandle(ListViewModelPrivate *d) : QObject(d->q_ptr), d_ptr(d) {
    }
    ListViewModelQmlHandle::~ListViewModelQmlHandle() = default;
    int ListViewModelQmlHandle::count() const {
        Q_D(const ListViewModel);
        return d->items.size();
    }
    int ListViewModelQmlHandle::currentIndex() const {
        Q_D(const ListViewModel);
        return d->currentIndex;
    }
    void ListViewModelQmlHandle::setCurrentIndex(int index) {
        Q_D(ListViewModel);
        if (d->currentIndex != index) {
            d->currentIndex = index;
            emit currentIndexChanged();
            emit d->q_ptr->currentIndexChanged(index);
        }
    }
    bool ListViewModelQmlHandle::isIntermediate() const {
        Q_D(const ListViewModel);
        return d->intermediate;
    }
    void ListViewModelQmlHandle::setIntermediate(bool intermediate) {
        Q_D(ListViewModel);
        if (d->intermediate != intermediate) {
            d->intermediate = intermediate;
            emit intermediateChanged();
            emit d->q_ptr->intermediateChanged(intermediate);
        }
    }
    QObjectList ListViewModelQmlHandle::items() const {
        Q_D(const ListViewModel);
        return d->items;
    }
    void ListViewModelQmlHandle::rotateItems(int index, int count, int middleIndex) {
        Q_D(ListViewModel);
        std::rotate(std::next(d->items.begin(), index), std::next(d->items.begin(), middleIndex), std::next(d->items.begin(), index + count));
        emit itemsChanged();
        emit d->q_ptr->itemsRotated(index, count, middleIndex);
        if (d->currentIndex >= index && d->currentIndex < middleIndex) {
            setCurrentIndex(index + count - middleIndex + d->currentIndex);
        } else if (d->currentIndex >= middleIndex && d->currentIndex < index + count) {
            setCurrentIndex(d->currentIndex - index);
        }
    }
    QObject *ListViewModelQmlHandle::itemAt(int index) const {
        Q_D(const ListViewModel);
        if (index < 0 || index >= d->items.size()) {
            return nullptr;
        }
        return d->items.at(index);
    }
    void ListViewModelPrivate::handleItemSelectedChanged() {
        Q_Q(ListViewModel);
        auto item = q->sender();
        if (item->property(selectedProperty).toBool()) {
            emit q->itemSelected(item);
        } else {
            emit q->itemDeselected(item);
        }
    }

    static QMetaMethod handleItemSelectedChangedMetaMethod;

    ListViewModel::ListViewModel(QObject *parent, const QByteArray &selectedProperty) : QObject(parent), d_ptr(new ListViewModelPrivate) {
        Q_D(ListViewModel);
        d->q_ptr = this;
        d->handle = new ListViewModelQmlHandle(d);
        d->selectedProperty = selectedProperty;
        if (!handleItemSelectedChangedMetaMethod.isValid()) {
            for (int i = staticMetaObject.methodOffset(); i < staticMetaObject.methodCount(); i++) {
                auto method = staticMetaObject.method(i);
                if (method.name() == "handleItemSelectedChanged") {
                    handleItemSelectedChangedMetaMethod = method;
                }
                if (handleItemSelectedChangedMetaMethod.isValid())
                    break;
            }
        }
        Q_ASSERT(handleItemSelectedChangedMetaMethod.isValid());
    }

    ListViewModel::~ListViewModel() = default;

    int ListViewModel::currentIndex() const {
        Q_D(const ListViewModel);
        return d->currentIndex;
    }
    void ListViewModel::setCurrentIndex(int index) {
        Q_D(ListViewModel);
        if (d->currentIndex != index) {
            d->currentIndex = index;
            emit d->handle->currentIndexChanged();
        }
    }
    bool ListViewModel::isIntermediate() const {
        Q_D(const ListViewModel);
        return d->intermediate;
    }
    void ListViewModel::setIntermediate(bool intermediate) {
        Q_D(ListViewModel);
        if (d->intermediate != intermediate) {
            d->intermediate = intermediate;
            emit d->handle->intermediateChanged();
        }
    }
    QObjectList ListViewModel::items() const {
        Q_D(const ListViewModel);
        return d->items;
    }
    void ListViewModel::setItems(const QObjectList &items) {
        Q_D(ListViewModel);
        auto oldSize = d->items.size();
        for (auto item : d->items) {
            disconnect(item, nullptr, this, nullptr);
        }
        d->items = items;
        for (auto item : d->items) {
            ViewModelHelper::connectPropertyNotify(item, d->selectedProperty, this, handleItemSelectedChangedMetaMethod);
        }
        if (oldSize != items.size())
            emit d->handle->countChanged();
        emit d->handle->itemsChanged();
    }
    QObjectList ListViewModel::selection() const {
        Q_D(const ListViewModel);
        QObjectList ret;
        std::copy_if(d->items.cbegin(), d->items.cend(), std::back_inserter(ret), [d](QObject *item) {
            return item->property(d->selectedProperty).toBool();
        });
        return ret;
    }
}

#include "ListViewModel.moc"
#include "moc_ListViewModel.cpp"
#include "moc_ListViewModel_p.cpp"
