#include <QGuiApplication>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QQmlApplicationEngine>

#include <SVSCraftCore/MusicTimeline.h>

#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/TimeViewModel.h>
#include <ScopicFlowCore/PlaybackViewModel.h>
#include <ScopicFlowCore/ScrollBehaviorViewModel.h>

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

    ScrollBehaviorViewModel scrollBehaviorViewModel(&a);
    scrollBehaviorViewModel.setAffectVelocity(true);

    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        {"timeLayoutViewModel", QVariant::fromValue(&timeLayoutViewModel)},
        {"timeViewModel", QVariant::fromValue(&timeViewModel)},
        {"playbackViewModel", QVariant::fromValue(&playbackViewModel)},
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)}
    });
    engine.load(":/qt/qml/dev/sjimo/ScopicFlow/Test/Timeline/main.qml");

    return a.exec();
}
