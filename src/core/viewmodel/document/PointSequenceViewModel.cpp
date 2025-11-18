#include "PointSequenceViewModel.h"
#include "PointSequenceViewModel_p.h"

#include <ScopicFlowCore/private/ViewModelHelper_p.h>

namespace sflow {

    PointSequenceViewModelSelectableManipulatorInterface::PointSequenceViewModelSelectableManipulatorInterface(PointSequenceViewModelPrivate *d) 
        : SelectableViewModelManipulatorInterface(d->q_ptr), d(d) {
    }

    void PointSequenceViewModelSelectableManipulatorInterface::setSelected(const QVariant &item, bool selected) {
        item.value<QObject *>()->setProperty(d->selectedProperty, selected);
    }

    bool PointSequenceViewModelSelectableManipulatorInterface::isSelected(const QVariant &item) const {
        return item.value<QObject *>()->property(d->selectedProperty).toBool();
    }

    QVariant PointSequenceViewModelSelectableManipulatorInterface::nextItem(const QVariant &item) const {
        return QVariant::fromValue(d->container.nextItem(item.value<QObject *>()));
    }

    QVariant PointSequenceViewModelSelectableManipulatorInterface::previousItem(const QVariant &item) const {
        return QVariant::fromValue(d->container.previousItem(item.value<QObject *>()));
    }

    QVariant PointSequenceViewModelSelectableManipulatorInterface::firstItem() const {
        return QVariant::fromValue(d->container.firstItem());
    }

    QVariant PointSequenceViewModelSelectableManipulatorInterface::lastItem() const {
        return QVariant::fromValue(d->container.lastItem());
    }

    QVariant PointSequenceViewModelSelectableManipulatorInterface::currentItem() const {
        return QVariant::fromValue(d->currentItem);
    }

    void PointSequenceViewModelSelectableManipulatorInterface::setCurrentItem(const QVariant &item) {
        d->q_ptr->setCurrentItem(item.value<QObject *>());
    }

    QVariantList PointSequenceViewModelSelectableManipulatorInterface::selection() const {
        QVariantList ret;
        ret.reserve(d->selection.size());
        std::ranges::transform(std::as_const(d->selection), std::back_inserter(ret), [](auto item) {
            return QVariant::fromValue(item);
        });
        return ret;
    }

    int PointSequenceViewModelSelectableManipulatorInterface::compareOrder(const QVariant &item1, const QVariant &item2) const {
        return item1.value<QObject *>()->property(d->positionProperty).toInt() - item2.value<QObject *>()->property(d->positionProperty).toInt();
    }

    bool PointSequenceViewModelSelectableManipulatorInterface::isValidItem(const QVariant &item) const {
        return static_cast<bool>(item.value<QObject *>());
    }

    qsizetype PointSequenceViewModelSelectableManipulatorInterface::getId(const QVariant &item) const {
        return reinterpret_cast<qsizetype>(item.value<QObject *>());
    }

    QVariant PointSequenceViewModelSelectableManipulatorInterface::fromId(qsizetype id) const {
        return QVariant::fromValue(reinterpret_cast<QObject *>(id));
    }

    QObject *PointSequenceViewModelSelectableManipulatorInterface::viewModel() const {
        return d->q_ptr;
    }

    PointSequenceViewModelSliceableManipulatorInterface::PointSequenceViewModelSliceableManipulatorInterface(PointSequenceViewModelPrivate *d) : SliceableViewModelManipulatorInterface(d->q_ptr), d_ptr(d) {
    }
    PointSequenceViewModelSliceableManipulatorInterface::~PointSequenceViewModelSliceableManipulatorInterface() = default;

    QObjectList PointSequenceViewModelSliceableManipulatorInterface::items() const {
        Q_D(const PointSequenceViewModel);
        return d->container.m_positions.keys();
    }

    QObjectList PointSequenceViewModelSliceableManipulatorInterface::slice(int position, int length) const {
        Q_D(const PointSequenceViewModel);
        return d->container.slice(position, length);
    }
    int PointSequenceViewModelSliceableManipulatorInterface::itemPosition(QObject *item) const {
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
            Q_EMIT q->itemSelected(item);
        } else {
            selection.remove(item);
            Q_EMIT q->itemDeselected(item);
        }
        Q_EMIT q->selectionCountChanged(selection.size());
        Q_EMIT q->selectionChanged();
    }
    void PointSequenceViewModelPrivate::handleItemPositionChanged() {
        Q_Q(PointSequenceViewModel);
        auto item = q->sender();
        container.insertItem(item, item->property(positionProperty).toInt());
        Q_EMIT q->itemUpdated(item);
        Q_EMIT q->itemsChanged();
    }


    PointSequenceViewModel::PointSequenceViewModel(QObject *parent, const QByteArray &positionProperty, const QByteArray &selectedProperty) : QObject(parent), d_ptr(new PointSequenceViewModelPrivate) {
        Q_D(PointSequenceViewModel);
        d->q_ptr = this;
        d->iSliceable = new PointSequenceViewModelSliceableManipulatorInterface(d);
        d->iSelectable = new PointSequenceViewModelSelectableManipulatorInterface(d);
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
            Q_EMIT currentItemChanged(item);
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
        if (d->container.m_positions.contains(item))
            return;
        Q_EMIT itemAboutToInsert(item);
        d->insertItem(item);
        Q_EMIT itemInserted(item);
        Q_EMIT countChanged(count());
        Q_EMIT itemsChanged();

    }
    void PointSequenceViewModel::removeItem(QObject *item) {
        Q_D(PointSequenceViewModel);
        if (!d->container.m_positions.contains(item))
            return;
        Q_EMIT itemAboutToRemove(item);
        d->removeItem(item);
        Q_EMIT itemRemoved(item);
        Q_EMIT countChanged(count());
        Q_EMIT itemsChanged();
    }

    int PointSequenceViewModel::selectionCount() const {
        Q_D(const PointSequenceViewModel);
        return d->selection.size();
    }

    QObjectList PointSequenceViewModel::selection() const {
        Q_D(const PointSequenceViewModel);
        return d->selection.values();
    }
}

#include "moc_PointSequenceViewModel.cpp"
#include "moc_PointSequenceViewModel_p.cpp"
