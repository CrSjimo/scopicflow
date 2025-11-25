#include <QGuiApplication>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QQmlApplicationEngine>

#include <SVSCraftCore/MusicTimeline.h>

#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/TimeViewModel.h>
#include <ScopicFlowCore/ScrollBehaviorViewModel.h>
#include <ScopicFlowCore/PointSequenceViewModel.h>
#include <ScopicFlowCore/LabelViewModel.h>
#include <ScopicFlowCore/LabelSequenceInteractionController.h>

using namespace sflow;

int main(int argc, char **argv) {
    QGuiApplication a(argc, argv);
    auto sf = QSurfaceFormat::defaultFormat();
    sf.setSamples(8);
    QSurfaceFormat::setDefaultFormat(sf);

    QQuickStyle::setStyle("SVSCraft.UIComponents");
    QQuickStyle::setFallbackStyle("Basic");

    TimeLayoutViewModel timeLayoutViewModel(&a);
    timeLayoutViewModel.setPositionAlignment(240);

    TimeLayoutViewModel arrangementTimeLayoutViewModel(&a);
    arrangementTimeLayoutViewModel.setPositionAlignment(480);

    SVS::MusicTimeline musicTimeline(&a);

    TimeViewModel timeViewModel(&a);
    timeViewModel.setTimeline(&musicTimeline);

    PointSequenceViewModel labelSequenceViewModel(&a);

    for (int i = 0; i < 2000; ++i) {
        auto labelViewModel = new LabelViewModel(&a);
        labelViewModel->setPosition(i * 240);
        labelViewModel->setContent(QString::number(i));
        labelSequenceViewModel.insertItem(labelViewModel);
    }

    ScrollBehaviorViewModel scrollBehaviorViewModel(&a);
    scrollBehaviorViewModel.setAutoScroll(true);

    LabelSequenceInteractionController labelSequenceInteractionController(&a);

    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        {"timeLayoutViewModel", QVariant::fromValue(&timeLayoutViewModel)},
        {"timeViewModel", QVariant::fromValue(&timeViewModel)},
        {"labelSequenceViewModel", QVariant::fromValue(&labelSequenceViewModel)},
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
        {"labelSequenceInteractionController", QVariant::fromValue(&labelSequenceInteractionController)}
    });
    engine.load(":/qt/qml/dev/sjimo/ScopicFlow/Test/LabelSequence/main.qml");

    return a.exec();
}
