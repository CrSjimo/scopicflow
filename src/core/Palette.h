#ifndef SCOPIC_FLOW_PALETTE_H
#define SCOPIC_FLOW_PALETTE_H

#include <QObject>
#include <qqmlintegration.h>
#include <QSharedDataPointer>

#include <SVSCraftQuick/AttachedPropertyPropagator.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace SVS {
    class ColorChange;
}

namespace sflow {

    class PalettePrivate;

    class SCOPIC_FLOW_CORE_EXPORT Palette : public SVS::AttachedPropertyPropagator {
        Q_OBJECT
        QML_NAMED_ELEMENT(SFPalette)
        QML_ATTACHED(Palette)
        QML_UNCREATABLE("")

        Q_PROPERTY(QColor loopColor READ loopColor WRITE setLoopColor RESET resetLoopColor NOTIFY loopColorChanged)

        Q_PROPERTY(QColor levelMeterColor READ levelMeterColor WRITE setLevelMeterColor RESET resetLevelMeterColor NOTIFY levelMeterColorChanged)

        Q_PROPERTY(QColor editAreaPrimaryColor READ editAreaPrimaryColor WRITE setEditAreaPrimaryColor RESET resetEditAreaPrimaryColor NOTIFY editAreaPrimaryColorChanged)
        Q_PROPERTY(QColor editAreaSecondaryColor READ editAreaSecondaryColor WRITE setEditAreaSecondaryColor RESET resetEditAreaSecondaryColor NOTIFY editAreaSecondaryColorChanged)
        Q_PROPERTY(QColor playheadPrimaryColor READ playheadPrimaryColor WRITE setPlayheadPrimaryColor RESET resetPlayheadPrimaryColor NOTIFY playheadPrimaryColorChanged)
        Q_PROPERTY(QColor playheadSecondaryColor READ playheadSecondaryColor WRITE setPlayheadSecondaryColor RESET resetPlayheadSecondaryColor NOTIFY playheadSecondaryColorChanged)
        Q_PROPERTY(QColor cursorIndicatorColor READ cursorIndicatorColor WRITE setCursorIndicatorColor RESET resetCursorIndicatorColor NOTIFY cursorIndicatorColorChanged)
        Q_PROPERTY(QColor scissorIndicatorColor READ scissorIndicatorColor WRITE setScissorIndicatorColor RESET resetScissorIndicatorColor NOTIFY scissorIndicatorColorChanged)

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
        Q_PROPERTY(QColor clipMuteColor READ clipMuteColor WRITE setClipMuteColor RESET resetClipMuteColor NOTIFY clipMuteColorChanged)

        Q_PROPERTY(QColor whiteKeyColor READ whiteKeyColor WRITE setWhiteKeyColor RESET resetWhiteKeyColor NOTIFY whiteKeyColorChanged)
        Q_PROPERTY(QColor blackKeyColor READ blackKeyColor WRITE setBlackKeyColor RESET resetBlackKeyColor NOTIFY blackKeyColorChanged)
        Q_PROPERTY(QColor whiteKeyTextColor READ whiteKeyTextColor WRITE setWhiteKeyTextColor RESET resetWhiteKeyTextColor NOTIFY whiteKeyTextColorChanged)
        Q_PROPERTY(QColor blackKeyTextColor READ blackKeyTextColor WRITE setBlackKeyTextColor RESET resetBlackKeyTextColor NOTIFY blackKeyTextColorChanged)

        Q_PROPERTY(SVS::ColorChange itemSelectedColorChange READ itemSelectedColorChange WRITE setItemSelectedColorChange RESET resetItemSelectedColorChange NOTIFY itemSelectedColorChangeChanged)
        Q_PROPERTY(SVS::ColorChange clipSelectedColorChange READ clipSelectedColorChange WRITE setClipSelectedColorChange RESET resetClipSelectedColorChange NOTIFY clipSelectedColorChangeChanged)
        Q_PROPERTY(SVS::ColorChange clipThumbnailColorChange READ clipThumbnailColorChange WRITE setClipThumbnailColorChange RESET resetClipThumbnailColorChange NOTIFY clipThumbnailColorChangeChanged)
        Q_PROPERTY(SVS::ColorChange noteSelectedColorChange READ noteSelectedColorChange WRITE setNoteSelectedColorChange RESET resetNoteSelectedColorChange NOTIFY noteSelectedColorChangeChanged)

    public:
        ~Palette() override;

        static Palette *qmlAttachedProperties(QObject *object);
        static Palette *defaultPalette();
        static Palette *get(QObject *object);

        QColor loopColor() const;
        void setLoopColor(const QColor &loopColor);
        void resetLoopColor();

        QColor levelMeterColor() const;
        void setLevelMeterColor(const QColor &levelMeterColor);
        void resetLevelMeterColor();

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

        QColor scissorIndicatorColor() const;
        void setScissorIndicatorColor(const QColor &scissorIndicatorColor);
        void resetScissorIndicatorColor();

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

        QColor clipMuteColor() const;
        void setClipMuteColor(const QColor &clipMuteColor);
        void resetClipMuteColor();

        QColor whiteKeyColor() const;
        void setWhiteKeyColor(const QColor &whiteKeyColor);
        void resetWhiteKeyColor();

        QColor blackKeyColor() const;
        void setBlackKeyColor(const QColor &blackKeyColor);
        void resetBlackKeyColor();

        QColor whiteKeyTextColor() const;
        void setWhiteKeyTextColor(const QColor &whiteKeyTextColor);
        void resetWhiteKeyTextColor();

        QColor blackKeyTextColor() const;
        void setBlackKeyTextColor(const QColor &blackKeyTextColor);
        void resetBlackKeyTextColor();

        SVS::ColorChange itemSelectedColorChange() const;
        void setItemSelectedColorChange(const SVS::ColorChange &itemSelectedColorChange);
        void resetItemSelectedColorChange();

        SVS::ColorChange clipSelectedColorChange() const;
        void setClipSelectedColorChange(const SVS::ColorChange &clipSelectedColorChange);
        void resetClipSelectedColorChange();

        SVS::ColorChange clipThumbnailColorChange() const;
        void setClipThumbnailColorChange(const SVS::ColorChange &clipThumbnailColorChange);
        void resetClipThumbnailColorChange();

        SVS::ColorChange noteSelectedColorChange() const;
        void setNoteSelectedColorChange(const SVS::ColorChange &noteSelectedColorChange);
        void resetNoteSelectedColorChange();

    signals:
        void loopColorChanged();
        void levelMeterColorChanged();
        void editAreaPrimaryColorChanged();
        void editAreaSecondaryColorChanged();
        void playheadPrimaryColorChanged();
        void playheadSecondaryColorChanged();
        void cursorIndicatorColorChanged();
        void scissorIndicatorColorChanged();
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
        void clipMuteColorChanged();
        void whiteKeyColorChanged();
        void blackKeyColorChanged();
        void whiteKeyTextColorChanged();
        void blackKeyTextColorChanged();
        void itemSelectedColorChangeChanged();
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
