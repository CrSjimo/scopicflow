#include "SelectableViewModelManipulatorInterface_p.h"

#include <QHash>

namespace sflow {

    SelectableViewModelManipulatorInterface::SelectableViewModelManipulatorInterface(QObject *parent) : QObject(parent) {
    }

    void SelectableViewModelManipulatorInterface::select(const QVariant &item, int button, int modifiers) {
        if (!isValidItem(item)) {
            if (!(modifiers & Qt::ControlModifier) && !(modifiers & Qt::ShiftModifier)) {
                for (const auto &o : selection())
                    setSelected(o, false);
            }
            return;
        }
        if (!(modifiers & Qt::ControlModifier) && !(button == Qt::RightButton && isSelected(item))) {
            for (const auto &o : selection())
                setSelected(o, false);
        }
        if (modifiers & Qt::ShiftModifier) {
            if (!isValidItem(currentItem())) {
                setCurrentItem(item);
            }
            int order = compareOrder(currentItem(), item);
            if (order < 0) {
                for (auto o = currentItem(); isValidItem(o); o = nextItem(o)) {
                    setSelected(o, true);
                    if (o == item)
                        break;
                }
            } else if (order > 0) {
                for (auto o = currentItem(); isValidItem(o); o = previousItem(o)) {
                    setSelected(o, true);
                    if (o == item)
                        break;
                }
            } else {
                setSelected(currentItem(), true);
                setSelected(item, true);
            }
            return;
        }
        setCurrentItem(item);
        if (modifiers & Qt::ControlModifier) {
            if (button == Qt::RightButton) {
                setSelected(item, true);
            } else {
                setSelected(item, !isSelected(item));
            }
            return;
        }
        setSelected(item, true);
    }

}
