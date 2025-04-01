#ifndef SCOPIC_FLOW_PALETTE_H
#define SCOPIC_FLOW_PALETTE_H

#include <QObject>
#include <qqmlintegration.h>
#include <QSharedDataPointer>

#include <SVSCraftQuick/AttachedPropertyPropagator.h>

namespace SVS {
    class ColorChange;
}

namespace sflow {

    class PalettePrivate;

    class Palette : public SVS::AttachedPropertyPropagator {
        Q_OBJECT
        QML_NAMED_ELEMENT(SFPalette)
        QML_ATTACHED(Palette)
        QML_UNCREATABLE("")

        Q_PROPERTY(QColor timelineBackgroundColor READ timelineBackgroundColor WRITE setTimelineBackgroundColor RESET resetTimelineBackgroundColor NOTIFY timelineBackgroundColorChanged)
        Q_PROPERTY(QColor trackListBackgroundColor READ trackListBackgroundColor WRITE setTrackListBackgroundColor RESET resetTrackListBackgroundColor NOTIFY trackListBackgroundColorChanged)
        Q_PROPERTY(QColor labelSequenceBackgroundColor READ labelSequenceBackgroundColor WRITE setLabelSequenceBackgroundColor RESET resetLabelSequenceBackgroundColor NOTIFY labelSequenceBackgroundColorChanged)
        Q_PROPERTY(QColor levelMeterColor READ levelMeterColor WRITE setLevelMeterColor RESET resetLevelMeterColor NOTIFY levelMeterColorChanged)

        Q_PROPERTY(QList<QColor> foregroundAvailableColors READ foregroundAvailableColors WRITE setForegroundAvailableColors RESET resetForegroundAvailableColors NOTIFY foregroundAvailableColorsChanged)

        Q_PROPERTY(QColor editAreaPrimaryColor READ editAreaPrimaryColor WRITE setEditAreaPrimaryColor RESET resetEditAreaPrimaryColor NOTIFY editAreaPrimaryColorChanged)
        Q_PROPERTY(QColor editAreaSecondaryColor READ editAreaSecondaryColor WRITE setEditAreaSecondaryColor RESET resetEditAreaSecondaryColor NOTIFY editAreaSecondaryColorChanged)
        Q_PROPERTY(QColor playheadPrimaryColor READ playheadPrimaryColor WRITE setPlayheadPrimaryColor RESET resetPlayheadPrimaryColor NOTIFY playheadPrimaryColorChanged)
        Q_PROPERTY(QColor playheadSecondaryColor READ playheadSecondaryColor WRITE setPlayheadSecondaryColor RESET resetPlayheadSecondaryColor NOTIFY playheadSecondaryColorChanged)
        Q_PROPERTY(QColor cursorIndicatorColor READ cursorIndicatorColor WRITE setCursorIndicatorColor RESET resetCursorIndicatorColor NOTIFY cursorIndicatorColorChanged)

        Q_PROPERTY(QColor scalePrimaryColor READ scalePrimaryColor WRITE setScalePrimaryColor RESET resetScalePrimaryColor NOTIFY scalePrimaryColorChanged)
        Q_PROPERTY(QColor scaleSecondaryColor READ scaleSecondaryColor WRITE setScaleSecondaryColor RESET resetScaleSecondaryColor NOTIFY scaleSecondaryColorChanged)
        Q_PROPERTY(QColor scaleTertiaryColor READ scaleTertiaryColor WRITE setScaleTertiaryColor RESET resetScaleTertiaryColor NOTIFY scaleTertiaryColorChanged)

        Q_PROPERTY(QColor levelLowColor READ levelLowColor WRITE setLevelLowColor RESET resetLevelLowColor NOTIFY levelLowColorChanged)
        Q_PROPERTY(QColor levelMediumColor READ levelMediumColor WRITE setLevelMediumColor RESET resetLevelMediumColor NOTIFY levelMediumColorChanged)
        Q_PROPERTY(QColor levelHighColor READ levelHighColor WRITE setLevelHighColor RESET resetLevelHighColor NOTIFY levelHighColorChanged)

        Q_PROPERTY(QColor muteColor READ muteColor WRITE setMuteColor RESET resetMuteColor NOTIFY muteColorChanged)
        Q_PROPERTY(QColor soloColor READ soloColor WRITE setSoloColor RESET resetSoloColor NOTIFY soloColorChanged)
        Q_PROPERTY(QColor recordColor READ recordColor WRITE setRecordColor RESET resetRecordColor NOTIFY recordColorChanged)
        Q_PROPERTY(QColor routeColor READ routeColor WRITE setRouteColor RESET resetRouteColor NOTIFY routeColorChanged)

