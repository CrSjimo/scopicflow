#ifndef SCOPIC_FLOW_RUBBERBANDSELECTOR_P_P_H
#define SCOPIC_FLOW_RUBBERBANDSELECTOR_P_P_H

#include <memory>
#include <unordered_map>

#include <QPointer>

#include <ScopicFlowInternal/private/RubberBandSelector_p.h>

namespace sflow {

    class RubberBandSelectorIndex;
    struct RubberBandSelectorItem;

    class RubberBandSelectorPrivate {
        Q_DECLARE_PUBLIC(RubberBandSelector)

    public:
        RubberBandSelectorPrivate();
        ~RubberBandSelectorPrivate();

        RubberBandSelectorIndex *ensureIndex();

        RubberBandSelector *q_ptr = nullptr;

        QPointer<SelectionController> selectionController;
        RubberBandSelector::ItemHint itemHint = RubberBandSelector::None;
        std::unordered_map<QObject *, std::unique_ptr<RubberBandSelectorItem>> items;
        std::unique_ptr<RubberBandSelectorIndex> index;
        quint64 nextItemId = 0;
    };

}

#endif // SCOPIC_FLOW_RUBBERBANDSELECTOR_P_P_H
