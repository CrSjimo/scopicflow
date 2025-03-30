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

using namespace sflow;

static SVS::MusicTimeSignature promptTimeSignature(QWidget *parent, SVS::MusicTimeSignature initialValue) {
    QDialog dlg(parent);
    auto layout = new QVBoxLayout;
    auto numeratorSpinBox = new QSpinBox;
    numeratorSpinBox->setRange(1, 32);
    layout->addWidget(numeratorSpinBox);
    auto denominatorComboBox = new QComboBox;
    denominatorComboBox->addItems({"1", "2", "4", "8", "16", "32"});
    layout->addWidget(denominatorComboBox);
    auto okButton = new QPushButton("OK");
    okButton->setDefault(true);
    layout->addWidget(okButton);

    dlg.setLayout(layout);

    numeratorSpinBox->setValue(initialValue.numerator());
    denominatorComboBox->setCurrentText(QString::number(initialValue.denominator()));
    QObject::connect(okButton, &QPushButton::clicked, &dlg, &QDialog::accept);

    if (dlg.exec() == QDialog::Accepted) {
        return {numeratorSpinBox->value(), denominatorComboBox->currentText().toInt()};
    } else {
        return {0, 0};
    }
}

static QObject *loadCustomPalette(QWidget *parent) {
    static auto engine = [] {
        auto engine = std::make_unique<QQmlEngine>();
        engine->addImportPath("qrc:/ScopicFlow/modules");
        return engine;
    }();
    auto file = QFileDialog::getOpenFileName(parent);
    if (file.isEmpty())
        return nullptr;
    QQmlComponent component(engine.get(), QUrl::fromLocalFile(file));
    QObject *customPalette = component.create();
    return customPalette;
}

