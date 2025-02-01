#include "PointSequenceViewModel.h"
#include "PointSequenceViewModel_p.h"

#include <ScopicFlow/private/ViewModelHelper_p.h>
#include <ScopicFlow/private/SelectableViewModelManipulatorInterface_p.h>

namespace sflow {

    class PointSequenceViewModelManipulatorInterface : public SelectableViewModelManipulatorInterface {
        Q_OBJECT
    public:
        Q_INVOKABLE explicit PointSequenceViewModelManipulatorInterface(QObject *viewModel, QObject *parent) : SelectableViewModelManipulatorInterface(parent) {
            handle = viewModel->property("handle").value<PointSequenceViewModelQmlHandle *>();
        }
        void setSelected(const QVariant &item, bool selected) override {
            item.value<QObject *>()->setProperty(handle->d_func()->selectedProperty, selected);
        }
        bool isSelected(const QVariant &item) const override {
            return item.value<QObject *>()->property(handle->d_func()->selectedProperty).toBool();
        }
        QVariant nextItem(const QVariant &item) const override {
            return QVariant::fromValue(handle->nextItem(item.value<QObject *>()));
        }
        QVariant previousItem(const QVariant &item) const override {
            return QVariant::fromValue(handle->previousItem(item.value<QObject *>()));
        }
        QVariant firstItem() const override {
            return QVariant::fromValue(handle->firstItem());
        }
        QVariant lastItem() const override {
            return QVariant::fromValue(handle->lastItem());
        }
        QVariant currentItem() const override {
            return QVariant::fromValue(handle->currentItem());
        }
        void setCurrentItem(const QVariant &item) override {
            handle->setCurrentItem(item.value<QObject *>());
        }
        QVariantList selection() const override {
            QVariantList ret;
            ret.reserve(handle->d_func()->selection.size());
            std::transform(handle->d_func()->selection.cbegin(), handle->d_func()->selection.cend(), std::back_inserter(ret), [](auto item) {
                return QVariant::fromValue(item);
            });
            return ret;
        }
        int compareOrder(const QVariant &item1, const QVariant &item2) const override {
            return item1.value<QObject *>()->property(handle->d_func()->positionProperty).toInt() - item2.value<QObject *>()->property(handle->d_func()->positionProperty).toInt();
        }
        bool isValidItem(const QVariant &item) const override {
            return static_cast<bool>(item.value<QObject *>());
        }
        qsizetype getId(const QVariant &item) const override {
            return reinterpret_cast<qsizetype>(item.value<QObject *>());
        }
        QVariant fromId(qsizetype id) const override {
            return QVariant::fromValue(reinterpret_cast<QObject *>(id));
        }
        QObject *viewModel() const override {
            return handle->d_func()->q_ptr;
        }

    private:
        PointSequenceViewModelQmlHandle *handle;
    };

    namespace {
        SelectableViewModelManipulatorInterfaceRegistry<PointSequenceViewModel, PointSequenceViewModelManipulatorInterface> _;
    }

    PointSequenceViewModelQmlHandle::PointSequenceViewModelQmlHandle(PointSequenceViewModelPrivate *d) : SliceableViewModelQmlHandle(d->q_ptr), d_ptr(d) {
    }
    PointSequenceViewModelQmlHandle::~PointSequenceViewModelQmlHandle() = default;
    QObject *PointSequenceViewModelQmlHandle::currentItem() const {
        Q_D(const PointSequenceViewModel);
        return d->currentItem;
    }
    void PointSequenceViewModelQmlHandle::setCurrentItem(QObject *item) {
        Q_D(PointSequenceViewModel);
        if (d->currentItem != item) {
            d->currentItem = item;
            emit currentItemChanged();
            emit d->q_ptr->currentItemChanged(item);
        }
    }
    bool PointSequenceViewModelQmlHandle::intermediate() const {
        Q_D(const PointSequenceViewModel);
        return d->intermediate;
    }
    void PointSequenceViewModelQmlHandle::setIntermediate(bool intermediate) {
        Q_D(PointSequenceViewModel);
        if (d->intermediate != intermediate) {
            d->intermediate = intermediate;
            emit intermediateChanged();
            emit d->q_ptr->intermediateChanged(intermediate);
        }
    }
    QObject *PointSequenceViewModelQmlHandle::previousItem(QObject *item) const {
        Q_D(const PointSequenceViewModel);
        return d->container.previousItem(item);
    }
    QObject *PointSequenceViewModelQmlHandle::nextItem(QObject *item) const {
        Q_D(const PointSequenceViewModel);
        return d->container.nextItem(item);
    }
    inline QObject *PointSequenceViewModelQmlHandle::firstItem() const {
        Q_D(const PointSequenceViewModel);
        return d->container.firstItem();
    }
    inline QObject *PointSequenceViewModelQmlHandle::lastItem() const {
        Q_D(const PointSequenceViewModel);
        return d->container.lastItem();
    }
    QObjectList PointSequenceViewModelQmlHandle::selection() const {
        Q_D(const PointSequenceViewModel);
        return QObjectList(d->selection.cbegin(), d->selection.cend());
    }
    void PointSequenceViewModelQmlHandle::insertItem(QObject *item) {
        Q_D(PointSequenceViewModel);
        d->insertItem(item);
        emit itemInserted(item);
        emit d->q_ptr->itemInserted(item);
    }
    void PointSequenceViewModelQmlHandle::removeItem(QObject *item) {
        Q_D(PointSequenceViewModel);
        d->removeItem(item);
        emit itemRemoved(item);
        emit d->q_ptr->itemRemoved(item);
    }
    QObjectList PointSequenceViewModelQmlHandle::slice(int position, int length) const {
        Q_D(const PointSequenceViewModel);
        return d->container.slice(position, length);
    }
    int PointSequenceViewModelQmlHandle::itemPosition(QObject *item) const {
        Q_D(const PointSequenceViewModel);
        return item->property(d->positionProperty).toInt();
    }


