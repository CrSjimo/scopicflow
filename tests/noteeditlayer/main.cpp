#include <QDebug>
#include <QGuiApplication>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QQmlApplicationEngine>
#include <QQuickItem>

#include <SVSCraftCore/MusicTimeline.h>
#include <SVSCraftFluentSystemIcons/FluentSystemIconsImageProvider.h>

#include <ScopicFlowCore/StandardSelectionController.h>
#include <ScopicFlowCore/ScrollBehaviorViewModel.h>
#include <ScopicFlowCore/TimeViewModel.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/RangeSequenceViewModel.h>
#include <ScopicFlowCore/NoteViewModel.h>
#include <ScopicFlowCore/NoteEditLayerInteractionController.h>
#include <ScopicFlowCore/ClavierViewModel.h>
#include <ScopicFlowCore/ClipViewModel.h>
#include <ScopicFlowCore/ListViewModel.h>
#include <ScopicFlowCore/TrackViewModel.h>

using namespace sflow;

int main(int argc, char **argv) {
    QGuiApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QQuickStyle::setStyle("SVSCraft.UIComponents");
    QQuickStyle::setFallbackStyle("Basic");

    ScrollBehaviorViewModel scrollBehaviorViewModel(&a);

    TimeLayoutViewModel timeLayoutViewModel(&a);
    timeLayoutViewModel.setPositionAlignment(240);

    SVS::MusicTimeline musicTimeline(&a);

    TimeViewModel timeViewModel(&a);
    timeViewModel.setTimeline(&musicTimeline);

    ClavierViewModel clavierViewModel(&a);

    RangeSequenceViewModel noteSequenceViewModel(&a);

    for (int i = 0; i < 600; ++i) {
        auto noteViewModel = new NoteViewModel(&a);
        noteViewModel->setKey(60 + i % 12);
        noteViewModel->setLyric(QString::number(i));
        noteViewModel->setAdditionalText(QString::number(i));
        noteViewModel->setPosition(240 * i);
        noteViewModel->setLength(240);
        noteViewModel->setAdditionalTextHighlighted(i % 5 == 0);
        noteSequenceViewModel.insertItem(noteViewModel);
    }

    ClipViewModel clipViewModel(&a);
    clipViewModel.setPosition(19200);
    clipViewModel.setClipStart(1920);
    clipViewModel.setLength(19200);
    clipViewModel.setAssociatedNoteSequence(&noteSequenceViewModel);

    RangeSequenceViewModel noteSequenceViewModel2(&a);

    for (int i = 0; i < 600; ++i) {
        auto noteViewModel = new NoteViewModel(&a);
        noteViewModel->setKey(60 + i % 12);
        noteViewModel->setLyric(QString::number(i));
        noteViewModel->setAdditionalText(QString::number(i));
        noteViewModel->setPosition(240 * i);
        noteViewModel->setLength(240);
        noteViewModel->setAdditionalTextHighlighted(i % 5 == 0);
        noteSequenceViewModel2.insertItem(noteViewModel);
    }

    ClipViewModel clipViewModel2(&a);
    clipViewModel2.setPosition(42240);
    clipViewModel2.setLength(19200);
    clipViewModel2.setTrackIndex(1);
    clipViewModel2.setAssociatedNoteSequence(&noteSequenceViewModel2);

    RangeSequenceViewModel clipSequenceViewModel;
    clipSequenceViewModel.insertItem(&clipViewModel);
    clipSequenceViewModel.insertItem(&clipViewModel2);

    ListViewModel trackListViewModel(&a);
    TrackViewModel trackViewModel(&a);
    trackViewModel.setColor(Qt::darkGreen);
    trackListViewModel.insertItem(0, &trackViewModel);
    TrackViewModel trackViewModel2(&a);
    trackViewModel2.setColor(Qt::darkMagenta);
    trackListViewModel.insertItem(1, &trackViewModel2);

    StandardSelectionController selectionController(&noteSequenceViewModel, &a);
    NoteEditLayerInteractionController noteEditLayerInteractionController(&a);

    QQmlApplicationEngine engine;
    SVS::FluentSystemIconsImageProvider::addToEngine(&engine);
    engine.setInitialProperties({
        {"clavierViewModel", QVariant::fromValue(&clavierViewModel)},
        {"selectionController", QVariant::fromValue(&selectionController)},
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
        {"timeViewModel", QVariant::fromValue(&timeViewModel)},
        {"timeLayoutViewModel", QVariant::fromValue(&timeLayoutViewModel)},
        {"noteSequenceViewModel", QVariant::fromValue(&noteSequenceViewModel)},
        {"noteSequenceViewModel2", QVariant::fromValue(&noteSequenceViewModel2)},
        {"noteEditLayerInteractionController", QVariant::fromValue(&noteEditLayerInteractionController)},
        {"clipSequenceViewModel", QVariant::fromValue(&clipSequenceViewModel)},
        {"clipViewModel", QVariant::fromValue(&clipViewModel)},
        {"trackListViewModel", QVariant::fromValue(&trackListViewModel)},
    });
    engine.load(":/qt/qml/dev/sjimo/ScopicFlow/Test/NoteEditLayer/main.qml");

    return a.exec();
}
