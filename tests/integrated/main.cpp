#include <memory>
#include <random>

#include <QApplication>
#include <QSurfaceFormat>
#include <QMainWindow>
#include <QGridLayout>
#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QMenu>
#include <QInputDialog>
#include <QMenuBar>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QFileDialog>
#include <QSplitter>
#include <QQuickView>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQmlEngineExtensionPlugin>
#include <QMessageBox>
#include <QQuickStyle>
#include <QQmlApplicationEngine>
#include <QTimer>

#include <SVSCraftCore/MusicTimeline.h>
#include <SVSCraftCore/MusicTime.h>
#include <SVSCraftCore/MusicTimeSignature.h>
#include <SVSCraftQuick/Theme.h>
#include <SVSCraftGui/ColorChange.h>

#include <ScopicFlow/TimeViewModel.h>
#include <ScopicFlow/TimeLayoutViewModel.h>
#include <ScopicFlow/PlaybackViewModel.h>
#include <ScopicFlow/ClavierViewModel.h>
#include <ScopicFlow/ScrollBehaviorViewModel.h>
#include <ScopicFlow/AnimationViewModel.h>
#include <ScopicFlow/PointSequenceViewModel.h>
#include <ScopicFlow/LabelViewModel.h>
#include <ScopicFlow/ListViewModel.h>
#include <ScopicFlow/TrackListLayoutViewModel.h>
#include <ScopicFlow/TrackViewModel.h>
#include <ScopicFlow/NoteViewModel.h>
#include <ScopicFlow/LabelSequenceBehaviorViewModel.h>
#include <ScopicFlow/RangeSequenceViewModel.h>
#include <ScopicFlow/PianoRollNoteAreaBehaviorViewModel.h>
#include <ScopicFlow/ClipViewModel.h>
#include <ScopicFlow/ClipPaneBehaviorViewModel.h>
#include <ScopicFlow/BusTrackViewModel.h>
#include <ScopicFlow/AnchorViewModel.h>

