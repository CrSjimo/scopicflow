#include "StandardSelectionController.h"
#include "StandardSelectionController_p.h"

#include <utility>

#include <QVariant>

#include <ScopicFlowCore/private/SliceableViewModelManipulatorInterface_p.h>
#include <ScopicFlowCore/ListViewModel.h>

namespace sflow {

    void StandardSelectionControllerPrivate::addToSelection(QObject *item) {
        selection.insert(item);
        item->setProperty(selectedProperty.toUtf8(), true);
    }

    void StandardSelectionControllerPrivate::removeFromSelection(QObject *item) {
        selection.remove(item);
        item->setProperty(selectedProperty.toUtf8(), false);
    }

    void StandardSelectionControllerPrivate::clearSelection() {
        for (auto *item : selection) {
            item->setProperty(selectedProperty.toUtf8(), false);
        }
        selection.clear();
    }

    StandardSelectionController::StandardSelectionController(QObject *viewModel, const QString &selectedProperty, QObject *parent) : SelectionController(parent), d_ptr(new StandardSelectionControllerPrivate) {
        Q_D(StandardSelectionController);
        d->q_ptr = this;
        d->selectedProperty = selectedProperty;
        d->viewModel = viewModel;

    }
    StandardSelectionController::~StandardSelectionController() = default;

    QObjectList StandardSelectionController::getSelectedItems() const {
        Q_D(const StandardSelectionController);
        return d->selection.values();
    }
    QObjectList StandardSelectionController::getItemsBetween(QObject *startItem, QObject *endItem) const {
        Q_D(const StandardSelectionController);
        if (auto iSliceable = d->viewModel->property("iSliceable").value<SliceableViewModelManipulatorInterface *>()) {
            return iSliceable->itemsBetween(startItem, endItem);
        }
        if (auto listViewModel = qobject_cast<ListViewModel *>(d->viewModel)) {
            auto items = listViewModel->items();
            auto startIndex = items.indexOf(startItem);
            auto endIndex = items.indexOf(endItem);
            if (startIndex == -1 || endIndex == -1) {
                return {};
            }
            if (startIndex > endIndex) {
                std::swap(startIndex, endIndex);
            }
            return items.mid(startIndex, endIndex - startIndex + 1);
        }
        return {};
    }
    void StandardSelectionController::select(QObject *item, SelectionCommand command) {
        Q_D(StandardSelectionController);

        if (command & ClearPreviousSelection) {
            d->clearSelection();
        }

        if (command & SetCurrentItem) {
            if (d->currentItem != item) {
                d->currentItem = item;
                Q_EMIT currentItemChanged();
            }
        }

        if (item) {
            if ((command & Select) && (command & Deselect)) {
                // Toggle
                if (d->selection.contains(item)) {
                    d->removeFromSelection(item);
                } else {
                    d->addToSelection(item);
                }
            } else if (command & Select) {
                // Select only
                if (!d->selection.contains(item)) {
                    d->addToSelection(item);
                }
            } else if (command & Deselect) {
                // Deselect only
                if (d->selection.contains(item)) {
                    d->removeFromSelection(item);
                }
            }
        }

    }
    QObject *StandardSelectionController::currentItem() const {
        Q_D(const StandardSelectionController);
        return d->currentItem;
    }

}
