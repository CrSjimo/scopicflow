#include "RangeSequenceViewModel.h"
#include "RangeSequenceViewModel_p.h"

#include <ScopicFlowCore/private/ViewModelHelper_p.h>

namespace sflow {

    RangeSequenceViewModelSliceableManipulatorInterface::RangeSequenceViewModelSliceableManipulatorInterface(RangeSequenceViewModelPrivate *d) : SliceableViewModelManipulatorInterface(d->q_ptr), d_ptr(d) {
        auto q = d->q_ptr;
        connect(q, &RangeSequenceViewModel::itemUpdated, this, &RangeSequenceViewModelSliceableManipulatorInterface::itemUpdated);
        connect(q, &RangeSequenceViewModel::itemAboutToInsert, this, &RangeSequenceViewModelSliceableManipulatorInterface::itemAboutToInsert);
        connect(q, &RangeSequenceViewModel::itemInserted, this, &RangeSequenceViewModelSliceableManipulatorInterface::itemInserted);
        connect(q, &RangeSequenceViewModel::itemAboutToRemove, this, &RangeSequenceViewModelSliceableManipulatorInterface::itemAboutToRemove);
        connect(q, &RangeSequenceViewModel::itemRemoved, this, &RangeSequenceViewModelSliceableManipulatorInterface::itemRemoved);
    }
    RangeSequenceViewModelSliceableManipulatorInterface::~RangeSequenceViewModelSliceableManipulatorInterface() = default;

    QObjectList RangeSequenceViewModelSliceableManipulatorInterface::items() const {
        Q_D(const RangeSequenceViewModel);
        return d->pointContainer.m_positions.keys();
    }

    QObjectList RangeSequenceViewModelSliceableManipulatorInterface::slice(int position, int length) const {
        Q_D(const RangeSequenceViewModel);
        return d->rangeContainer.slice(position, length);
    }
    int RangeSequenceViewModelSliceableManipulatorInterface::itemPosition(QObject *item) const {
        Q_D(const RangeSequenceViewModel);
        return item->property(d->positionProperty).toInt();
    }
    QObjectList RangeSequenceViewModelSliceableManipulatorInterface::itemsBetween(QObject *item1, QObject *item2) const {
        Q_D(const RangeSequenceViewModel);
        auto position1 = item1->property(d->positionProperty).toInt();
        auto position2 = item2->property(d->positionProperty).toInt();
        QObjectList ret;
        if (position1 < position2) {
            for (auto o = item1; o; o = d->pointContainer.nextItem(o)) {
                ret.append(o);
                if (o == item2)
                    break;
            }
        } else if (position1 > position2) {
            for (auto o = item1; o; o = d->pointContainer.previousItem(o)) {
                ret.append(o);
                if (o == item2)
                    break;
            }
        } else {
            ret.append(item1);
            ret.append(item2);
        }
        return ret;
    }
    QObject *RangeSequenceViewModelSliceableManipulatorInterface::previousItem(QObject *item) const {
        Q_D(const RangeSequenceViewModel);
        return d->pointContainer.previousItem(item);
    }
    QObject *RangeSequenceViewModelSliceableManipulatorInterface::nextItem(QObject *item) const {
        Q_D(const RangeSequenceViewModel);
        return d->pointContainer.nextItem(item);
    }

    static QMetaMethod handleItemPositionChangedMetaMethod;
    static QMetaMethod handleItemLengthChangedMetaMethod;


