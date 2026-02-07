#include <QDebug>
#include <QGuiApplication>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QQmlApplicationEngine>
#include <QQuickItem>

#include <SVSCraftFluentSystemIcons/FluentSystemIconsImageProvider.h>

#include <ScopicFlowCore/ClavierViewModel.h>
#include <ScopicFlowCore/ScrollBehaviorViewModel.h>
#include <ScopicFlowCore/ClavierInteractionController.h>


using namespace sflow;

int main(int argc, char **argv) {
    QGuiApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QQuickStyle::setStyle("SVSCraft.UIComponents");
    QQuickStyle::setFallbackStyle("Basic");

    ClavierViewModel clavierViewModel(&a);
    ScrollBehaviorViewModel scrollBehaviorViewModel(&a);
    ClavierInteractionController clavierInteractionController(&a);

    clavierInteractionController.setDisplayStyle(ClavierInteractionController::Realistic);

    QQmlApplicationEngine engine;
    SVS::FluentSystemIconsImageProvider::addToEngine(&engine);
    engine.setInitialProperties({
        {"clavierViewModel", QVariant::fromValue(&clavierViewModel)},
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
        {"clavierInteractionController", QVariant::fromValue(&clavierInteractionController)}
    });
    engine.load(":/qt/qml/dev/sjimo/ScopicFlow/Test/Clavier/main.qml");

    return a.exec();
}
