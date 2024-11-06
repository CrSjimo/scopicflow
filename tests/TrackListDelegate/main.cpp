#include <QGuiApplication>
#include <QQuickView>

#include <ScopicFlow/TimeAlignmentViewModel.h>

using namespace sflow;

int main(int argc, char *argv[]) {
    QGuiApplication a(argc, argv);
    auto format = QSurfaceFormat::defaultFormat();
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    TimeAlignmentViewModel viewModel;

    QQuickView view(QUrl("qrc:/main.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();

    return a.exec();
}