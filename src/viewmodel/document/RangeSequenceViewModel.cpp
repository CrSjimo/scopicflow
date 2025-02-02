#include "RangeSequenceViewModel.h"
#include "RangeSequenceViewModel_p.h"

#include <ScopicFlow/private/ViewModelHelper_p.h>
#include <ScopicFlow/private/SelectableViewModelManipulatorInterface_p.h>

namespace sflow {

    class RangeSequenceViewModelManipulatorInterface : public SelectableViewModelManipulatorInterface {
        Q_OBJECT
    public:
        Q_INVOKABLE explicit RangeSequenceViewModelManipulatorInterface(QObject *viewModel, QObject *parent = nullptr) : SelectableViewModelManipulatorInterface(parent) {
            handle = viewModel->property("handle").value<RangeSequenceViewModelQmlHandle *>();
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
        RangeSequenceViewModelQmlHandle *handle;
    };

    namespace {
        SelectableViewModelManipulatorInterfaceRegistry<RangeSequenceViewModel, RangeSequenceViewModelManipulatorInterface> _;
    }

    RangeSequenceViewModelQmlHandle::RangeSequenceViewModelQmlHandle(RangeSequenceViewModelPrivate *d) : SliceableViewModelQmlHandle(d->q_ptr), d_ptr(d) {
    }
    RangeSequenceViewModelQmlHandle::~RangeSequenceViewModelQmlHandle() = default;
    QObject *RangeSequenceViewModelQmlHandle::currentItem() const {
        Q_D(const RangeSequenceViewModel);
        return d->currentItem;
    }
    void RangeSequenceViewModelQmlHandle::setCurrentItem(QObject *item) {
        Q_D(RangeSequenceViewModel);
        if (d->currentItem != item) {
            d->currentItem = item;
            emit currentItemChanged();
            emit d->q_ptr->currentItemChanged(item);
        }
    }
    bool RangeSequenceViewModelQmlHandle::intermediate() const {
        Q_D(const RangeSequenceViewModel);
        return d->intermediate;
    }
    void RangeSequenceViewModelQmlHandle::setIntermediate(bool intermediate) {
        Q_D(RangeSequenceViewModel);
        if (d->intermediate != intermediate) {
            d->intermediate = intermediate;
            emit intermediateChanged();
            emit d->q_ptr->intermediateChanged(intermediate);
        }
    }
    QObject *RangeSequenceViewModelQmlHandle::previousItem(QObject *item) const {
        Q_D(const RangeSequenceViewModel);
        return d->pointSequenceContainer.previousItem(item);
    }
    QObject *RangeSequenceViewModelQmlHandle::nextItem(QObject *item) const {
        Q_D(const RangeSequenceViewModel);
        return d->pointSequenceContainer.nextItem(item);
    }
    QObject *RangeSequenceViewModelQmlHandle::firstItem() const {
        Q_D(const RangeSequenceViewModel);
        return d->pointSequenceContainer.firstItem();
    }
    QObject *RangeSequenceViewModelQmlHandle::lastItem() const {
        Q_D(const RangeSequenceViewModel);
        return d->pointSequenceContainer.lastItem();
    }
    QObjectList RangeSequenceViewModelQmlHandle::items() const {
        Q_D(const RangeSequenceViewModel);
        return d->pointSequenceContainer.m_positions.keys();
    }
    QObjectList RangeSequenceViewModelQmlHandle::selection() const {
        Q_D(const RangeSequenceViewModel);
        return QObjectList(d->selection.cbegin(), d->selection.cend());
    }
    void RangeSequenceViewModelQmlHandle::insertItem(QObject *item) {
        Q_D(RangeSequenceViewModel);
        d->insertItem(item);
        emit itemInserted(item);
        emit d->q_ptr->itemInserted(item);
    }
    void RangeSequenceViewModelQmlHandle::removeItem(QObject *item) {
        Q_D(RangeSequenceViewModel);
        d->removeItem(item);
        emit itemRemoved(item);
        emit d->q_ptr->itemRemoved(item);
    }
    QObjectList RangeSequenceViewModelQmlHandle::slice(int position, int length) const {
        Q_D(const RangeSequenceViewModel);
        return d->rangeSequenceContainer.slice(position, length);
    }
    int RangeSequenceViewModelQmlHandle::itemPosition(QObject *item) const {
        Q_D(const RangeSequenceViewModel);
        return item->property(d->positionProperty).toInt();
    }
    int RangeSequenceViewModelQmlHandle::itemLength(QObject *item) const {
        Q_D(const RangeSequenceViewModel);
        return item->property(d->lengthProperty).toInt();
    }


    static QMetaMethod handleItemSelectedChangedMetaMethod;
    static QMetaMethod handleItemPositionChangedMetaMethod;
    static QMetaMethod handleItemLengthChangedMetaMethod;


