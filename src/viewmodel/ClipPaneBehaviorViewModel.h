#ifndef SCOPIC_FLOW_CLIPPANEBEHAVIORVIEWMODEL_H
#define SCOPIC_FLOW_CLIPPANEBEHAVIORVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowNamespace.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT ClipPaneBehaviorViewModel : public QQmlPropertyMap {
        Q_OBJECT

    public:
        explicit ClipPaneBehaviorViewModel(QObject *parent = nullptr);
        ~ClipPaneBehaviorViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, lengthHint, lengthHint, setLengthHint, resetLengthHint)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(ScopicFlow::MouseBehavior, mouseBehavior, mouseBehavior, setMouseBehavior, resetMouseBehavior)
    };

}

#endif //SCOPIC_FLOW_CLIPPANEBEHAVIORVIEWMODEL_H