        Q_PROPERTY(QColor whiteKeyColor READ whiteKeyColor WRITE setWhiteKeyColor RESET resetWhiteKeyColor NOTIFY whiteKeyColorChanged)
        Q_PROPERTY(QColor blackKeyColor READ blackKeyColor WRITE setBlackKeyColor RESET resetBlackKeyColor NOTIFY blackKeyColorChanged)
        Q_PROPERTY(QColor whiteKeyHoveredColor READ whiteKeyHoveredColor WRITE setWhiteKeyHoveredColor RESET resetWhiteKeyHoveredColor NOTIFY whiteKeyHoveredColorChanged)
        Q_PROPERTY(QColor blackKeyHoveredColor READ blackKeyHoveredColor WRITE setBlackKeyHoveredColor RESET resetBlackKeyHoveredColor NOTIFY blackKeyHoveredColorChanged)
        Q_PROPERTY(QColor whiteKeyPressedColor READ whiteKeyPressedColor WRITE setWhiteKeyPressedColor RESET resetWhiteKeyPressedColor NOTIFY whiteKeyPressedColorChanged)
        Q_PROPERTY(QColor blackKeyPressedColor READ blackKeyPressedColor WRITE setBlackKeyPressedColor RESET resetBlackKeyPressedColor NOTIFY blackKeyPressedColorChanged)

        Q_PROPERTY(SVS::ColorChange trackListSelectedColorChange READ trackListSelectedColorChange WRITE setTrackListSelectedColorChange RESET resetTrackListSelectedColorChange NOTIFY trackListSelectedColorChangeChanged)
        Q_PROPERTY(SVS::ColorChange clipSelectedColorChange READ clipSelectedColorChange WRITE setClipSelectedColorChange RESET resetClipSelectedColorChange NOTIFY clipSelectedColorChangeChanged)
        Q_PROPERTY(SVS::ColorChange clipThumbnailColorChange READ clipThumbnailColorChange WRITE setClipThumbnailColorChange RESET resetClipThumbnailColorChange NOTIFY clipThumbnailColorChangeChanged)
        Q_PROPERTY(SVS::ColorChange noteSelectedColorChange READ noteSelectedColorChange WRITE setNoteSelectedColorChange RESET resetNoteSelectedColorChange NOTIFY noteSelectedColorChangeChanged)

    public:
        ~Palette() override;

        static Palette *qmlAttachedProperties(QObject *object);
        static Palette *defaultPalette();
        static Palette *get(QObject *object);

        QColor timelineBackgroundColor() const;
        void setTimelineBackgroundColor(const QColor &timelineBackgroundColor);
        void resetTimelineBackgroundColor();

        QColor trackListBackgroundColor() const;
        void setTrackListBackgroundColor(const QColor &trackListBackgroundColor);
        void resetTrackListBackgroundColor();

        QColor labelSequenceBackgroundColor() const;
        void setLabelSequenceBackgroundColor(const QColor &labelSequenceBackgroundColor);
        void resetLabelSequenceBackgroundColor();

        QColor levelMeterColor() const;
        void setLevelMeterColor(const QColor &levelMeterColor);
        void resetLevelMeterColor();

        QList<QColor> foregroundAvailableColors() const;
        void setForegroundAvailableColors(const QList<QColor> &foregroundAvailableColors);
        void resetForegroundAvailableColors();

        QColor editAreaPrimaryColor() const;
        void setEditAreaPrimaryColor(const QColor &editAreaPrimaryColor);
        void resetEditAreaPrimaryColor();

        QColor editAreaSecondaryColor() const;
        void setEditAreaSecondaryColor(const QColor &editAreaSecondaryColor);
        void resetEditAreaSecondaryColor();

        QColor playheadPrimaryColor() const;
        void setPlayheadPrimaryColor(const QColor &playheadPrimaryColor);
        void resetPlayheadPrimaryColor();

        QColor playheadSecondaryColor() const;
        void setPlayheadSecondaryColor(const QColor &playheadSecondaryColor);
        void resetPlayheadSecondaryColor();

        QColor cursorIndicatorColor() const;
        void setCursorIndicatorColor(const QColor &cursorIndicatorColor);
        void resetCursorIndicatorColor();

        QColor scalePrimaryColor() const;
        void setScalePrimaryColor(const QColor &scalePrimaryColor);
        void resetScalePrimaryColor();

