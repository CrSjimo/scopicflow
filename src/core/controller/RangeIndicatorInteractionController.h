#ifndef SCOPIC_FLOW_RANGEINDICATORINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_RANGEINDICATORINTERACTIONCONTROLLER_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class RangeIndicatorViewModel;

    class SCOPIC_FLOW_CORE_EXPORT RangeIndicatorInteractionController : public QObject {
        Q_OBJECT
        QML_ELEMENT

    public:
        explicit RangeIndicatorInteractionController(QObject *parent = nullptr);
        ~RangeIndicatorInteractionController() override;

    Q_SIGNALS:
        void clicked(QQuickItem *rangeIndicatorSequence, int position);
        void itemClicked(QQuickItem *rangeIndicatorSequence, RangeIndicatorViewModel *item);

        void hoverEntered(QQuickItem *rangeIndicatorSequence, int position);
        void hoverMoved(QQuickItem *rangeIndicatorSequence, int position);
        void hoverExited(QQuickItem *rangeIndicatorSequence);

        void itemHoverEntered(QQuickItem *rangeIndicatorSequence, RangeIndicatorViewModel *item);
        void itemHoverExited(QQuickItem *rangeIndicatorSequence, RangeIndicatorViewModel *item);

        void doubleClicked(QQuickItem *rangeIndicatorSequence, int position);
        void itemDoubleClicked(QQuickItem *rangeIndicatorSequence, RangeIndicatorViewModel *item);

        void contextMenuRequested(QQuickItem *rangeIndicatorSequence, int position);
        void itemContextMenuRequested(QQuickItem *rangeIndicatorSequence, RangeIndicatorViewModel *item);
    };

}

#endif // SCOPIC_FLOW_RANGEINDICATORINTERACTIONCONTROLLER_H
