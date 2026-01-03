#include <QGuiApplication>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QQmlApplicationEngine>

#include <SVSCraftCore/MusicTimeline.h>

#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/TimeViewModel.h>
#include <ScopicFlowCore/PlaybackViewModel.h>
#include <ScopicFlowCore/ScrollBehaviorViewModel.h>
#include <ScopicFlowCore/TimelineInteractionController.h>

using namespace sflow;

int main(int argc, char **argv) {
    QGuiApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QQuickStyle::setStyle("SVSCraft.UIComponents");
    QQuickStyle::setFallbackStyle("Basic");

    TimeLayoutViewModel timeLayoutViewModel(&a);
    timeLayoutViewModel.setPositionAlignment(240);

    TimeLayoutViewModel arrangementTimeLayoutViewModel(&a);
    arrangementTimeLayoutViewModel.setPositionAlignment(480);

    SVS::MusicTimeline musicTimeline(&a);

    TimeViewModel timeViewModel(&a);
    timeViewModel.setTimeline(&musicTimeline);

    PlaybackViewModel playbackViewModel(&a);
    playbackViewModel.setLoopStart(1920);
    playbackViewModel.setLoopLength(4800);

    ScrollBehaviorViewModel scrollBehaviorViewModel(&a);
    scrollBehaviorViewModel.setAutoScroll(true);

    TimelineInteractionController timelineInteractionController(&a);

    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        {"timeLayoutViewModel", QVariant::fromValue(&timeLayoutViewModel)},
        {"timeViewModel", QVariant::fromValue(&timeViewModel)},
        {"playbackViewModel", QVariant::fromValue(&playbackViewModel)},
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
        {"timelineInteractionController", QVariant::fromValue(&timelineInteractionController)}
    });
    engine.load(":/qt/qml/dev/sjimo/ScopicFlow/Test/Timeline/main.qml");

    return a.exec();
}
