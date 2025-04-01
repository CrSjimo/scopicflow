#ifndef SCOPIC_FLOW_PALETTE_P_H
#define SCOPIC_FLOW_PALETTE_P_H

#include <ScopicFlowStyle/Palette.h>

#include <SVSCraftGui/ColorChange.h>

#include <SVSCraftQuick/AttachedPropertyPropagatorProperties.h>

namespace sflow {

    class PalettePrivate : public SVS::AttachedPropertyPropagatorProperties {
    public:
        static Palette defaultPalette;
        inline explicit PalettePrivate(Palette *palette) : SVS::AttachedPropertyPropagatorProperties(palette, defaultPalette.properties(), {
            {"timelineBackgroundColor", QVariant::fromValue(QColor(0x212124))},
            {"trackListBackgroundColor", QVariant::fromValue(QColor(0x232427))},
            {"labelSequenceBackgroundColor", QVariant::fromValue(QColor(0x212124))},
            {"levelMeterColor", QVariant::fromValue(QColor(0x111112))},
            {"foregroundAvailableColors", QVariant::fromValue(QList<QColor>{0xdadada, 0x252525})},
            {"editAreaPrimaryColor", QVariant::fromValue(QColor(0x353942))},
            {"editAreaSecondaryColor", QVariant::fromValue(QColor(0x495058))},
            {"playheadPrimaryColor", QVariant::fromValue(QColor(0xcc4455))},
            {"playheadSecondaryColor", QVariant::fromValue(QColor::fromRgba(0x7fcc4455))},
            {"cursorIndicatorColor", QVariant::fromValue(QColor(0x7f5566ff))},
            {"muteColor", QVariant::fromValue(QColor(0xe67700))},
            {"soloColor", QVariant::fromValue(QColor(0x2b8a3e))},
            {"recordColor", QVariant::fromValue(QColor(0xc92a2a))},
            {"routeColor", QVariant::fromValue(QColor(0x5f3dc4))},
            {"whiteKeyColor", QVariant::fromValue(QColor(0xf8f9fa))},
            {"blackKeyColor", QVariant::fromValue(QColor(0x212529))},
            {"whiteKeyHoveredColor", QVariant::fromValue(QColor(0x868e96))},
            {"blackKeyHoveredColor", QVariant::fromValue(QColor(0x485057))},
            {"whiteKeyPressedColor", QVariant::fromValue(QColor(0xdee2e6))},
            {"blackKeyPressedColor", QVariant::fromValue(QColor(0x343a40))},
            {"trackListSelectedColorChange", QVariant::fromValue(SVS::ColorChange(QColor::fromRgba(0x275566ff)))},
            {"clipSelectedColorChange", QVariant::fromValue(SVS::ColorChange({}, 0.7, 0xffffff))},
            {"clipThumbnailColorChange", QVariant::fromValue(SVS::ColorChange({}, 0.7, 0xffffff))},
            {"noteSelectedColorChange", QVariant::fromValue(SVS::ColorChange({}, 0.7, 0xffffff))},
        }) {}
    };

}

#endif //SCOPIC_FLOW_PALETTE_P_H
