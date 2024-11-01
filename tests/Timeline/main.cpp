#include <limits>

#include <QApplication>
#include <QFormLayout>
#include <QLabel>
#include <QMainWindow>
#include <QQuickView>
#include <QVBoxLayout>
#include <QQuickWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include <SVSCraftCore/musictimeline.h>

#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <ScopicFlow/TimelineWidget.h>

using namespace sflow;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QMainWindow win;
    auto mainWidget = new QWidget;
    auto mainLayout = new QVBoxLayout;
    auto timelineWidget = new TimelineWidget;
    mainLayout->addWidget(timelineWidget);

    auto timeViewModel = new TimeAlignmentViewModel;
    timeViewModel->setTimeline(new SVS::MusicTimeline);
    timeViewModel->setPositionAlignment(240);
    timeViewModel->timeline()->setTimeSignature(3, {6, 8});

    timelineWidget->setTimeAlignmentViewModel(timeViewModel);

    mainWidget->setLayout(mainLayout);
    win.setCentralWidget(mainWidget);

    auto formLayout = new QFormLayout;

    auto startSpinBox = new QSpinBox;
    startSpinBox->setMaximum(std::numeric_limits<int>::max());
    formLayout->addRow("Start", startSpinBox);

    auto pixelDensitySpinBox = new QDoubleSpinBox;
    pixelDensitySpinBox->setRange(0.0005, 1);
    pixelDensitySpinBox->setDecimals(4);
    pixelDensitySpinBox->setValue(0.2);
    formLayout->addRow("Pixel density", pixelDensitySpinBox);

    auto primaryPositionSpinBox = new QSpinBox;
    primaryPositionSpinBox->setMaximum(std::numeric_limits<int>::max());
    formLayout->addRow("Primary position", primaryPositionSpinBox);

    auto secondaryPositionSpinBox = new QSpinBox;
    secondaryPositionSpinBox->setMaximum(std::numeric_limits<int>::max());
    formLayout->addRow("Second position", secondaryPositionSpinBox);

    auto positionAlignmentSpinBox = new QSpinBox;
    positionAlignmentSpinBox->setRange(1, std::numeric_limits<int>::max());
    positionAlignmentSpinBox->setValue(timeViewModel->positionAlignment());
    formLayout->addRow("Position alignment", positionAlignmentSpinBox);

    mainLayout->addLayout(formLayout);

    QObject::connect(startSpinBox, &QSpinBox::valueChanged, timeViewModel, &TimeViewModel::setStart);
    QObject::connect(pixelDensitySpinBox, &QDoubleSpinBox::valueChanged, timeViewModel, &TimeViewModel::setPixelDensity);
    QObject::connect(primaryPositionSpinBox, &QSpinBox::valueChanged, timeViewModel, &TimeViewModel::setPrimaryPosition);
    QObject::connect(timeViewModel, &TimeViewModel::primaryPositionChanged, primaryPositionSpinBox, &QSpinBox::setValue);
    QObject::connect(secondaryPositionSpinBox, &QSpinBox::valueChanged, timeViewModel, &TimeViewModel::setSecondaryPosition);
    QObject::connect(timeViewModel, &TimeViewModel::secondaryPositionChanged, secondaryPositionSpinBox, &QSpinBox::setValue);
    QObject::connect(positionAlignmentSpinBox, &QSpinBox::valueChanged, timeViewModel, &TimeAlignmentViewModel::setPositionAlignment);

    win.show();

    return a.exec();
}
