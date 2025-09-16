#include "InteractionControllerNotifier.h"

namespace sflow {
    InteractionControllerNotifier::InteractionControllerNotifier(QObject *parent) : QObject(parent) {
    }
    InteractionControllerNotifier::~InteractionControllerNotifier() = default;
    bool InteractionControllerNotifier::handleItemInteraction(ScopicFlow::ItemInteraction interactionType, QObject *model, int index, QObject *containerModel, int flags) {
        return filterItemInteraction(interactionType, model, index, containerModel, flags);
    }
    bool InteractionControllerNotifier::handleSceneInteraction(ScopicFlow::ItemInteraction interactionType, QObject *sceneModel, QObject *behaviorModel, int position, int value, int flags) {
        return filterSceneInteraction(interactionType, sceneModel, behaviorModel, position, value, flags);
    }
    bool InteractionControllerNotifier::filterItemInteraction(ScopicFlow::ItemInteraction, QObject *, int, QObject *, int) {
        return false;
    }
    bool InteractionControllerNotifier::filterSceneInteraction(ScopicFlow::ItemInteraction, QObject *, QObject *, int, int, int) {
        return false;
    }
}
