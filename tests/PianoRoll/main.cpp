#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <ScopicFlow/private/PianoRollBackgroundQuickItem_p.h>
#include <ScopicFlow/private/PianoRollQuickItem_p.h>
#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <SVSCraftCore/musictimeline.h>

using namespace sflow;

int main(int argc, char *argv[]) {
    QGuiApplication a(argc, argv);

    qmlRegisterType<PianoRollQuickItem>("ScopicFlowPrivate", 1, 0, "PianoRoll");
    qmlRegisterType<PianoRollBackgroundQuickItem>("ScopicFlowPrivate", 1, 0, "PianoRollBackground");

    QQuickView *view = new QQuickView;
    view->setSource(QUrl("qrc:/main.qml"));
    view->setResizeMode(QQuickView::SizeRootObjectToView);

    auto pianoRoll = static_cast<PianoRollQuickItem *>(view->rootObject());

    auto timeViewModel = new TimeAlignmentViewModel;
    timeViewModel->setTimeline(new SVS::MusicTimeline);
    pianoRoll->setTimeAlignmentViewModel(timeViewModel);

    auto clavierViewModel  = new ClavierViewModel;
    pianoRoll->setClavierViewModel(clavierViewModel);

    view->show();
    return a.exec();
}
