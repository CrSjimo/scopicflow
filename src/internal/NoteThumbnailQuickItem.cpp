#include "NoteThumbnailQuickItem_p.h"
#include "NoteThumbnailQuickItem_p_p.h"

#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

#include <ScopicFlowCore/NoteViewModel.h>
#include <ScopicFlowCore/RangeSequenceViewModel.h>

namespace sflow {

    NoteThumbnailQuickItem::NoteThumbnailQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new NoteThumbnailQuickItemPrivate) {
        Q_D(NoteThumbnailQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents, true);
    }

    NoteThumbnailQuickItem::~NoteThumbnailQuickItem() = default;

    QColor NoteThumbnailQuickItem::color() const {
        Q_D(const NoteThumbnailQuickItem);
        return d->color;
    }

    void NoteThumbnailQuickItem::setColor(const QColor &color) {
        Q_D(NoteThumbnailQuickItem);
        if (d->color == color)
            return;
        d->color = color;
        Q_EMIT colorChanged();
        update();
    }

    RangeSequenceViewModel *NoteThumbnailQuickItem::noteSequenceViewModel() const {
        Q_D(const NoteThumbnailQuickItem);
        return d->noteSequenceViewModel;
    }

    void NoteThumbnailQuickItem::setNoteSequenceViewModel(RangeSequenceViewModel *noteSequenceViewModel) {
        Q_D(NoteThumbnailQuickItem);
        if (d->noteSequenceViewModel == noteSequenceViewModel)
            return;
        d->resetNoteSequenceViewModel(noteSequenceViewModel);
        Q_EMIT noteSequenceViewModelChanged();
        update();
    }

    double NoteThumbnailQuickItem::horizontalFactor() const {
        Q_UNUSED(this);
        return 1.0 / 256.0; // TODO
    }

    int NoteThumbnailQuickItem::maxKey() const {
        Q_D(const NoteThumbnailQuickItem);
        return d->calculateMaxKey();
    }

    int NoteThumbnailQuickItem::minKey() const {
        Q_D(const NoteThumbnailQuickItem);
        return d->calculateMinKey();
    }

    QSGNode *NoteThumbnailQuickItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *) {
        Q_D(NoteThumbnailQuickItem);
        if (!node) {
            node = new QSGNode;
        }
        for (auto itemModel : d->pendingRemoveItems) {
            auto itemNode = d->itemModelToNode.take(itemModel);
            if (!itemNode)
                continue;
            node->removeChildNode(itemNode);
            delete itemNode;
        }
        d->pendingRemoveItems.clear();
        if (d->color != d->nodeColor) {
            d->nodeColor = d->color;
            for (auto itemNode : d->itemModelToNode) {
                static_cast<QSGFlatColorMaterial *>(itemNode->material())->setColor(d->color);
                itemNode->markDirty(QSGNode::DirtyMaterial);
            }
        }
        for (auto itemModel : d->pendingUpdateItems) {
            auto itemNode = d->itemModelToNode.value(itemModel);
            if (!itemNode) {
                itemNode = new QSGGeometryNode;
                itemNode->setFlag(QSGNode::OwnedByParent);
                itemNode->setFlag(QSGNode::OwnsGeometry);
                itemNode->setFlag(QSGNode::OwnsMaterial);
                auto geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 4);
                geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
                geometry->setLineWidth(1);
                itemNode->setGeometry(geometry);
                auto material = new QSGFlatColorMaterial;
                material->setColor(d->color);
                itemNode->setMaterial(material);
                node->appendChildNode(itemNode);
                d->itemModelToNode.insert(itemModel, itemNode);
            }
            auto geometry = itemNode->geometry();
            geometry->vertexDataAsPoint2D()[0].set(horizontalFactor() * itemModel->position(), 128 - itemModel->key());
            geometry->vertexDataAsPoint2D()[1].set(horizontalFactor() * (itemModel->position() + itemModel->length()), 128 - itemModel->key());
            geometry->vertexDataAsPoint2D()[2].set(horizontalFactor() * itemModel->position(), 127 - itemModel->key());
            geometry->vertexDataAsPoint2D()[3].set(horizontalFactor() * (itemModel->position() + itemModel->length()), 127 - itemModel->key());
            itemNode->markDirty(QSGNode::DirtyGeometry);
        }
        d->pendingUpdateItems.clear();
        return node;
    }

    void NoteThumbnailQuickItemPrivate::resetNoteSequenceViewModel(RangeSequenceViewModel *viewModel) {
        Q_Q(NoteThumbnailQuickItem);
        if (noteSequenceViewModel == viewModel)
            return;
        if (noteSequenceViewModel) {
            QObject::disconnect(noteSequenceViewModel, nullptr, q, nullptr);
            detachAllItems();
        }
        noteSequenceViewModel = viewModel;
        if (noteSequenceViewModel) {
            QObject::connect(noteSequenceViewModel, &RangeSequenceViewModel::itemInserted, q, [this](QObject *item) {
                if (auto noteViewModel = qobject_cast<NoteViewModel *>(item)) {
                    onItemInserted(noteViewModel);
                }
            });
            QObject::connect(noteSequenceViewModel, &RangeSequenceViewModel::itemRemoved, q, [this](QObject *item) {
                if (auto noteViewModel = qobject_cast<NoteViewModel *>(item)) {
                    onItemRemoved(noteViewModel);
                }
            });
            QObject::connect(noteSequenceViewModel, &RangeSequenceViewModel::itemUpdated, q, [this](QObject *item) {
                if (auto noteViewModel = qobject_cast<NoteViewModel *>(item)) {
                    onItemUpdated(noteViewModel);
                }
            });
            for (auto item : noteSequenceViewModel->items()) {
                if (auto noteViewModel = qobject_cast<NoteViewModel *>(item)) {
                    onItemInserted(noteViewModel);
                }
            }
        }
    }

    void NoteThumbnailQuickItemPrivate::detachAllItems() {
        if (!noteSequenceViewModel)
            return;
        for (auto item : noteSequenceViewModel->items()) {
            if (auto noteViewModel = qobject_cast<NoteViewModel *>(item)) {
                detachItem(noteViewModel);
            }
        }
        keyCountMap.clear();
        itemToKeyMap.clear();
    }

    void NoteThumbnailQuickItemPrivate::detachItem(NoteViewModel *itemModel) {
        Q_Q(NoteThumbnailQuickItem);
        if (!itemModel)
            return;

        if (itemToKeyMap.contains(itemModel)) {
            int oldKey = itemToKeyMap.value(itemModel);
            removeKeyEntry(oldKey);
            itemToKeyMap.remove(itemModel);
        }
        
        QObject::disconnect(itemModel, nullptr, q, nullptr);
        pendingRemoveItems.insert(itemModel);
        pendingUpdateItems.remove(itemModel);
        q->update();
    }

    void NoteThumbnailQuickItemPrivate::onItemInserted(NoteViewModel *noteViewModel) {
        Q_Q(NoteThumbnailQuickItem);
        if (!noteViewModel)
            return;
        detachItem(noteViewModel);

        itemToKeyMap.insert(noteViewModel, noteViewModel->key());
        addKeyEntry(noteViewModel->key());
        
        QObject::connect(noteViewModel, &NoteViewModel::positionChanged, q, [this, noteViewModel]() {
            onItemUpdated(noteViewModel);
        });
        QObject::connect(noteViewModel, &NoteViewModel::keyChanged, q, [this, noteViewModel]() {
            if (itemToKeyMap.contains(noteViewModel)) {
                int oldKey = itemToKeyMap.value(noteViewModel);
                int newKey = noteViewModel->key();
                onItemKeyChanged(noteViewModel, oldKey, newKey);
            }
        });
        QObject::connect(noteViewModel, &NoteViewModel::lengthChanged, q, [this, noteViewModel]() {
            onItemUpdated(noteViewModel);
        });
        onItemUpdated(noteViewModel);
    }

    void NoteThumbnailQuickItemPrivate::onItemUpdated(NoteViewModel *itemModel) {
        Q_Q(NoteThumbnailQuickItem);
        pendingUpdateItems.insert(itemModel);
        q->update();
    }

    void NoteThumbnailQuickItemPrivate::onItemKeyChanged(NoteViewModel *itemModel, int oldKey, int newKey) {
        if (oldKey != newKey) {
            itemToKeyMap[itemModel] = newKey;
            removeKeyEntry(oldKey);
            addKeyEntry(newKey);
            onItemUpdated(itemModel);
        }
    }

    void NoteThumbnailQuickItemPrivate::removeKeyEntry(int key) {
        Q_Q(NoteThumbnailQuickItem);
        bool minChanged = key == calculateMinKey() && keyCountMap.value(key) == 1;
        bool maxChanged = key == calculateMaxKey() && keyCountMap.value(key) == 1;
        if (keyCountMap.contains(key)) {
            int count = keyCountMap[key];
            if (count > 1) {
                keyCountMap[key] = count - 1;
            } else {
                keyCountMap.remove(key);
            }
        }
        if (minChanged) {
            Q_EMIT q->minKeyChanged();
        }
        if (maxChanged) {
            Q_EMIT q->maxKeyChanged();
        }
    }

    void NoteThumbnailQuickItemPrivate::addKeyEntry(int key) {
        Q_Q(NoteThumbnailQuickItem);
        bool minChanged = key < calculateMinKey();
        bool maxChanged = key > calculateMaxKey();
        keyCountMap[key] = keyCountMap.value(key, 0) + 1;
        if (minChanged) {
            Q_EMIT q->minKeyChanged();
        }
        if (maxChanged) {
            Q_EMIT q->maxKeyChanged();
        }
    }

    int NoteThumbnailQuickItemPrivate::calculateMinKey() const {
        if (keyCountMap.isEmpty()) {
            return 128;
        }
        return keyCountMap.firstKey();
    }

    int NoteThumbnailQuickItemPrivate::calculateMaxKey() const {
        if (keyCountMap.isEmpty()) {
            return -1;
        }
        return keyCountMap.lastKey();
    }

    void NoteThumbnailQuickItemPrivate::onItemRemoved(NoteViewModel *itemModel) {
        detachItem(itemModel);
    }

}

#include "moc_NoteThumbnailQuickItem_p.cpp"
