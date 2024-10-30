#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QQuickView>
#include <QVBoxLayout>
#include <QQuickWidget>

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
    mainLayout->addWidget(quickViewContainer);

    auto timelineItem = static_cast<Timeline *>(quickView->rootObject());

    auto timeViewModel = new TimeViewModel;
    timeViewModel->setTimeline(new SVS::MusicTimeline);
    timelineItem->setTimeViewModel(timeViewModel);

    mainWidget->setLayout(mainLayout);
    win.setCentralWidget(mainWidget);

    win.show();

    return a.exec();
}
