#ifndef SCOPIC_FLOW_STANDARDSELECTIONCONTROLLER_H
#define SCOPIC_FLOW_STANDARDSELECTIONCONTROLLER_H

#include <ScopicFlowCore/SelectionController.h>

namespace sflow {

    class StandardSelectionControllerPrivate;

    class SCOPIC_FLOW_CORE_EXPORT StandardSelectionController : public SelectionController {
        Q_OBJECT
        Q_DECLARE_PRIVATE(StandardSelectionController)
    public:
        explicit StandardSelectionController(QObject *viewModel, QObject *parent = nullptr) : StandardSelectionController(viewModel, "selected", parent) {

        }
        explicit StandardSelectionController(QObject *viewModel, const QString &selectedProperty, QObject *parent = nullptr);
        ~StandardSelectionController() override;

        QObjectList getSelectedItems() const override;
        QObjectList getItemsBetween(QObject *startItem, QObject *endItem) const override;
        void select(QObject *item, SelectionCommand command) override;
        QObject *currentItem() const override;
        bool editScopeFocused() const override;

    private:
        QScopedPointer<StandardSelectionControllerPrivate> d_ptr;
    };

}

#endif //SCOPIC_FLOW_STANDARDSELECTIONCONTROLLER_H
