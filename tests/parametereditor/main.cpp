#include <cmath>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QVariant>

#include <SVSCraftCore/MusicTimeline.h>
#include <SVSCraftFluentSystemIcons/FluentSystemIconsImageProvider.h>

#include <ScopicFlowCore/AnchorParameterViewModel.h>
#include <ScopicFlowCore/ClavierViewModel.h>
#include <ScopicFlowCore/FreeParameterViewModel.h>
#include <ScopicFlowCore/ParameterAnchorViewModel.h>
#include <ScopicFlowCore/ParameterEditorInteractionController.h>
#include <ScopicFlowCore/ParameterRangeSelectionViewModel.h>
#include <ScopicFlowCore/ScrollBehaviorViewModel.h>
#include <ScopicFlowCore/StandardSelectionController.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/TimeViewModel.h>

using namespace sflow;

namespace {

    constexpr int timelineEnd = 6000;

    QList<QVariant> makeFreeValues(const auto &valueAtIndex) {
        QList<QVariant> values;
        values.reserve(timelineEnd / FreeParameterViewModel::step() + 1);
        for (int i = 0; i <= timelineEnd / FreeParameterViewModel::step(); ++i) {
            values.append(valueAtIndex(i));
        }
        return values;
    }

    ParameterAnchorViewModel *addAnchor(AnchorParameterViewModel &curve,
                                         int position,
                                         double value,
                                         ParameterAnchorViewModel::InterpolationMode interpolationMode) {
        auto *anchor = new ParameterAnchorViewModel(&curve);
        anchor->setPosition(position);
        anchor->setValue(value);
        anchor->setInterpolationMode(interpolationMode);
        if (!curve.insertItem(anchor)) {
            delete anchor;
            return nullptr;
        }
        return anchor;
    }

}