using namespace sflow;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QQuickStyle::setStyle("SVSCraft.UIComponents");
    QQuickStyle::setFallbackStyle("Basic");

    QMainWindow win;
    win.resize(1280, 800);
    auto splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);

    TimeLayoutViewModel timeLayoutViewModel(&win);
    timeLayoutViewModel.setPositionAlignment(240);

    TimeLayoutViewModel arrangementTimeLayoutViewModel(&win);
    arrangementTimeLayoutViewModel.setPositionAlignment(480);

    SVS::MusicTimeline musicTimeline(&win);

    TimeViewModel timeViewModel(&win);
    timeViewModel.setTimeline(&musicTimeline);
    TimeViewModel arrangementTimeViewModel(&win);
    arrangementTimeViewModel.setTimeline(&musicTimeline);

    PlaybackViewModel playbackViewModel(&win);

    ClavierViewModel clavierViewModel(&win);

    ScrollBehaviorViewModel scrollBehaviorViewModel(&win);

    AnimationViewModel animationViewModel(&win);

    PointSequenceViewModel labelSequenceViewModel(&win);
    for (int i = 0; i < 16; i++) {
        auto label = new LabelViewModel(&win);
        label->setPosition(i * 240);
        label->setContent("test" + QString::number(i));
        labelSequenceViewModel.insertItem(label);
    }
    std::mt19937 generator(114514);

    ListViewModel trackListViewModel(&win);
    QObjectList tracks;
    std::uniform_real_distribution<float> tchd(0.0, 1.0);
    for (int i = 0; i < 8; i++) {
        auto track = new TrackViewModel(&win);
        track->setName("Track " + QString::number(i + 1));
        track->setColor(QColor::fromHsvF(tchd(generator), 0.8, 1));
        tracks.append(track);
    }
    trackListViewModel.setItems(tracks);
    QTimer timer;
    timer.setInterval(20);
    timer.setSingleShot(false);
    timer.callOnTimeout([&] {
        for (auto track : tracks) {
            static_cast<TrackViewModel *>(track)->setLeftLevel(std::uniform_real_distribution(-60.0, 0.5)(generator));
        }

    });

    TrackListLayoutViewModel trackListLayoutViewModel(&win);

    LabelSequenceBehaviorViewModel labelSequenceBehaviorViewModel(&win);
    LabelSequenceBehaviorViewModel arrangementLabelSequenceBehaviorViewModel(&win);
    labelSequenceBehaviorViewModel.setColor(0x805e14);
    arrangementLabelSequenceBehaviorViewModel.setColor(0x574b90);


    std::uniform_int_distribution<int> distribution(-60, 60);

    RangeSequenceViewModel backNoteSequenceViewModel(&win);
    for (int i = 0, k = 48, p = 0; i < 16; i++) {
        auto note = new NoteViewModel(&win);
        note->setPosition(p);
        note->setLength(960);
        p += note->length();
        note->setKey(k = k + (distribution(generator) + (48 - k) * 5) / 20);
        note->setLyric(QString::number(i));
        backNoteSequenceViewModel.insertItem(note);
    }

    PianoRollNoteAreaBehaviorViewModel pianoRollNoteAreaBehaviorViewModel(&win);
    pianoRollNoteAreaBehaviorViewModel.setColor(0x3498cb);
    pianoRollNoteAreaBehaviorViewModel.setLengthHint(960);
    PianoRollNoteAreaBehaviorViewModel backPianoRollNoteAreaBehaviorViewModel(&win);
    backPianoRollNoteAreaBehaviorViewModel.setColor(0x3498cb);
    backPianoRollNoteAreaBehaviorViewModel.setMouseBehavior(ScopicFlow::MB_None);
    backPianoRollNoteAreaBehaviorViewModel.setCompactDisplay(true);

    RangeSequenceViewModel clipSequenceViewModel(&win);
    for (int i = 0; i < 16; i++) {
        auto clip = new ClipViewModel(&win);
        clip->setPosition(i * 480);
        clip->setLength(1440);
        clip->setTrackNumber(i % 8);
        clip->setName(QString::number(i));
        auto clipNoteSequenceViewModel = new RangeSequenceViewModel(&win);
        for (int i = 0, k = 48, p = 0; i < 16; i++) {
            auto note = new NoteViewModel(&win);
            note->setPosition(p);
            note->setLength(960);
            p += note->length();
            note->setKey(k = k + (distribution(generator) + (48 - k) * 5) / 20);
            note->setLyric(QString::number(i));
            clipNoteSequenceViewModel->insertItem(note);
        }
        QObject::connect(clipNoteSequenceViewModel, &RangeSequenceViewModel::itemInserted, [&](QObject *item) {
            item->setParent(&win);
            item->setProperty("lyric", "a");
        });
        QObject::connect(clipNoteSequenceViewModel, &RangeSequenceViewModel::itemRemoved, [&](QObject *item) {
            item->deleteLater();
        });
        clip->insert("noteSequenceViewModel", QVariant::fromValue(clipNoteSequenceViewModel));
        clipSequenceViewModel.insertItem(clip);
    }
    QObject::connect(&clipSequenceViewModel, &RangeSequenceViewModel::itemInserted, [&](QObject *clip) {
        qobject_cast<ClipViewModel *>(clip)->insert("noteSequenceViewModel", QVariant::fromValue(new RangeSequenceViewModel));
    });
    QObject::connect(&clipSequenceViewModel, &RangeSequenceViewModel::itemRemoved, [&](QObject *clip) {
        clip->property("noteSequenceViewModel").value<QObject *>()->deleteLater();
        clip->deleteLater();
    });

    ClipPaneBehaviorViewModel clipPaneBehaviorViewModel(&win);
    clipPaneBehaviorViewModel.setLengthHint(480);

    TrackListLayoutViewModel mixerLayoutViewModel(&win);
    TrackListLayoutViewModel busMixerLayoutViewModel(&win);

    ListViewModel busTrackListViewModel(&win);
    BusTrackViewModel masterTrackViewModel(&win);
    masterTrackViewModel.setName("Master");
    masterTrackViewModel.setColor(Qt::transparent);
    masterTrackViewModel.setRoute(false);
    BusTrackViewModel metronomeViewModel(&win);
    metronomeViewModel.setName("Metronome");
    metronomeViewModel.setColor(Qt::transparent);
    BusTrackViewModel audioDeviceViewModel(&win);
    audioDeviceViewModel.setName("Audio Device");
    audioDeviceViewModel.setColor(Qt::transparent);
    busTrackListViewModel.setItems({&masterTrackViewModel, &metronomeViewModel, &audioDeviceViewModel});

    PointSequenceViewModel anchoredCurveViewModel(&win);
    for (int i = 0; i < 256; i++) {
        auto anchor = new AnchorViewModel(&win);
        anchor->setPosition((i % 16) * 120 + (i / 16) * 32 * 120);
        anchor->setAnchorValue(distribution(generator));
        anchor->setAnchorType(i % 16 == 15 ? ScopicFlow::AT_Break : ScopicFlow::AT_Pchip);
        anchoredCurveViewModel.insertItem(anchor);
    }

    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        {"timeViewModel", QVariant::fromValue(&timeViewModel)},
        {"arrangementTimeViewModel", QVariant::fromValue(&arrangementTimeViewModel)},
        {"timeLayoutViewModel", QVariant::fromValue(&timeLayoutViewModel)},
        {"arrangementTimeLayoutViewModel", QVariant::fromValue(&arrangementTimeLayoutViewModel)},
        {"trackListViewModel", QVariant::fromValue(&trackListViewModel)},
        {"trackListLayoutViewModel", QVariant::fromValue(&trackListLayoutViewModel)},
        {"clavierViewModel", QVariant::fromValue(&clavierViewModel)},
        {"labelSequenceViewModel", QVariant::fromValue(&labelSequenceViewModel)},
        {"playbackViewModel", QVariant::fromValue(&playbackViewModel)},
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
        {"animationViewModel", QVariant::fromValue(&animationViewModel)},
        {"labelSequenceBehaviorViewModel", QVariant::fromValue(&labelSequenceBehaviorViewModel)},
        {"arrangementLabelSequenceBehaviorViewModel", QVariant::fromValue(&arrangementLabelSequenceBehaviorViewModel)},
        {"pianoRollNoteAreaBehaviorViewModel", QVariant::fromValue(&pianoRollNoteAreaBehaviorViewModel)},
        {"backNoteSequenceViewModel", QVariant::fromValue(&backNoteSequenceViewModel)},
        {"backPianoRollNoteAreaBehaviorViewModel", QVariant::fromValue(&backPianoRollNoteAreaBehaviorViewModel)},
        {"clipSequenceViewModel", QVariant::fromValue(&clipSequenceViewModel)},
        {"clipPaneBehaviorViewModel", QVariant::fromValue(&clipPaneBehaviorViewModel)},
        {"mixerLayoutViewModel", QVariant::fromValue(&mixerLayoutViewModel)},
        {"busTrackListViewModel", QVariant::fromValue(&busTrackListViewModel)},
        {"busMixerLayoutViewModel", QVariant::fromValue(&busMixerLayoutViewModel)},
        {"levelTimer", QVariant::fromValue(&timer)},
        {"anchoredCurveViewModel", QVariant::fromValue(&anchoredCurveViewModel)}
    });
    engine.load(QUrl("qrc:/qt/qml/dev/sjimo/ScopicFlow/Test/main.qml"));

    return a.exec();
}

#include "main.moc"
