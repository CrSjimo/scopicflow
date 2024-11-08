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

    QList<TrackViewModel *> tracks;
    for (int i = 0; i < 4; i++) {
        auto trackViewModel = new TrackViewModel;
        trackViewModel->setName("test");
        tracks.append(trackViewModel);
    }
    trackListViewModel.insertTracks(0, tracks);

    trackList->setTrackListViewModel(&trackListViewModel);

    QMainWindow win;
    win.setCentralWidget(w);

    win.show();

    return a.exec();
}