class MySlotHandler : public QObject {
    Q_OBJECT
public:
    QWidget *win;
    TimeViewModel *timeViewModel;
    public slots:
        void handleContextMenuRequestedForTimeline(int tick) {
        QMenu menu(win);
        auto musicTime = timeViewModel->timeline()->create(0, 0, tick);
        menu.addAction(QString("Set time signature at bar %1...").arg(musicTime.measure() + 1), [=] {
            auto timeSignature = promptTimeSignature(win, timeViewModel->timeline()->timeSignatureAt(musicTime.measure()));
            if (!timeSignature.isValid())
                return;
            timeViewModel->timeline()->setTimeSignature(musicTime.measure(), timeSignature);
        });
        auto removeAction = menu.addAction(QString("Remove time signature at bar %1").arg(musicTime.measure() + 1), [=] {
            timeViewModel->timeline()->removeTimeSignature(musicTime.measure());
        });
        removeAction->setEnabled(musicTime.measure() && timeViewModel->timeline()->nearestBarWithTimeSignatureTo(musicTime.measure()) == musicTime.measure());
        menu.exec(QCursor::pos());
    }
    void handleContextMenuRequestedForPositionIndicator() {
        QMenu menu(win);
        menu.addAction("Position indicator action");
        menu.exec(QCursor::pos());
    }
    void handlePositionIndicatorDoubleClicked() {
        QMessageBox::information({}, {}, "Position indicator double clicked");
    }
    void handleNotePressed(int key) {
        qDebug() << "Note on:" << key;
    }
    void handleNoteReleased(int key) {
        qDebug() << "Note off:" << key;
    }
    void handleNoteDoubleClicked(int key) {
        QMessageBox::information({}, {}, QString("Note double clicked %1").arg(key));
    }
    void handleContextMenuRequestedForNote(int key) {
        QMenu menu(win);
        menu.addAction(QString("Note action %1").arg(key));
        menu.exec(QCursor::pos());
    }
    void handleTrackDoubleClicked(int index) {
        QMessageBox::information({}, {}, QString("Track double clicked %1").arg(index));
    }
    void handleContextMenuRequestedForTrack(int index) {
        QMenu menu(win);
        if (index != -1) {
            menu.addAction(QString("Track action %1").arg(index));
        } else {
            menu.addAction("Track list action");
        }
        menu.exec(QCursor::pos());
    }
    void handleContextMenuRequestedForTrackDragging(int index, int target) {
        QMenu menu(win);
        menu.addAction(QString("index: %1, target: %2").arg(index).arg(target));
        menu.addAction("Move here");
        menu.addAction("Copy here");
        menu.addSeparator();
        menu.addAction("Cancel");
        menu.exec(QCursor::pos());
    }
    void handleContextMenuRequested(int tick) {
        QMenu menu(win);
        menu.addAction(QString("Label sequence action %1").arg(timeViewModel->timeline()->create(0, 0, tick).toString()));
        menu.exec(QCursor::pos());
    }
    void handleContextMenuRequestedForLabel(QObject *label) {
        QMenu menu(win);
        menu.addAction(QString("%1 -> 114514").arg(static_cast<LabelViewModel *>(label)->content()), [=] {
            static_cast<LabelViewModel *>(label)->setContent("114514");
        });
        menu.exec(QCursor::pos());
    }
    void handleClipDoubleClicked(QObject *clip) {

    }
};

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

    TimeLayoutViewModel timeLayoutViewModel;
    timeLayoutViewModel.setPositionAlignment(240);

    TimeLayoutViewModel arrangementTimeLayoutViewModel;
    arrangementTimeLayoutViewModel.setPositionAlignment(480);

    SVS::MusicTimeline musicTimeline;

    TimeViewModel timeViewModel;
    timeViewModel.setTimeline(&musicTimeline);
    TimeViewModel arrangementTimeViewModel;
    arrangementTimeViewModel.setTimeline(&musicTimeline);

    PlaybackViewModel playbackViewModel;

    ClavierViewModel clavierViewModel;

    ScrollBehaviorViewModel scrollBehaviorViewModel;

    AnimationViewModel animationViewModel;

    PointSequenceViewModel labelSequenceViewModel;
    for (int i = 0; i < 16; i++) {
        auto label = new LabelViewModel;
        label->setPosition(i * 240);
        label->setContent("test" + QString::number(i));
        labelSequenceViewModel.insertItem(label);
    }
    std::mt19937 generator(114514);

    ListViewModel trackListViewModel;
    QObjectList tracks;
    std::uniform_real_distribution<float> tchd(0.0, 1.0);
    for (int i = 0; i < 8; i++) {
        auto track = new TrackViewModel;
        track->setName("Track " + QString::number(i + 1));
        track->setColor(QColor::fromHsvF(tchd(generator), 0.8, 1.0));
        tracks.append(track);
    }
    trackListViewModel.setItems(tracks);

    TrackListLayoutViewModel trackListLayoutViewModel;

    LabelSequenceBehaviorViewModel labelSequenceBehaviorViewModel;
    LabelSequenceBehaviorViewModel arrangementLabelSequenceBehaviorViewModel;


    std::uniform_int_distribution<int> distribution(-60, 60);

    RangeSequenceViewModel backNoteSequenceViewModel;
    for (int i = 0, k = 48, p = 0; i < 256; i++) {
        auto note = new NoteViewModel;
        note->setPosition(p);
        note->setLength(960);
        p += note->length();
        note->setKey(k = k + (distribution(generator) + (48 - k) * 5) / 20);
        note->setLyric(QString::number(i));
        backNoteSequenceViewModel.insertItem(note);
    }

    PianoRollNoteAreaBehaviorViewModel pianoRollNoteAreaBehaviorViewModel;
    pianoRollNoteAreaBehaviorViewModel.setColor({0x34, 0x98, 0xcb});
    QObject::connect(&trackListViewModel, &ListViewModel::currentIndexChanged, [&](int index) {
        pianoRollNoteAreaBehaviorViewModel.setColor(trackListViewModel.items()[index]->property("color").value<QColor>());
    });
    pianoRollNoteAreaBehaviorViewModel.setLengthHint(960);
    PianoRollNoteAreaBehaviorViewModel backPianoRollNoteAreaBehaviorViewModel;
    backPianoRollNoteAreaBehaviorViewModel.setColor({0x34, 0x98, 0xcb});
    backPianoRollNoteAreaBehaviorViewModel.setMouseBehavior(PianoRollNoteAreaBehaviorViewModel::None);
    backPianoRollNoteAreaBehaviorViewModel.setCompactDisplay(true);

    RangeSequenceViewModel clipSequenceViewModel;
    for (int i = 0; i < 16; i++) {
        auto clip = new ClipViewModel;
        clip->setPosition(i * 480);
        clip->setLength(1440);
        clip->setTrackNumber(i % 8);
        clip->setName(QString::number(i));
        auto clipNoteSequenceViewModel = new RangeSequenceViewModel;
        for (int i = 0, k = 48, p = 0; i < 256; i++) {
            auto note = new NoteViewModel;
            note->setPosition(p);
            note->setLength(960);
            p += note->length();
            note->setKey(k = k + (distribution(generator) + (48 - k) * 5) / 20);
            note->setLyric(QString::number(i));
            clipNoteSequenceViewModel->insertItem(note);
        }
        QObject::connect(clipNoteSequenceViewModel, &RangeSequenceViewModel::itemInserted, [&](QObject *item) {
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

    ClipPaneBehaviorViewModel clipPaneBehaviorViewModel;
    clipPaneBehaviorViewModel.setLengthHint(480);

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
    });
    engine.load(QUrl("qrc:/qt/qml/dev/sjimo/ScopicFlow/Test/main.qml"));

    return a.exec();
}

#include "main.moc"
