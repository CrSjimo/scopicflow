#include "PianoRollNoteAreaRubberBandHelper_p.h"
#include "PianoRollNoteAreaRubberBandHelper_p_p.h"

#include <ScopicFlowCore/private/RangeSequenceViewModel_p.h>
#include <ScopicFlowInternal/private/RubberBandLayerQuickItem_p.h>

namespace sflow {

    static QMetaMethod handleItemUpdatedMethod;

    void PianoRollNoteAreaRubberBandHelperPrivate::handleItemUpdated() {
        Q_Q(PianoRollNoteAreaRubberBandHelper);
        handleItemUpdated(q->sender());
    }
    void PianoRollNoteAreaRubberBandHelperPrivate::handleItemUpdated(QObject *item) {
        Q_Q(PianoRollNoteAreaRubberBandHelper);
        if (rubberBandLayer) {
            rubberBandLayer->insertItem(QVariant::fromValue(item), QRectF(item->property("position").toInt(), item->property("key").toInt(), item->property("length").toInt(), 1));
        }
    }
    void PianoRollNoteAreaRubberBandHelperPrivate::handleItemInserted(QObject *item) {
        Q_Q(PianoRollNoteAreaRubberBandHelper);
        for (int i = item->metaObject()->propertyOffset(); i < item->metaObject()->propertyCount(); i++) {
            auto property = item->metaObject()->property(i);
            auto propertyName = QByteArray(property.name());
            if (propertyName == "position" || propertyName == "length" || propertyName == "key") {
                QObject::connect(item, property.notifySignal(), q, handleItemUpdatedMethod);
            }
        }
        handleItemUpdated(item);
    }
    void PianoRollNoteAreaRubberBandHelperPrivate::handleItemRemoved(QObject *item) {
        Q_Q(PianoRollNoteAreaRubberBandHelper);
        if (rubberBandLayer)
            rubberBandLayer->removeItem(QVariant::fromValue(item));
        item->disconnect(q);
    }


    PianoRollNoteAreaRubberBandHelper::PianoRollNoteAreaRubberBandHelper(QObject *parent) : QObject(parent), d_ptr(new PianoRollNoteAreaRubberBandHelperPrivate) {
        Q_D(PianoRollNoteAreaRubberBandHelper);
        d->q_ptr = this;
        if (!handleItemUpdatedMethod.isValid()) {
            for (int i = staticMetaObject.methodOffset(); i < staticMetaObject.methodCount(); i++) {
                auto method = staticMetaObject.method(i);
                if (method.name() == "handleItemUpdated") {
                    handleItemUpdatedMethod = method;
                    break;
                }
            }
        }
        Q_ASSERT(handleItemUpdatedMethod.isValid());
    }
    PianoRollNoteAreaRubberBandHelper::~PianoRollNoteAreaRubberBandHelper() = default;
    RangeSequenceViewModel *PianoRollNoteAreaRubberBandHelper::noteSequenceViewModel() const {
        Q_D(const PianoRollNoteAreaRubberBandHelper);
        return d->noteSequenceViewModel;
    }
    void PianoRollNoteAreaRubberBandHelper::setNoteSequenceViewModel(RangeSequenceViewModel *noteSequenceViewModel) {
        Q_D(PianoRollNoteAreaRubberBandHelper);
        if (d->noteSequenceViewModel == noteSequenceViewModel)
            return;
        if (d->handle) {
            for (auto item : d->handle->items()) {
                d->handleItemRemoved(item);
            }
            d->handle->disconnect(this);
        }
        d->noteSequenceViewModel = noteSequenceViewModel;
        if (!noteSequenceViewModel) {
            d->handle = nullptr;
            return;
        }
        d->handle = noteSequenceViewModel->property("handle").value<SliceableViewModelQmlHandle *>();
        connect(d->handle, &RangeSequenceViewModelQmlHandle::itemInserted, this, [=](QObject *item) {
            d->handleItemInserted(item);
        });
        connect(d->handle, &RangeSequenceViewModelQmlHandle::itemRemoved, this, [=](QObject *item) {
            d->handleItemRemoved(item);
        });
        for (auto item : d->handle->items()) {
            d->handleItemInserted(item);
        }
        emit noteSequenceViewModelChanged();
    }
    RubberBandLayerQuickItem *PianoRollNoteAreaRubberBandHelper::rubberBandLayer() const {
        Q_D(const PianoRollNoteAreaRubberBandHelper);
        return d->rubberBandLayer;
    }
    void PianoRollNoteAreaRubberBandHelper::setRubberBandLayer(RubberBandLayerQuickItem *rubberBandLayer) {
        Q_D(PianoRollNoteAreaRubberBandHelper);
        if (d->rubberBandLayer == rubberBandLayer)
            return;
        d->rubberBandLayer = rubberBandLayer;
        if (rubberBandLayer && d->handle) {
            for (auto item : d->handle->items()) {
                d->handleItemInserted(item);
            }
        }
        emit rubberBandLayerChanged();
    }
}

#include "moc_PianoRollNoteAreaRubberBandHelper_p.cpp"