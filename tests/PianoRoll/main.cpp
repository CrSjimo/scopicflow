#include <QApplication>
#include <QSurfaceFormat>
#include <QMainWindow>
#include <QGridLayout>
#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QMenu>

#include <SVSCraftCore/musictimeline.h>

#include <ScopicFlow/TimelineWidget.h>
#include <ScopicFlow/PianoRollWidget.h>
#include <ScopicFlow/ClavierWidget.h>
#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <ScopicFlow/PlaybackViewModel.h>
#include <ScopicFlow/ClavierViewModel.h>
#include <ScopicFlow/ScrollBehaviorViewModel.h>

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

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QMainWindow win;
    auto mainWidget = new QWidget;

    auto mainLayout = new QGridLayout;
    mainLayout->setSpacing(0);

    auto clavier = new ClavierWidget;
    auto timeline = new TimelineWidget;
    auto pianoRoll = new PianoRollWidget;

    TimeAlignmentViewModel timeViewModel;
    timeViewModel.setPositionAlignment(240);
    SVS::MusicTimeline musicTimeline;
    timeViewModel.setTimeline(&musicTimeline);

    timeline->setTimeAlignmentViewModel(&timeViewModel);
    pianoRoll->setTimeAlignmentViewModel(&timeViewModel);

    PlaybackViewModel playbackViewModel;
    timeline->setPlaybackViewModel(&playbackViewModel);
    pianoRoll->setPlaybackViewModel(&playbackViewModel);

    ClavierViewModel clavierViewModel;
    clavier->setClavierViewModel(&clavierViewModel);
    pianoRoll->setClavierViewModel(&clavierViewModel);

    ScrollBehaviorViewModel scrollBehaviorViewModel;
    scrollBehaviorViewModel.setUsePageModifierAsAlternateAxisZoom(true);
    timeline->setScrollBehaviorViewModel(&scrollBehaviorViewModel);
    clavier->setScrollBehaviorViewModel(&scrollBehaviorViewModel);
    pianoRoll->setScrollBehaviorViewModel(&scrollBehaviorViewModel);

    mainLayout->addWidget(clavier, 1, 0);
    mainLayout->addWidget(timeline, 0, 1);
    mainLayout->addWidget(pianoRoll, 1, 1);

    mainWidget->setLayout(mainLayout);
    win.setCentralWidget(mainWidget);
    win.show();

    QObject::connect(timeline, &TimelineWidget::contextMenuRequestedForTimeline, [=, &win, &timeViewModel](int tick) {
        QMenu menu(&win);
        auto musicTime = timeViewModel.timeline()->create(0, 0, tick);
        menu.addAction(QString("Set time signature at bar %1...").arg(musicTime.measure() + 1), [=, &win, &timeViewModel] {
            auto timeSignature = promptTimeSignature(&win, timeViewModel.timeline()->timeSignatureAt(musicTime.measure()));
            if (!timeSignature.isValid())
                return;
            timeViewModel.timeline()->setTimeSignature(musicTime.measure(), timeSignature);
        });
        auto removeAction = menu.addAction(QString("Remove time signature at bar %1").arg(musicTime.measure() + 1), [=, &timeViewModel] {
            timeViewModel.timeline()->removeTimeSignature(musicTime.measure());
        });
        removeAction->setEnabled(musicTime.measure() && timeViewModel.timeline()->nearestTimeSignatureTo(musicTime.measure()) == musicTime.measure());
        menu.exec(QCursor::pos());
    });

    QObject::connect(clavier, &ClavierWidget::noteOn, [](int key) {
        qDebug() << "note on" << key;
    });

    QObject::connect(clavier, &ClavierWidget::noteOff, [](int key) {
        qDebug() << "note off" << key;
    });
    QObject::connect(clavier, &ClavierWidget::noteDoubleClicked, [](int key) {
        qDebug() << "note double clicked" << key;
    });
    QObject::connect(clavier, &ClavierWidget::contextMenuRequestedForNote, [=, &win, &clavierViewModel](int key) {
        QMenu menu(&win);
        menu.addAction(QString("Note %1").arg(key));
        menu.exec(QCursor::pos());
    });

    return a.exec();
}
