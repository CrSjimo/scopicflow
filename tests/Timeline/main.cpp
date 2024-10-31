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

#include <ScopicFlow/Timeline.h>
#include <ScopicFlow/TimeViewModel.h>

using namespace sflow;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    qmlRegisterType<Timeline>("ScopicFlow", 1, 0, "Timeline");
    QMainWindow win;
    auto mainWidget = new QWidget;
    auto mainLayout = new QVBoxLayout;
    auto quickView = new QQuickView(QUrl("qrc:/main.qml"));
    auto quickViewContainer = QWidget::createWindowContainer(quickView);
    quickView->setResizeMode(QQuickView::SizeRootObjectToView);
    quickViewContainer->setMinimumHeight(24);
    mainLayout->addWidget(quickViewContainer);

    auto timelineItem = static_cast<Timeline *>(quickView->rootObject());

    auto timeViewModel = new TimeViewModel;
    timeViewModel->setTimeline(new SVS::MusicTimeline);
    timelineItem->setTimeViewModel(timeViewModel);

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

    auto primayPositionSpinBox = new QSpinBox;
    primayPositionSpinBox->setMaximum(std::numeric_limits<int>::max());
    formLayout->addRow("Primary position", primayPositionSpinBox);

    mainLayout->addLayout(formLayout);

    QObject::connect(startSpinBox, &QSpinBox::valueChanged, timeViewModel, &TimeViewModel::setStart);
    QObject::connect(pixelDensitySpinBox, &QDoubleSpinBox::valueChanged, timeViewModel, &TimeViewModel::setPixelDensity);
    QObject::connect(primayPositionSpinBox, &QSpinBox::valueChanged, timeViewModel, &TimeViewModel::setPrimaryPosition);
    QObject::connect(timeViewModel, &TimeViewModel::primaryPositionChanged, primayPositionSpinBox, &QSpinBox::setValue);

    win.show();

    return a.exec();
}