    static QMetaMethod handleItemSelectedChangedMetaMethod;
    static QMetaMethod handleItemPositionChangedMetaMethod;


    void PointSequenceViewModelPrivate::insertItem(QObject *item) {
        Q_Q(PointSequenceViewModel);
        container.insertItem(item, item->property(positionProperty).toInt());
        if (item->property(selectedProperty).toBool())
            selection.insert(item);
        ViewModelHelper::connectPropertyNotify(item, selectedProperty, q, handleItemSelectedChangedMetaMethod);
        ViewModelHelper::connectPropertyNotify(item, positionProperty, q, handleItemPositionChangedMetaMethod);
    }
    void PointSequenceViewModelPrivate::removeItem(QObject *item) {
        Q_Q(PointSequenceViewModel);
        container.removeItem(item);
        selection.remove(item);
        QObject::disconnect(item, nullptr, q, nullptr);
        if (item == currentItem)
            currentItem = nullptr;
    }
    void PointSequenceViewModelPrivate::handleItemSelectedChanged() {
        Q_Q(PointSequenceViewModel);
        auto item = q->sender();
        if (item->property(selectedProperty).toBool()) {
            selection.insert(item);
            emit q->itemSelected(item);
        } else {
            selection.remove(item);
            emit q->itemDeselected(item);
        }
        emit handle->selectionChanged();
    }
    void PointSequenceViewModelPrivate::handleItemPositionChanged() {
        Q_Q(PointSequenceViewModel);
        auto item = q->sender();
        container.insertItem(item, item->property(positionProperty).toInt());
        emit handle->itemUpdated(item);
    }


    PointSequenceViewModel::PointSequenceViewModel(QObject *parent, const QByteArray &positionProperty, const QByteArray &selectedProperty) : QObject(parent), d_ptr(new PointSequenceViewModelPrivate) {
        Q_D(PointSequenceViewModel);
        d->q_ptr = this;
        d->handle = new PointSequenceViewModelQmlHandle(d);
        d->positionProperty = positionProperty;
        d->selectedProperty = selectedProperty;
        if (!handleItemSelectedChangedMetaMethod.isValid() || !handleItemPositionChangedMetaMethod.isValid()) {
            for (int i = staticMetaObject.methodOffset(); i < staticMetaObject.methodCount(); i++) {
                auto method = staticMetaObject.method(i);
                if (method.name() == "handleItemSelectedChanged") {
                    handleItemSelectedChangedMetaMethod = method;
                }
                if (method.name() == "handleItemPositionChanged") {
                    handleItemPositionChangedMetaMethod = method;
                }
                if (handleItemSelectedChangedMetaMethod.isValid() && handleItemPositionChangedMetaMethod.isValid())
                    break;
            }
        }
        Q_ASSERT(handleItemSelectedChangedMetaMethod.isValid() && handleItemPositionChangedMetaMethod.isValid());
    }
    PointSequenceViewModel::~PointSequenceViewModel() = default;
    QObject *PointSequenceViewModel::currentItem() const {
        Q_D(const PointSequenceViewModel);
        return d->currentItem;
    }
    void PointSequenceViewModel::setCurrentItem(QObject *item) {
        Q_D(PointSequenceViewModel);
        if (d->currentItem != item) {
            d->currentItem = item;
            emit d->handle->currentItemChanged();
        }
    }
    bool PointSequenceViewModel::intermediate() const {
        Q_D(const PointSequenceViewModel);
        return d->intermediate;
    }
    void PointSequenceViewModel::setIntermediate(bool intermediate) {
        Q_D(PointSequenceViewModel);
        if (d->intermediate != intermediate) {
            d->intermediate = intermediate;
            emit d->handle->intermediateChanged();
        }
    }
    int PointSequenceViewModel::count() const {
        Q_D(const PointSequenceViewModel);
        return d->container.m_items.size();
    }
    QObjectList PointSequenceViewModel::items() const {
        Q_D(const PointSequenceViewModel);
        return d->container.m_positions.keys();
    }
    void PointSequenceViewModel::insertItem(QObject *item) {
        Q_D(PointSequenceViewModel);
        d->insertItem(item);
        emit d->handle->itemInserted(item);

    }
    void PointSequenceViewModel::removeItem(QObject *item) {
        Q_D(PointSequenceViewModel);
        d->removeItem(item);
        emit d->handle->itemRemoved(item);
    }
    QObjectList PointSequenceViewModel::selection() const {
        Q_D(const PointSequenceViewModel);
        return d->selection.values();
    }
}

#include "PointSequenceViewModel.moc"
#include "moc_PointSequenceViewModel.cpp"
#include "moc_PointSequenceViewModel_p.cpp"