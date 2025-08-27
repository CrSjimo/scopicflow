#ifndef SCOPIC_FLOW_PALETTE_P_H
#define SCOPIC_FLOW_PALETTE_P_H

#include <ScopicFlowCore/Palette.h>

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

            {"scalePrimaryColor", QVariant::fromValue(QColor(Qt::black))},
            {"scaleSecondaryColor", QVariant::fromValue(QColor::fromRgba(0xbf000000))},
            {"scaleTertiaryColor", QVariant::fromValue(QColor::fromRgba(0x7f000000))},

            {"levelLowColor", QVariant::fromValue(QColor(0x00c853))},
            {"levelMediumColor", QVariant::fromValue(QColor(0xffab00))},
            {"levelHighColor", QVariant::fromValue(QColor(0xff2c00))},

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

            {"trackListSelectedColorChange", QVariant::fromValue(SVS::ColorChange{SVS::TopBlendColorFilter{QColor::fromRgba(0x275566ff)}})},
            {"clipSelectedColorChange", QVariant::fromValue(SVS::ColorChange{SVS::LighterColorChange{125}})},
            {"clipThumbnailColorChange", QVariant::fromValue(SVS::ColorChange{SVS::LighterColorChange{120}})},
            {"noteSelectedColorChange", QVariant::fromValue(SVS::ColorChange{SVS::LighterColorChange{125}})},
            {"noteBorderColorChange", QVariant::fromValue(SVS::ColorChange{SVS::LighterColorChange{90}})},
        }) {}
    };

}

#endif //SCOPIC_FLOW_PALETTE_P_H