int main(int argc, char **argv) {
    QGuiApplication application(argc, argv);

    auto surfaceFormat = QSurfaceFormat::defaultFormat();
    surfaceFormat.setSamples(8);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    QQuickStyle::setStyle("SVSCraft.UIComponents");
    QQuickStyle::setFallbackStyle("Basic");

    ScrollBehaviorViewModel scrollBehaviorViewModel(&application);

    TimeLayoutViewModel timeLayoutViewModel(&application);
    timeLayoutViewModel.setPixelDensity(0.4);
    timeLayoutViewModel.setPositionAlignment(FreeParameterViewModel::step());

    SVS::MusicTimeline musicTimeline(&application);
    TimeViewModel timeViewModel(&application);
    timeViewModel.setTimeline(&musicTimeline);
    timeViewModel.setEnd(timelineEnd);

    ClavierViewModel clavierViewModel(&application);

    FreeParameterViewModel mainFreeParameterViewModel(&application);
    mainFreeParameterViewModel.setValues(0, makeFreeValues([](int i) -> QVariant {
        if ((i >= 45 && i < 85) || (i >= 105 && i < 155)
            || (i >= 405 && i < 455) || (i >= 865 && i < 920)) {
            return {};
        }
        return 0.48 + 0.26 * std::sin(i * 0.031) + 0.08 * std::sin(i * 0.113);
    }));

    FreeParameterViewModel originalParameterViewModel(&application);
    originalParameterViewModel.setValues(0, makeFreeValues([](int i) -> QVariant {
        if ((i >= 55 && i < 64) || (i >= 880 && i < 900)) {
            return {};
        }
        return 0.38 + 0.18 * std::sin(i * 0.017 + 0.7);
    }));

    FreeParameterViewModel transformFreeParameterViewModel(&application);
    transformFreeParameterViewModel.setValues(0, makeFreeValues([](int i) -> QVariant {
        if ((i >= 265 && i < 325) || (i >= 710 && i < 770)) {
            return {};
        }
        return 0.5 + 0.18 * std::sin(i * 0.024 - 0.4);
    }));

    AnchorParameterViewModel mainAnchorParameterViewModel(&application);
    addAnchor(mainAnchorParameterViewModel, 320, 0.25, ParameterAnchorViewModel::Hermite);
    addAnchor(mainAnchorParameterViewModel, 670, 0.78, ParameterAnchorViewModel::Hermite);
    addAnchor(mainAnchorParameterViewModel, 1040, 0.42, ParameterAnchorViewModel::Linear);
    addAnchor(mainAnchorParameterViewModel, 1420, 0.85, ParameterAnchorViewModel::None);
    addAnchor(mainAnchorParameterViewModel, 1810, 0.31, ParameterAnchorViewModel::Hermite);
    addAnchor(mainAnchorParameterViewModel, 2360, 0.66, ParameterAnchorViewModel::Linear);

    AnchorParameterViewModel transformAnchorParameterViewModel(&application);
    addAnchor(transformAnchorParameterViewModel, 520, 0.34, ParameterAnchorViewModel::Hermite);
    addAnchor(transformAnchorParameterViewModel, 920, 0.72, ParameterAnchorViewModel::Hermite);
    addAnchor(transformAnchorParameterViewModel, 1290, 0.46, ParameterAnchorViewModel::None);
    addAnchor(transformAnchorParameterViewModel, 1640, 0.58, ParameterAnchorViewModel::Linear);
    addAnchor(transformAnchorParameterViewModel, 2180, 0.41, ParameterAnchorViewModel::Hermite);

    ParameterRangeSelectionViewModel mainRangeSelectionViewModel(&application);
    ParameterRangeSelectionViewModel originalRangeSelectionViewModel(&application);
    ParameterRangeSelectionViewModel transformRangeSelectionViewModel(&application);

    StandardSelectionController mainAnchorSelectionController(&mainAnchorParameterViewModel, &application);
    StandardSelectionController transformAnchorSelectionController(&transformAnchorParameterViewModel, &application);

    ParameterEditorInteractionController mainInteractionController(&application);
    ParameterEditorInteractionController originalInteractionController(&application);
    ParameterEditorInteractionController transformInteractionController(&application);
    transformInteractionController.setDefaultValueEnabled(true);
    transformInteractionController.setDefaultValue(0.5);
    transformInteractionController.setOriginalAndDefaultCurveDisplayMode(
        ParameterEditorInteractionController::CurveSolid);

    QQmlApplicationEngine engine;
    SVS::FluentSystemIconsImageProvider::addToEngine(&engine);
    engine.setInitialProperties({
        {"scrollBehaviorViewModel", QVariant::fromValue(&scrollBehaviorViewModel)},
        {"timeViewModel", QVariant::fromValue(&timeViewModel)},
        {"timeLayoutViewModel", QVariant::fromValue(&timeLayoutViewModel)},
        {"clavierViewModel", QVariant::fromValue(&clavierViewModel)},
        {"mainFreeParameterViewModel", QVariant::fromValue(&mainFreeParameterViewModel)},
        {"mainAnchorParameterViewModel", QVariant::fromValue(&mainAnchorParameterViewModel)},
        {"originalParameterViewModel", QVariant::fromValue(&originalParameterViewModel)},
        {"transformFreeParameterViewModel", QVariant::fromValue(&transformFreeParameterViewModel)},
        {"transformAnchorParameterViewModel", QVariant::fromValue(&transformAnchorParameterViewModel)},
        {"mainRangeSelectionViewModel", QVariant::fromValue(&mainRangeSelectionViewModel)},
        {"originalRangeSelectionViewModel", QVariant::fromValue(&originalRangeSelectionViewModel)},
        {"transformRangeSelectionViewModel", QVariant::fromValue(&transformRangeSelectionViewModel)},
        {"mainAnchorSelectionController", QVariant::fromValue(&mainAnchorSelectionController)},
        {"transformAnchorSelectionController", QVariant::fromValue(&transformAnchorSelectionController)},
        {"mainInteractionController", QVariant::fromValue(&mainInteractionController)},
        {"originalInteractionController", QVariant::fromValue(&originalInteractionController)},
        {"transformInteractionController", QVariant::fromValue(&transformInteractionController)},
    });
    engine.load("qrc:/qt/qml/dev/sjimo/ScopicFlow/Test/ParameterEditor/main.qml");

    return application.exec();
}
