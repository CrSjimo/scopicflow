#include "Palette.h"
#include "Palette_p.h"

#include <cmath>
#include <algorithm>

#include <QQmlEngine>

#define IMPLEMENTATION(property, getFunc, setFunc, resetFunc) SVS_ATTACHED_PROPERTY_PROPAGATOR_IMPLEMENTATION(Palette, property, getFunc, setFunc, resetFunc)

namespace sflow {
    Palette PalettePrivate::defaultPalette = Palette(Palette::QPrivateSignal{});

    Palette *Palette::qmlAttachedProperties(QObject *object) {
        return new Palette(object);
    }

    SVS::AttachedPropertyPropagatorProperties *Palette::properties() const {
        return d.data();
    }
    Palette::Palette(QObject *parent) : SVS::AttachedPropertyPropagator(parent), d(new PalettePrivate(this)) {
        initialize();
        Palette::properties()->inheritAll();
    }
    Palette::Palette(QPrivateSignal) : d(new PalettePrivate(this)) {
        initialize();
    }
    Palette::~Palette() = default;
    Palette *Palette::defaultPalette() {
        return &PalettePrivate::defaultPalette;
    }
    Palette *Palette::get(QObject *object) {
        return qobject_cast<Palette *>(qmlAttachedPropertiesObject<Palette>(object));
    }

    IMPLEMENTATION(loopColor, loopColor, setLoopColor, resetLoopColor)

    IMPLEMENTATION(levelMeterColor, levelMeterColor, setLevelMeterColor, resetLevelMeterColor)

    IMPLEMENTATION(editAreaPrimaryColor, editAreaPrimaryColor, setEditAreaPrimaryColor, resetEditAreaPrimaryColor)
    IMPLEMENTATION(editAreaSecondaryColor, editAreaSecondaryColor, setEditAreaSecondaryColor, resetEditAreaSecondaryColor)
    IMPLEMENTATION(editAreaPrimaryHighlightColor, editAreaPrimaryHighlightColor, setEditAreaPrimaryHighlightColor, resetEditAreaPrimaryHighlightColor)
    IMPLEMENTATION(editAreaSecondaryHighlightColor, editAreaSecondaryHighlightColor, setEditAreaSecondaryHighlightColor, resetEditAreaSecondaryHighlightColor)
    IMPLEMENTATION(playheadPrimaryColor, playheadPrimaryColor, setPlayheadPrimaryColor, resetPlayheadPrimaryColor)
    IMPLEMENTATION(playheadSecondaryColor, playheadSecondaryColor, setPlayheadSecondaryColor, resetPlayheadSecondaryColor)
    IMPLEMENTATION(cursorIndicatorColor, cursorIndicatorColor, setCursorIndicatorColor, resetCursorIndicatorColor)
    IMPLEMENTATION(scissorIndicatorColor, scissorIndicatorColor, setScissorIndicatorColor, resetScissorIndicatorColor)

    IMPLEMENTATION(scalePrimaryColor, scalePrimaryColor, setScalePrimaryColor, resetScalePrimaryColor)
    IMPLEMENTATION(scaleSecondaryColor, scaleSecondaryColor, setScaleSecondaryColor, resetScaleSecondaryColor)
    IMPLEMENTATION(scaleTertiaryColor, scaleTertiaryColor, setScaleTertiaryColor, resetScaleTertiaryColor)

    IMPLEMENTATION(levelLowColor, levelLowColor, setLevelLowColor, resetLevelLowColor)
    IMPLEMENTATION(levelMediumColor, levelMediumColor, setLevelMediumColor, resetLevelMediumColor)
    IMPLEMENTATION(levelHighColor, levelHighColor, setLevelHighColor, resetLevelHighColor)

    IMPLEMENTATION(muteColor, muteColor, setMuteColor, resetMuteColor)
    IMPLEMENTATION(soloColor, soloColor, setSoloColor, resetSoloColor)
    IMPLEMENTATION(recordColor, recordColor, setRecordColor, resetRecordColor)
    IMPLEMENTATION(routeColor, routeColor, setRouteColor, resetRouteColor)
    IMPLEMENTATION(clipMuteColor, clipMuteColor, setClipMuteColor, resetClipMuteColor)

    IMPLEMENTATION(whiteKeyColor, whiteKeyColor, setWhiteKeyColor, resetWhiteKeyColor)
    IMPLEMENTATION(blackKeyColor, blackKeyColor, setBlackKeyColor, resetBlackKeyColor)
    IMPLEMENTATION(whiteKeyTextColor, whiteKeyTextColor, setWhiteKeyTextColor, resetWhiteKeyTextColor)
    IMPLEMENTATION(blackKeyTextColor, blackKeyTextColor, setBlackKeyTextColor, resetBlackKeyTextColor)

    IMPLEMENTATION(itemSelectedColorChange, itemSelectedColorChange, setItemSelectedColorChange, resetItemSelectedColorChange)
    IMPLEMENTATION(clipSelectedColorChange, clipSelectedColorChange, setClipSelectedColorChange, resetClipSelectedColorChange)
    IMPLEMENTATION(clipThumbnailColorChange, clipThumbnailColorChange, setClipThumbnailColorChange, resetClipThumbnailColorChange)
    IMPLEMENTATION(noteSelectedColorChange, noteSelectedColorChange, setNoteSelectedColorChange, resetNoteSelectedColorChange)

}

#include "moc_Palette.cpp"