        QColor scaleSecondaryColor() const;
        void setScaleSecondaryColor(const QColor &scaleSecondaryColor);
        void resetScaleSecondaryColor();

        QColor scaleTertiaryColor() const;
        void setScaleTertiaryColor(const QColor &scaleTertiaryColor);
        void resetScaleTertiaryColor();

        QColor levelLowColor() const;
        void setLevelLowColor(const QColor &levelLowColor);
        void resetLevelLowColor();

        QColor levelMediumColor() const;
        void setLevelMediumColor(const QColor &levelMediumColor);
        void resetLevelMediumColor();

        QColor levelHighColor() const;
        void setLevelHighColor(const QColor &levelHighColor);
        void resetLevelHighColor();

        QColor muteColor() const;
        void setMuteColor(const QColor &muteColor);
        void resetMuteColor();

        QColor soloColor() const;
        void setSoloColor(const QColor &soloColor);
        void resetSoloColor();

        QColor recordColor() const;
        void setRecordColor(const QColor &recordColor);
        void resetRecordColor();

        QColor routeColor() const;
        void setRouteColor(const QColor &routeColor);
        void resetRouteColor();

        QColor whiteKeyColor() const;
        void setWhiteKeyColor(const QColor &whiteKeyColor);
        void resetWhiteKeyColor();

        QColor blackKeyColor() const;
        void setBlackKeyColor(const QColor &blackKeyColor);
        void resetBlackKeyColor();

        QColor whiteKeyHoveredColor() const;
        void setWhiteKeyHoveredColor(const QColor &whiteKeyHoveredColor);
        void resetWhiteKeyHoveredColor();

        QColor blackKeyHoveredColor() const;
        void setBlackKeyHoveredColor(const QColor &blackKeyHoveredColor);
        void resetBlackKeyHoveredColor();

        QColor whiteKeyPressedColor() const;
        void setWhiteKeyPressedColor(const QColor &whiteKeyPressedColor);
        void resetWhiteKeyPressedColor();

        QColor blackKeyPressedColor() const;
        void setBlackKeyPressedColor(const QColor &blackKeyPressedColor);
        void resetBlackKeyPressedColor();

        SVS::ColorChange trackListSelectedColorChange() const;
        void setTrackListSelectedColorChange(const SVS::ColorChange &trackListSelectedColorChange);
        void resetTrackListSelectedColorChange();

        SVS::ColorChange clipSelectedColorChange() const;
        void setClipSelectedColorChange(const SVS::ColorChange &clipSelectedColorChange);
        void resetClipSelectedColorChange();

        SVS::ColorChange clipThumbnailColorChange() const;
        void setClipThumbnailColorChange(const SVS::ColorChange &clipThumbnailColorChange);
        void resetClipThumbnailColorChange();

        SVS::ColorChange noteSelectedColorChange() const;
        void setNoteSelectedColorChange(const SVS::ColorChange &noteSelectedColorChange);
        void resetNoteSelectedColorChange();

        Q_INVOKABLE QColor suitableForegroundColor(const QColor &backgroundColor) const;

    signals:
        void timelineBackgroundColorChanged();
        void trackListBackgroundColorChanged();
        void labelSequenceBackgroundColorChanged();
        void levelMeterColorChanged();
        void foregroundAvailableColorsChanged();
        void editAreaPrimaryColorChanged();
        void editAreaSecondaryColorChanged();
        void playheadPrimaryColorChanged();
        void playheadSecondaryColorChanged();
        void cursorIndicatorColorChanged();
        void scalePrimaryColorChanged();
        void scaleSecondaryColorChanged();
        void scaleTertiaryColorChanged();
        void levelLowColorChanged();
        void levelMediumColorChanged();
        void levelHighColorChanged();
        void muteColorChanged();
        void soloColorChanged();
        void recordColorChanged();
        void routeColorChanged();
        void whiteKeyColorChanged();
        void blackKeyColorChanged();
        void whiteKeyHoveredColorChanged();
        void blackKeyHoveredColorChanged();
        void whiteKeyPressedColorChanged();
        void blackKeyPressedColorChanged();
        void trackListSelectedColorChangeChanged();
        void clipSelectedColorChangeChanged();
        void clipThumbnailColorChangeChanged();
        void noteSelectedColorChangeChanged();

    protected:
        SVS::AttachedPropertyPropagatorProperties *properties() const override;

    private:
        friend class PalettePrivate;
        QScopedPointer<PalettePrivate> d;
        explicit Palette(QObject *parent = nullptr);
        explicit Palette(QPrivateSignal);
    };

}

#endif //SCOPIC_FLOW_PALETTE_H
