#include <QApplication>
#include <QQuickView>
#include <QMainWindow>

#include <ScopicFlow/private/QuickWrapperHelper_p.h>
#include <ScopicFlow/private/TrackListQuickItem_p.h>
#include <ScopicFlow/TrackViewModel.h>
#include <ScopicFlow/TrackListViewModel.h>

using namespace sflow;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    auto format = QSurfaceFormat::defaultFormat();
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    qmlRegisterType<TrackListQuickItem>("ScopicFlowPrivate", 1, 0, "TrackList");
    auto [w, item] = QuickWrapperHelper::wrap("TrackList");

    auto trackList = static_cast<TrackListQuickItem *>(item);
    TrackListViewModel trackListViewModel;

    TrackViewModel trackViewModel1;
    trackViewModel1.setName("test1");
    TrackViewModel trackViewModel2;
    trackViewModel2.setName("test2");
    trackListViewModel.insertTracks(0, {&trackViewModel1, &trackViewModel2});

    trackList->setTrackListViewModel(&trackListViewModel);

    QMainWindow win;
    win.setCentralWidget(w);

    win.show();

    return a.exec();
}