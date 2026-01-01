#include <QGuiApplication>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QQmlApplicationEngine>

#include <SVSCraftFluentSystemIcons/FluentSystemIconsImageProvider.h>

#include <ScopicFlowCore/TrackViewModel.h>
#include <ScopicFlowCore/ListViewModel.h>
#include <ScopicFlowCore/TrackListInteractionController.h>
#include <ScopicFlowCore/StandardSelectionController.h>
#include <ScopicFlowCore/ScrollBehaviorViewModel.h>
#include <ScopicFlowCore/TrackListLayoutViewModel.h>

using namespace sflow;

int main(int argc, char **argv) {
    QGuiApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QQuickStyle::setStyle("SVSCraft.UIComponents");
    QQuickStyle::setFallbackStyle("Basic");

    ScrollBehaviorViewModel scrollBehaviorViewModel(&a);

    TrackListInteractionController trackListInteractionController(&a);

    ListViewModel trackListViewModel(&a);

    for (int i = 0; i < 4; i++) {
        auto trackViewModel = new TrackViewModel(&a);
        trackViewModel->setName("Track " + QString::number(i));
        trackViewModel->setColor(QColor::fromHsl(i * 60, 100, 80));
        trackViewModel->setLeftLevel(-i * 6);
        trackViewModel->setRightClipping(true);
        trackListViewModel.insertItem(i, trackViewModel);
    }

    StandardSelectionController selectionController(&trackListViewModel, &a);

    TrackListLayoutViewModel trackListLayoutViewModel(&a);

    QQmlApplicationEngine engine;
    SVS::FluentSystemIconsImageProvider::addToEngine(&engine);
    engine.setInitialProperties({
        {"trackListViewModel", QVariant::fromValue(&trackListViewModel)},
        {"selectionController", QVariant::fromValue(&selectionController)},
        {"trackListLayoutViewModel", QVariant::fromValue(&trackListLayoutViewModel)},
        {"trackListInteractionController", QVariant::fromValue(&trackListInteractionController)},
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
    });
    engine.load(":/qt/qml/dev/sjimo/ScopicFlow/Test/TrackList/main.qml");

    return a.exec();
}
