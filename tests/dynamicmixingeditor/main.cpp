#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QVariant>

#include <SVSCraftCore/MusicTimeline.h>

#include <ScopicFlowCore/ClavierViewModel.h>
#include <ScopicFlowCore/DynamicMixingAnchorViewModel.h>
#include <ScopicFlowCore/DynamicMixingEditorInteractionController.h>
#include <ScopicFlowCore/DynamicMixingViewModel.h>
#include <ScopicFlowCore/ScrollBehaviorViewModel.h>
#include <ScopicFlowCore/StandardSelectionController.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/TimeViewModel.h>

using namespace sflow;

namespace {

    constexpr int timelineEnd = 7200;

    DynamicMixingAnchorViewModel *addAnchor(DynamicMixingViewModel &viewModel,
                                             int position,
                                             const QList<double> &ratio) {
        auto *anchor = new DynamicMixingAnchorViewModel(&viewModel);
        anchor->setPosition(position);
        anchor->setRatio(ratio);
        if (!viewModel.insertItem(anchor)) {
            delete anchor;
            return nullptr;
        }
        return anchor;
    }

}

int main(int argc, char **argv) {
    QGuiApplication application(argc, argv);

    auto surfaceFormat = QSurfaceFormat::defaultFormat();
    surfaceFormat.setSamples(8);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    QQuickStyle::setStyle("SVSCraft.UIComponents");
    QQuickStyle::setFallbackStyle("Basic");

    ScrollBehaviorViewModel scrollBehaviorViewModel(&application);
    scrollBehaviorViewModel.setAutoScroll(true);

    TimeLayoutViewModel timeLayoutViewModel(&application);
    timeLayoutViewModel.setPixelDensity(0.24);
    timeLayoutViewModel.setPositionAlignment(120);

    SVS::MusicTimeline musicTimeline(&application);
    TimeViewModel timeViewModel(&application);
    timeViewModel.setTimeline(&musicTimeline);
    timeViewModel.setEnd(timelineEnd);

    ClavierViewModel clavierViewModel(&application);

    DynamicMixingViewModel dynamicMixingViewModel(&application);
    dynamicMixingViewModel.setVoiceCount(5);
    addAnchor(dynamicMixingViewModel, 240, {0.42, 0.20});
    addAnchor(dynamicMixingViewModel, 1200, {0.12, 0.40, 0.10, 0.12});
    addAnchor(dynamicMixingViewModel, 2280, {0.08, 0.12, 0.52, 0.08});
    addAnchor(dynamicMixingViewModel, 3600, {0.20, 0.08, 0.16, 0.36});
    addAnchor(dynamicMixingViewModel, 5280, {0.18, 0.22, 0.10, 0.14, 0.05});

    StandardSelectionController selectionController(&dynamicMixingViewModel, &application);
    DynamicMixingEditorInteractionController interactionController(&application);

    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
        {"timeViewModel", QVariant::fromValue(&timeViewModel)},
        {"timeLayoutViewModel", QVariant::fromValue(&timeLayoutViewModel)},
        {"clavierViewModel", QVariant::fromValue(&clavierViewModel)},
        {"dynamicMixingViewModel", QVariant::fromValue(&dynamicMixingViewModel)},
        {"selectionController", QVariant::fromValue(&selectionController)},
        {"interactionController", QVariant::fromValue(&interactionController)},
    });
    engine.load("qrc:/qt/qml/dev/sjimo/ScopicFlow/Test/DynamicMixingEditor/main.qml");

    return application.exec();
}