    void RangeSequenceViewModelPrivate::insertItem(QObject *item) {
        Q_Q(RangeSequenceViewModel);
        pointSequenceContainer.insertItem(item, item->property(positionProperty).toInt());
        auto affectedItems = rangeSequenceContainer.insertItem(item, item->property(positionProperty).toInt(), item->property(lengthProperty).toInt());
        updateOverlapped(affectedItems);
        if (item->property(selectedProperty).toBool())
            selection.insert(item);
        ViewModelHelper::connectPropertyNotify(item, selectedProperty, q, handleItemSelectedChangedMetaMethod);
        ViewModelHelper::connectPropertyNotify(item, positionProperty, q, handleItemPositionChangedMetaMethod);
        ViewModelHelper::connectPropertyNotify(item, lengthProperty, q, handleItemLengthChangedMetaMethod);
    }
    void RangeSequenceViewModelPrivate::removeItem(QObject *item) {
        Q_Q(RangeSequenceViewModel);
        pointSequenceContainer.removeItem(item);
        auto affectedItems = rangeSequenceContainer.removeItem(item);
        updateOverlapped(affectedItems);
        selection.remove(item);
        QObject::disconnect(item, nullptr, q, nullptr);
        if (item == currentItem)
            currentItem = nullptr;
    }
    void RangeSequenceViewModelPrivate::updateOverlapped(const QObjectList &affectedItems) const {
        for (auto item : affectedItems) {
            item->setProperty(overlappedProperty, rangeSequenceContainer.isOverlapped(item));
        }
    }
    void RangeSequenceViewModelPrivate::handleItemSelectedChanged() {
        Q_Q(RangeSequenceViewModel);
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
    void RangeSequenceViewModelPrivate::handleItemPositionChanged() {
        Q_Q(RangeSequenceViewModel);
        auto item = q->sender();
        pointSequenceContainer.insertItem(item, item->property(positionProperty).toInt());
        auto affectedItems = rangeSequenceContainer.insertItem(item, item->property(positionProperty).toInt(), item->property(lengthProperty).toInt());
        updateOverlapped(affectedItems);
        emit handle->itemUpdated(item);
    }
    void RangeSequenceViewModelPrivate::handleItemLengthChanged() {
        Q_Q(RangeSequenceViewModel);
        auto item = q->sender();
        auto affectedItems = rangeSequenceContainer.insertItem(item, item->property(positionProperty).toInt(), item->property(lengthProperty).toInt());
        updateOverlapped(affectedItems);
        emit handle->itemUpdated(item);
    }


    RangeSequenceViewModel::RangeSequenceViewModel(QObject *parent, const QByteArray &positionProperty, const QByteArray &lengthProperty, const QByteArray &selectedProperty, const QByteArray &overlappedProperty) : QObject(parent), d_ptr(new RangeSequenceViewModelPrivate) {
        Q_D(RangeSequenceViewModel);
        d->q_ptr = this;
        d->handle = new RangeSequenceViewModelQmlHandle(d);
        d->positionProperty = positionProperty;
        d->lengthProperty = lengthProperty;
        d->selectedProperty = selectedProperty;
        d->overlappedProperty = overlappedProperty;
        if (!handleItemSelectedChangedMetaMethod.isValid() || !handleItemPositionChangedMetaMethod.isValid() || !handleItemLengthChangedMetaMethod.isValid()) {
            for (int i = staticMetaObject.methodOffset(); i < staticMetaObject.methodCount(); i++) {
                auto method = staticMetaObject.method(i);
                if (method.name() == "handleItemSelectedChanged") {
                    handleItemSelectedChangedMetaMethod = method;
                }
                if (method.name() == "handleItemPositionChanged") {
                    handleItemPositionChangedMetaMethod = method;
                }
                if (method.name() == "handleItemLengthChanged") {
                    handleItemLengthChangedMetaMethod = method;
                }
                if (handleItemSelectedChangedMetaMethod.isValid() && handleItemPositionChangedMetaMethod.isValid() && handleItemLengthChangedMetaMethod.isValid())
                    break;
            }
        }
        Q_ASSERT(handleItemSelectedChangedMetaMethod.isValid() && handleItemPositionChangedMetaMethod.isValid() && handleItemLengthChangedMetaMethod.isValid());
    }
    RangeSequenceViewModel::~RangeSequenceViewModel() = default;
    QObject *RangeSequenceViewModel::currentItem() const {
        Q_D(const RangeSequenceViewModel);
        return d->currentItem;
    }
    void RangeSequenceViewModel::setCurrentItem(QObject *item) {
        Q_D(RangeSequenceViewModel);
        if (d->currentItem != item) {
            d->currentItem = item;
            emit d->handle->currentItemChanged();
        }
    }
    bool RangeSequenceViewModel::intermediate() const {
        Q_D(const RangeSequenceViewModel);
        return d->intermediate;
    }
    void RangeSequenceViewModel::setIntermediate(bool intermediate) {
        Q_D(RangeSequenceViewModel);
        if (d->intermediate != intermediate) {
            d->intermediate = intermediate;
            emit d->handle->intermediateChanged();
        }
    }
    int RangeSequenceViewModel::count() const {
        Q_D(const RangeSequenceViewModel);
        return d->pointSequenceContainer.m_positions.size();
    }
    QObjectList RangeSequenceViewModel::items() const {
        Q_D(const RangeSequenceViewModel);
        return d->pointSequenceContainer.m_positions.keys();
    }
    void RangeSequenceViewModel::insertItem(QObject *item) {
        Q_D(RangeSequenceViewModel);
        d->insertItem(item);
        emit d->handle->itemInserted(item);
    }
    void RangeSequenceViewModel::removeItem(QObject *item) {
        Q_D(RangeSequenceViewModel);
        d->removeItem(item);
        emit d->handle->itemRemoved(item);
    }
    QObjectList RangeSequenceViewModel::selection() const {
        Q_D(const RangeSequenceViewModel);
        return d->selection.values();
    }
}

#include "RangeSequenceViewModel.moc"
#include "moc_RangeSequenceViewModel.cpp"
#include "moc_RangeSequenceViewModel_p.cpp"