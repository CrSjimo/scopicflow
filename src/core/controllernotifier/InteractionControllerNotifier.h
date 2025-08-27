#ifndef SCOPIC_FLOW_INTERACTIONCONTROLLERNOTIFIER_H
#define SCOPIC_FLOW_INTERACTIONCONTROLLERNOTIFIER_H

#include <QObject>

#include <ScopicFlowCore/ScopicFlowNamespace.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT InteractionControllerNotifier : public QObject {
        Q_OBJECT
    public:
        explicit InteractionControllerNotifier(QObject *parent = nullptr);
        ~InteractionControllerNotifier() override;

        Q_INVOKABLE bool handleItemInteraction(ScopicFlow::ItemInteraction interactionType, QObject *model, int index, QObject *containerModel, int flags = 0);
        Q_INVOKABLE bool handleSceneInteraction(ScopicFlow::ItemInteraction interactionType, QObject *sceneModel, QObject *behaviorModel, int position, int value, int flags = 0);


    signals:
        void itemInteracted(ScopicFlow::ItemInteraction interactionType, QObject *model, int index, QObject *containerModel, int flags = 0);
        void sceneInteracted(ScopicFlow::ItemInteraction interactionType, QObject *sceneModel, QObject *behaviorModel, int position, int value, int flags = 0);

    protected:
        virtual bool filterItemInteraction(ScopicFlow::ItemInteraction interactionType, QObject *model, int index, QObject *containerModel, int flags);
        virtual bool filterSceneInteraction(ScopicFlow::ItemInteraction interactionType, QObject *sceneModel, QObject *behaviorModel, int position, int value, int flags);

    };

}

#endif //SCOPIC_FLOW_INTERACTIONCONTROLLERNOTIFIER_H
