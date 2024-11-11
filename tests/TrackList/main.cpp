#include <QApplication>
#include <QQuickView>
#include <QMainWindow>
#include <QMessageBox>
#include <QMenu>

#include <ScopicFlow/TrackListWidget.h>
#include <ScopicFlow/TrackViewModel.h>
#include <ScopicFlow/TrackListViewModel.h>

using namespace sflow;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    auto format = QSurfaceFormat::defaultFormat();
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    auto trackList = new TrackListWidget;
    TrackListViewModel trackListViewModel;

    QList<TrackViewModel *> tracks;
    for (int i = 0; i < 8; i++) {
        auto trackViewModel = new TrackViewModel;
        trackViewModel->setName("test" + QString::number(i));
        tracks.append(trackViewModel);
    }
    trackListViewModel.insertTracks(0, tracks);

    trackList->setTrackListViewModel(&trackListViewModel);

    QMainWindow win;
    win.setCentralWidget(trackList);

    win.show();

    QObject::connect(trackList, &TrackListWidget::trackDoubleClicked, [=, &win](int index) {
        QMessageBox::information(&win, {}, "track double-clicked " + QString::number(index));
    });
    QObject::connect(trackList, &TrackListWidget::contextMenuRequestedForTrack, [=, &win](int index) {
        QMenu menu(&win);
        menu.addAction("Menu on Track " + QString::number(index));
        menu.exec(QCursor::pos());
    });
    QObject::connect(trackList, &TrackListWidget::contextMenuRequestedForTrackDragging, [=, &win](int index, int target) {
        QMenu menu(&win);
        menu.addAction("Move to here");
        menu.addAction("Copy to here");
        menu.exec(QCursor::pos());
    });

    return a.exec();
}