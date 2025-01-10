#include <memory>

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

#include <SVSCraftCore/musictimeline.h>

#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <ScopicFlow/PlaybackViewModel.h>
#include <ScopicFlow/ClavierViewModel.h>
#include <ScopicFlow/ScrollBehaviorViewModel.h>
#include <ScopicFlow/AnimationViewModel.h>
#include <ScopicFlow/PaletteViewModel.h>
#include <ScopicFlow/LabelSequenceViewModel.h>
#include <ScopicFlow/LabelViewModel.h>
#include <ScopicFlow/ListViewModel.h>
#include <ScopicFlow/TrackListLayoutViewModel.h>
#include <ScopicFlow/TrackViewModel.h>
#include <ScopicFlow/NoteViewModel.h>

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
    TimeAlignmentViewModel *timeViewModel;
public slots:
    void handleTimelineContextMenu(int tick) {
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
        removeAction->setEnabled(musicTime.measure() && timeViewModel->timeline()->nearestTimeSignatureTo(musicTime.measure()) == musicTime.measure());
        menu.exec(QCursor::pos());
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QMainWindow win;
    win.resize(1280, 800);
    auto splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);

    NoteViewModel _;

    TimeAlignmentViewModel arrangementTimeViewModel;
    arrangementTimeViewModel.setPositionAlignment(480);
    TimeAlignmentViewModel timeViewModel;
    timeViewModel.setPositionAlignment(240);
    SVS::MusicTimeline musicTimeline;
    arrangementTimeViewModel.setTimeline(&musicTimeline);
    timeViewModel.setTimeline(&musicTimeline);

    PlaybackViewModel playbackViewModel;

    ClavierViewModel clavierViewModel;

    ScrollBehaviorViewModel scrollBehaviorViewModel;

    AnimationViewModel animationViewModel;

    PaletteViewModel paletteViewModel;

    LabelSequenceViewModel labelSequenceViewModel;
    for (int i = 0; i < 16; i++) {
        auto label = new LabelViewModel;
        label->setPosition(i * 480);
        label->setContent("test" + QString::number(i));
        labelSequenceViewModel.insertLabels({label});
    }

    ListViewModel trackListViewModel;
    QObjectList tracks;
    for (int i = 0; i < 4; i++) {
        auto track = new TrackViewModel;
        track->setName("Track " + QString::number(i + 1));
        tracks.append(track);
    }
    trackListViewModel.setItems(tracks);

    TrackListLayoutViewModel trackListLayoutViewModel;

    auto v1 = new QQuickView;
    v1->engine()->addImportPath("qrc:/");
    v1->setInitialProperties({
        {"timeAlignmentViewModel", QVariant::fromValue(&timeViewModel)},
        {"arrangementTimeAlignmentViewModel", QVariant::fromValue(&arrangementTimeViewModel)},
        {"trackListViewModel", QVariant::fromValue(&trackListViewModel)},
        {"trackListLayoutViewModel", QVariant::fromValue(&trackListLayoutViewModel)},
        {"clavierViewModel", QVariant::fromValue(&clavierViewModel)},
        {"labelSequenceViewModel", QVariant::fromValue(&labelSequenceViewModel)},
        {"playbackViewModel", QVariant::fromValue(&playbackViewModel)},
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
        {"animationViewModel", QVariant::fromValue(&animationViewModel)},
        {"paletteViewModel", QVariant::fromValue(&paletteViewModel)}
    });
    v1->setSource(QUrl("qrc:/dev/sjimo/ScopicFlow/Test/main.qml"));
    v1->setResizeMode(QQuickView::SizeRootObjectToView);
    splitter->addWidget(QWidget::createWindowContainer(v1));

    win.setCentralWidget(splitter);
    win.show();

    auto context = qmlContext(v1->rootObject());

    MySlotHandler o;
    o.win = &win;
    o.timeViewModel = &timeViewModel;

    QObject::connect(context->objectForName("timeline"), SIGNAL(contextMenuRequestedForTimeline(int)), &o, SLOT(handleTimelineContextMenu(int)));
    QObject::connect(context->objectForName("arrangementTimeline"), SIGNAL(contextMenuRequestedForTimeline(int)), &o, SLOT(handleTimelineContextMenu(int)));

    auto mainMenu = new QMenu("Edit");
    mainMenu->addAction("Set Position Alignment...", [&] {
        auto v = QInputDialog::getInt(&win, {}, "Position alignment", timeViewModel.positionAlignment(), 1, 480);
        timeViewModel.setPositionAlignment(v);
    });
    mainMenu->addAction("Set Arrangement Position Alignment...", [&] {
        auto v = QInputDialog::getInt(&win, {}, "Arrangement position alignment", arrangementTimeViewModel.positionAlignment(), 1, 480);
        arrangementTimeViewModel.setPositionAlignment(v);
    });
    mainMenu->addAction("Set Visual Effect Animation Ratio...", [&] {
        auto v = QInputDialog::getDouble(&win, {}, "Animation ratio", animationViewModel.visualEffectAnimationRatio(), 0, 10);
        animationViewModel.setVisualEffectAnimationRatio(v);
    });
    mainMenu->addAction("Set Scroll Animation Ratio...", [&] {
        auto v = QInputDialog::getDouble(&win, {}, "Animation ratio", animationViewModel.scrollAnimationRatio(), 0, 10);
        animationViewModel.setScrollAnimationRatio(v);
    });
    mainMenu->addAction("Set Color Animation Ratio...", [&] {
        auto v = QInputDialog::getDouble(&win, {}, "Animation ratio", animationViewModel.colorAnimationRatio(), 0, 10);
        animationViewModel.setColorAnimationRatio(v);
    });
    mainMenu->addAction("Load Custom Palette...", [&] {
        auto palette = loadCustomPalette(&win);
        if (palette) {
            paletteViewModel.setPalette(palette);
        }
    });
    mainMenu->addAction("Reset to Default Palette", [&] {
        paletteViewModel.setPalette(nullptr);
    });
    QQuickView scrollBehaviorDialog;
    scrollBehaviorDialog.setInitialProperties({
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
    });
    scrollBehaviorDialog.setSource(QUrl("qrc:/dev/sjimo/ScopicFlow/Test/ScrollBehaviorDialog.qml"));
    scrollBehaviorDialog.setTitle("Set Scroll Behavior");
    scrollBehaviorDialog.setResizeMode(QQuickView::SizeRootObjectToView);
    scrollBehaviorDialog.resize(400, 240);
    mainMenu->addAction("Set Scroll Behavior", [&] {
        scrollBehaviorDialog.show();
    });
    win.menuBar()->addMenu(mainMenu);

    return a.exec();
}

#include "main.moc"
