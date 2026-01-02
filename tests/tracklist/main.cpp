#include <QDebug>
#include <QGuiApplication>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QQmlApplicationEngine>
#include <QQuickItem>

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

    QObject::connect(&trackListInteractionController, &TrackListInteractionController::rubberBandDraggingStarted, &a, [](QQuickItem *trackList) {
        qDebug() << "rubberBandDraggingStarted" << trackList;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::rubberBandDraggingFinished, &a, [](QQuickItem *trackList) {
        qDebug() << "rubberBandDraggingFinished" << trackList;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::hoverEntered, &a, [](QQuickItem *trackList) {
        qDebug() << "hoverEntered" << trackList;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::hoverExited, &a, [](QQuickItem *trackList) {
        qDebug() << "hoverExited" << trackList;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::doubleClicked, &a, [](QQuickItem *trackList) {
        qDebug() << "doubleClicked" << trackList;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::contextMenuRequested, &a, [](QQuickItem *trackList) {
        qDebug() << "contextMenuRequested" << trackList;
    });

    QObject::connect(&trackListInteractionController, &TrackListInteractionController::dragMovingStarted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "dragMovingStarted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::dragMovingCommitted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "dragMovingCommitted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::dragMovingAborted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "dragMovingAborted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::muteEditingStarted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "muteEditingStarted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::muteEditingFinished, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "muteEditingFinished" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::soloEditingStarted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "soloEditingStarted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::soloEditingFinished, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "soloEditingFinished" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::recordEditingStarted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "recordEditingStarted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::recordEditingFinished, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "recordEditingFinished" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::nameEditingStarted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "nameEditingStarted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::nameEditingCommitted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "nameEditingCommitted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::nameEditingAborted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "nameEditingAborted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::gainEditingStarted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "gainEditingStarted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::gainEditingCommitted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "gainEditingCommitted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::gainEditingAborted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "gainEditingAborted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::panEditingStarted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "panEditingStarted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::panEditingCommitted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "panEditingCommitted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::panEditingAborted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "panEditingAborted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::heightAdjustingStarted, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "heightAdjustingStarted" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::heightAdjustingFinished, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "heightAdjustingFinished" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::levelMeterClicked, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "levelMeterClicked" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::itemHoverEntered, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "itemHoverEntered" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::itemHoverExited, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "itemHoverExited" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::itemColorIndicatorHoverEntered, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "itemColorIndicatorHoverEntered" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::itemColorIndicatorHoverExited, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "itemColorIndicatorHoverExited" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::itemLevelMeterHoverEntered, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "itemLevelMeterHoverEntered" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::itemLevelMeterHoverExited, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "itemLevelMeterHoverExited" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::itemColorIndicatorClicked, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "itemColorIndicatorClicked" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::itemLevelMeterClicked, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "itemLevelMeterClicked" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::itemDoubleClicked, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "itemDoubleClicked" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::itemContextMenuRequested, &a, [](QQuickItem *trackList, int index) {
        qDebug() << "itemContextMenuRequested" << trackList << index;
    });
    QObject::connect(&trackListInteractionController, &TrackListInteractionController::copyItemsRequested, &a, [](QQuickItem *trackList, int targetIndex) {
        qDebug() << "copyItemsRequested" << trackList << targetIndex;
    });

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