    void RangeSequenceViewModelPrivate::insertItem(QObject *item) {
        Q_Q(RangeSequenceViewModel);
        auto position = item->property(positionProperty).toInt();
        auto length = item->property(lengthProperty).toInt();
        pointContainer.insertItem(item, position);
        auto affectedItems = rangeContainer.insertItem(item, position, length);
        ViewModelHelper::connectPropertyNotify(item, positionProperty, q, handleItemPositionChangedMetaMethod);
        ViewModelHelper::connectPropertyNotify(item, lengthProperty, q, handleItemLengthChangedMetaMethod);
        for (auto affectedItem : affectedItems) {
            Q_EMIT q->itemUpdated(affectedItem);
        }
    }
    void RangeSequenceViewModelPrivate::removeItem(QObject *item) {
        Q_Q(RangeSequenceViewModel);
        pointContainer.removeItem(item);
        auto affectedItems = rangeContainer.removeItem(item);
        QObject::disconnect(item, nullptr, q, nullptr);
        for (auto affectedItem : affectedItems) {
            Q_EMIT q->itemUpdated(affectedItem);
        }
    }
    void RangeSequenceViewModelPrivate::handleItemPositionChanged() {
        Q_Q(RangeSequenceViewModel);
        auto item = q->sender();
        auto position = item->property(positionProperty).toInt();
        auto length = item->property(lengthProperty).toInt();
        pointContainer.insertItem(item, position);
        auto affectedItems = rangeContainer.insertItem(item, position, length);
        for (auto affectedItem : affectedItems) {
            Q_EMIT q->itemUpdated(affectedItem);
        }
        Q_EMIT q->itemsChanged();
    }
    void RangeSequenceViewModelPrivate::handleItemLengthChanged() {
        Q_Q(RangeSequenceViewModel);
        auto item = q->sender();
        auto position = item->property(positionProperty).toInt();
        auto length = item->property(lengthProperty).toInt();
        auto affectedItems = rangeContainer.insertItem(item, position, length);
        for (auto affectedItem : affectedItems) {
            Q_EMIT q->itemUpdated(affectedItem);
        }
        Q_EMIT q->itemsChanged();
    }


    RangeSequenceViewModel::RangeSequenceViewModel(QObject *parent, const QByteArray &positionProperty, const QByteArray &lengthProperty) : QObject(parent), d_ptr(new RangeSequenceViewModelPrivate) {
        Q_D(RangeSequenceViewModel);
        d->q_ptr = this;
        d->iSliceable = new RangeSequenceViewModelSliceableManipulatorInterface(d);
        d->positionProperty = positionProperty;
        d->lengthProperty = lengthProperty;
        if (!handleItemPositionChangedMetaMethod.isValid() || !handleItemLengthChangedMetaMethod.isValid()) {
            for (int i = staticMetaObject.methodOffset(); i < staticMetaObject.methodCount(); i++) {
                auto method = staticMetaObject.method(i);
                if (method.name() == "handleItemPositionChanged") {
                    handleItemPositionChangedMetaMethod = method;
                }
                if (method.name() == "handleItemLengthChanged") {
                    handleItemLengthChangedMetaMethod = method;
                }
                if (handleItemPositionChangedMetaMethod.isValid() && handleItemLengthChangedMetaMethod.isValid())
                    break;
            }
        }
        Q_ASSERT(handleItemPositionChangedMetaMethod.isValid());
        Q_ASSERT(handleItemLengthChangedMetaMethod.isValid());
    }
    RangeSequenceViewModel::~RangeSequenceViewModel() = default;
    int RangeSequenceViewModel::count() const {
        Q_D(const RangeSequenceViewModel);
        return d->pointContainer.m_items.size();
    }
    QObjectList RangeSequenceViewModel::items() const {
        Q_D(const RangeSequenceViewModel);
        return d->pointContainer.m_positions.keys();
    }
    void RangeSequenceViewModel::insertItem(QObject *item) {
        Q_D(RangeSequenceViewModel);
        if (d->pointContainer.m_positions.contains(item))
            return;
        Q_EMIT itemAboutToInsert(item);
        d->insertItem(item);
        Q_EMIT itemInserted(item);
        Q_EMIT countChanged(count());
        Q_EMIT itemsChanged();

    }
    void RangeSequenceViewModel::removeItem(QObject *item) {
        Q_D(RangeSequenceViewModel);
        if (!d->pointContainer.m_positions.contains(item))
            return;
        Q_EMIT itemAboutToRemove(item);
        d->removeItem(item);
        Q_EMIT itemRemoved(item);
        Q_EMIT countChanged(count());
        Q_EMIT itemsChanged();
    }

}

#include "moc_RangeSequenceViewModel.cpp"
#include "moc_RangeSequenceViewModel_p.cpp"
