#include <QGuiApplication>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QQmlApplicationEngine>

#include <ScopicFlowCore/TrackViewModel.h>
#include <SVSCraftFluentSystemIcons/FluentSystemIconsImageProvider.h>

using namespace sflow;

int main(int argc, char **argv) {
    QGuiApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QQuickStyle::setStyle("SVSCraft.UIComponents");
    QQuickStyle::setFallbackStyle("Basic");

    TrackViewModel trackViewModel(&a);
    trackViewModel.setName("Test");
    trackViewModel.setColor(Qt::magenta);
    trackViewModel.setLeftLevel(0);
    trackViewModel.setRightClipping(true);
    // trackViewModel.setSelected(true);

    QQmlApplicationEngine engine;
    SVS::FluentSystemIconsImageProvider::addToEngine(&engine);
    engine.setInitialProperties({
        {"trackViewModel", QVariant::fromValue(&trackViewModel)},
    });
    engine.load(":/qt/qml/dev/sjimo/ScopicFlow/Test/TrackList/main.qml");

    return a.exec();
}
