#include <memory>

#include <QApplication>
#include <QSurfaceFormat>
#include <QMainWindow>
#include <QGridLayout>
#include <QComboBox>
#include <QInputDialog>
#include <QQmlEngine>
#include <QFileDialog>
#include <QSplitter>
#include <QQuickView>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQmlPropertyMap>

#include <SVSCraftCore/musictimeline.h>

#include <ScopicFlow/TimeViewModel.h>
#include <ScopicFlow/TimeLayoutViewModel.h>
#include <ScopicFlow/PlaybackViewModel.h>
#include <ScopicFlow/ScrollBehaviorViewModel.h>
#include <ScopicFlow/AnimationViewModel.h>
#include <ScopicFlow/PaletteViewModel.h>
#include <ScopicFlow/PointSequenceViewModel.h>

using namespace sflow;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QMainWindow win;
    win.resize(1280, 800);
    auto splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);

    TimeViewModel timeViewModel;
    TimeLayoutViewModel timeLayoutViewModel;
    timeLayoutViewModel.setPositionAlignment(240);
    SVS::MusicTimeline musicTimeline;
    timeViewModel.setTimeline(&musicTimeline);

    PlaybackViewModel playbackViewModel;

    ScrollBehaviorViewModel scrollBehaviorViewModel;

    AnimationViewModel animationViewModel;

    PaletteViewModel paletteViewModel;

    PointSequenceViewModel pointSequenceViewModel;
    for (int i = 0; i < 64; i++) {
        auto model = new QQmlPropertyMap(&pointSequenceViewModel);
        model->insert("position", i * 480);
        model->insert("selected", false);
        model->insert("text", i);
        pointSequenceViewModel.insertItem(model);
    }

    auto v1 = new QQuickView;
    v1->engine()->addImportPath("qrc:/");
    v1->setInitialProperties({
        {"timeViewModel", QVariant::fromValue(&timeViewModel)},
        {"timeLayoutViewModel", QVariant::fromValue(&timeLayoutViewModel)},
        {"playbackViewModel", QVariant::fromValue(&playbackViewModel)},
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
        {"animationViewModel", QVariant::fromValue(&animationViewModel)},
        {"paletteViewModel", QVariant::fromValue(&paletteViewModel)},
        {"pointSequenceViewModel", QVariant::fromValue(&pointSequenceViewModel)}
    });
    v1->setSource(QUrl("qrc:/dev/sjimo/ScopicFlow/Test/SequenceSlicer/main.qml"));
    v1->setResizeMode(QQuickView::SizeRootObjectToView);
    splitter->addWidget(QWidget::createWindowContainer(v1));

    win.setCentralWidget(splitter);
    win.show();

    return a.exec();
}

#include "main.moc"
