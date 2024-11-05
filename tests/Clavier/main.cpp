#include <QApplication>

#include <QMainWindow>
#include <QGridLayout>
#include <QSurfaceFormat>

#include <SVSCraftCore/musictimeline.h>

#include <ScopicFlow/ClavierWidget.h>
#include <ScopicFlow/TimelineWidget.h>
#include <ScopicFlow/ClavierViewModel.h>
#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <ScopicFlow/WheelModifierViewModel.h>

using namespace sflow;

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

    TimeAlignmentViewModel timeViewModel;
    SVS::MusicTimeline musicTimeline;
    timeViewModel.setTimeline(&musicTimeline);

    timeline->setTimeAlignmentViewModel(&timeViewModel);

    ClavierViewModel clavierViewModel;
    clavier->setClavierViewModel(&clavierViewModel);

    WheelModifierViewModel wheelModifierViewModel;
    clavier->setWheelModifierViewModel(&wheelModifierViewModel);
    timeline->setWheelModifierViewModel(&wheelModifierViewModel);

    mainLayout->addWidget(clavier, 1, 0);
    mainLayout->addWidget(timeline, 0, 1);
    mainLayout->addWidget(new QWidget, 1, 1);

    mainWidget->setLayout(mainLayout);
    win.setCentralWidget(mainWidget);
    win.show();

    return a.exec();
}