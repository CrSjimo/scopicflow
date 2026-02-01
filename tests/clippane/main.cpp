#include <QDebug>
#include <QGuiApplication>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QQmlApplicationEngine>
#include <QQuickItem>

#include <SVSCraftCore/MusicTimeline.h>
#include <SVSCraftFluentSystemIcons/FluentSystemIconsImageProvider.h>

#include <ScopicFlowCore/TrackViewModel.h>
#include <ScopicFlowCore/ListViewModel.h>
#include <ScopicFlowCore/TrackListInteractionController.h>
#include <ScopicFlowCore/StandardSelectionController.h>
#include <ScopicFlowCore/ScrollBehaviorViewModel.h>
#include <ScopicFlowCore/TrackListLayoutViewModel.h>
#include <ScopicFlowCore/TimeViewModel.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/RangeSequenceViewModel.h>
#include <ScopicFlowCore/ClipViewModel.h>
#include <ScopicFlowCore/ClipPaneInteractionController.h>

using namespace sflow;

int main(int argc, char **argv) {
    QGuiApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QQuickStyle::setStyle("SVSCraft.UIComponents");
    QQuickStyle::setFallbackStyle("Basic");

    ScrollBehaviorViewModel scrollBehaviorViewModel(&a);

    ListViewModel trackListViewModel(&a);

    for (int i = 0; i < 4; i++) {
        auto trackViewModel = new TrackViewModel(&a);
        trackViewModel->setName("Track " + QString::number(i));
        trackViewModel->setColor(QColor::fromHsl(i * 60, 100, 80));
        trackViewModel->setLeftLevel(-i * 6);
        trackViewModel->setRightClipping(true);
        trackListViewModel.insertItem(i, trackViewModel);
    }

    TrackListLayoutViewModel trackListLayoutViewModel(&a);

    TimeLayoutViewModel timeLayoutViewModel(&a);
    timeLayoutViewModel.setPositionAlignment(240);

    SVS::MusicTimeline musicTimeline(&a);

    TimeViewModel timeViewModel(&a);
    timeViewModel.setTimeline(&musicTimeline);

    RangeSequenceViewModel clipSequenceViewModel(&a);
    for (int i = 0; i < 8; i++) {
        auto clipViewModel = new ClipViewModel(&a);
        clipViewModel->setName("Clip " + QString::number(i));
        clipViewModel->setTrackIndex(i % 4);
        clipViewModel->setPosition(i * 480);
        clipViewModel->setLength(1440);
        clipViewModel->setMaxLength(1920);
        clipViewModel->setIconSource(i % 2 ? QUrl("image://fluent-system-icons/mic") : QUrl("image://fluent-system-icons/sound_wave_circle"));
        clipViewModel->setOverlapped(i == 6);
        clipSequenceViewModel.insertItem(clipViewModel);
    }

    StandardSelectionController selectionController(&clipSequenceViewModel, &a);
    ClipPaneInteractionController clipPaneInteractionController(&a);

    QQmlApplicationEngine engine;
    SVS::FluentSystemIconsImageProvider::addToEngine(&engine);
    engine.setInitialProperties({
        {"trackListViewModel", QVariant::fromValue(&trackListViewModel)},
        {"selectionController", QVariant::fromValue(&selectionController)},
        {"trackListLayoutViewModel", QVariant::fromValue(&trackListLayoutViewModel)},
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
        {"timeViewModel", QVariant::fromValue(&timeViewModel)},
        {"timeLayoutViewModel", QVariant::fromValue(&timeLayoutViewModel)},
        {"clipSequenceViewModel", QVariant::fromValue(&clipSequenceViewModel)},
        {"clipPaneInteractionController", QVariant::fromValue(&clipPaneInteractionController)},
    });
    engine.load(":/qt/qml/dev/sjimo/ScopicFlow/Test/ClipPane/main.qml");

    return a.exec();
}
