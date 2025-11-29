#include "SelectionController.h"

#include <QVariant>

namespace sflow {
    SelectionController::SelectionController(QObject *parent) : QObject(parent) {

    }

    SelectionController::~SelectionController() = default;

    void SelectionController::selectByMouse(QObject *item, Qt::MouseButton button, Qt::KeyboardModifiers modifiers, const QString &selectedProperty) {
        if (!item) {
            if (!(modifiers & Qt::ControlModifier) && !(modifiers & Qt::ShiftModifier)) {
                select(nullptr, ClearPreviousSelection);
            }
            return;
        }
        if (!(modifiers & Qt::ControlModifier) && !(button == Qt::RightButton && item->property(selectedProperty.toUtf8()).toBool())) {
            if (!(modifiers & Qt::ControlModifier) && !(modifiers & Qt::ShiftModifier)) {
                select(nullptr, ClearPreviousSelection);
            }
        }
        if (modifiers & Qt::ShiftModifier) {
            if (!currentItem()) {
                select(item, SetCurrentItem);
            }
            for (auto o : getItemsBetween(currentItem(), item)) {
                select(o, Select);
            }
            return;
        }
        select(item, SetCurrentItem);
        if (modifiers & Qt::ControlModifier) {
            if (button == Qt::RightButton) {
                select(item, Select);
            } else {
                select(item, Toggle);
            }
            return;
        }
        select(item, Select);
    }
}
