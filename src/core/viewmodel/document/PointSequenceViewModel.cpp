#include "PointSequenceViewModel.h"
#include "PointSequenceViewModel_p.h"

#include <ScopicFlowCore/private/ViewModelHelper_p.h>

namespace sflow {

    PointSequenceViewModelSliceableManipulatorInterface::PointSequenceViewModelSliceableManipulatorInterface(PointSequenceViewModelPrivate *d) : SliceableViewModelManipulatorInterface(d->q_ptr), d_ptr(d) {
        auto q = d->q_ptr;
        connect(q, &PointSequenceViewModel::itemUpdated, this, &PointSequenceViewModelSliceableManipulatorInterface::itemUpdated);
        connect(q, &PointSequenceViewModel::itemAboutToInsert, this, &PointSequenceViewModelSliceableManipulatorInterface::itemAboutToInsert);
        connect(q, &PointSequenceViewModel::itemInserted, this, &PointSequenceViewModelSliceableManipulatorInterface::itemInserted);
        connect(q, &PointSequenceViewModel::itemAboutToRemove, this, &PointSequenceViewModelSliceableManipulatorInterface::itemAboutToRemove);
        connect(q, &PointSequenceViewModel::itemRemoved, this, &PointSequenceViewModelSliceableManipulatorInterface::itemRemoved);
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
    QObjectList PointSequenceViewModelSliceableManipulatorInterface::itemsBetween(QObject *item1, QObject *item2) const {
        Q_D(const PointSequenceViewModel);
        auto position1 = item1->property(d->positionProperty).toInt();
        auto position2 = item2->property(d->positionProperty).toInt();
        QObjectList ret;
        if (position1 < position2) {
            for (auto o = item1; o; o = d->container.nextItem(o)) {
                ret.append(o);
                if (o == item2)
                    break;
            }
        } else if (position1 > position2) {
            for (auto o = item1; o; o = d->container.previousItem(o)) {
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
    QObject *PointSequenceViewModelSliceableManipulatorInterface::previousItem(QObject *item) const {
        Q_D(const PointSequenceViewModel);
        return d->container.previousItem(item);
    }
    QObject *PointSequenceViewModelSliceableManipulatorInterface::nextItem(QObject *item) const {
        Q_D(const PointSequenceViewModel);
        return d->container.nextItem(item);
    }

    static QMetaMethod handleItemPositionChangedMetaMethod;


    void PointSequenceViewModelPrivate::insertItem(QObject *item) {
        Q_Q(PointSequenceViewModel);
        container.insertItem(item, item->property(positionProperty).toInt());
        ViewModelHelper::connectPropertyNotify(item, positionProperty, q, handleItemPositionChangedMetaMethod);
    }
    void PointSequenceViewModelPrivate::removeItem(QObject *item) {
        Q_Q(PointSequenceViewModel);
        container.removeItem(item);
        QObject::disconnect(item, nullptr, q, nullptr);
    }
    void PointSequenceViewModelPrivate::handleItemPositionChanged() {
        Q_Q(PointSequenceViewModel);
        auto item = q->sender();
        container.insertItem(item, item->property(positionProperty).toInt());
        Q_EMIT q->itemUpdated(item);
        Q_EMIT q->itemsChanged();
    }


    PointSequenceViewModel::PointSequenceViewModel(QObject *parent, const QByteArray &positionProperty) : QObject(parent), d_ptr(new PointSequenceViewModelPrivate) {
        Q_D(PointSequenceViewModel);
        d->q_ptr = this;
        d->iSliceable = new PointSequenceViewModelSliceableManipulatorInterface(d);
        d->positionProperty = positionProperty;
        if (!handleItemPositionChangedMetaMethod.isValid()) {
            for (int i = staticMetaObject.methodOffset(); i < staticMetaObject.methodCount(); i++) {
                auto method = staticMetaObject.method(i);
                if (method.name() == "handleItemPositionChanged") {
                    handleItemPositionChangedMetaMethod = method;
                }
                if (handleItemPositionChangedMetaMethod.isValid())
                    break;
            }
        }
        Q_ASSERT(handleItemPositionChangedMetaMethod.isValid());
    }
    PointSequenceViewModel::~PointSequenceViewModel() = default;
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

}

#include "moc_PointSequenceViewModel.cpp"
#include "moc_PointSequenceViewModel_p.cpp"
