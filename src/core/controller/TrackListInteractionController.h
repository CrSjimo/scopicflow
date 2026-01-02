#ifndef SCOPIC_FLOW_TRACKLISTINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_TRACKLISTINTERACTIONCONTROLLER_H

#include <qqmlintegration.h>

#include <QObject>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT TrackListInteractionController : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(Interaction interaction READ interaction NOTIFY interactionChanged)
        Q_PROPERTY(ItemInteraction itemInteraction READ itemInteraction NOTIFY itemInteractionChanged)
    public:
        explicit TrackListInteractionController(QObject *parent = nullptr);
        ~TrackListInteractionController() override;

        enum InteractionFlag {
            SelectByRubberBand = 0x1,
        };
        Q_ENUM(InteractionFlag)
        Q_DECLARE_FLAGS(Interaction, InteractionFlag)

        void setInteraction(Interaction interaction);
        Interaction interaction() const;

        enum ItemInteractionFlag {
            DragMove = 0x1,
            Select = 0x2,
            EditMute = 0x4,
            EditSolo = 0x8,
            EditRecord = 0x10,
            EditName = 0x20,
            EditGain = 0x40,
            EditPan = 0x80,
            AdjustHeight = 0x100,
        };
        Q_ENUM(ItemInteractionFlag)
        Q_DECLARE_FLAGS(ItemInteraction, ItemInteractionFlag)

        void setItemInteraction(ItemInteraction itemInteraction);
        ItemInteraction itemInteraction() const;

        enum ItemHoverTarget {
            ItemBackground,
            ColorIndicator,
            MuteButton,
            SoloButton,
            RecordButton,
            NameLabel,
            GainSlider,
            GainLabel,
            PanDial,
            PanLabel,
            LevelMeter,
        };
        Q_ENUM(ItemHoverTarget)

    Q_SIGNALS:
        void interactionChanged();
        void itemInteractionChanged();

        void rubberBandDraggingStarted(QQuickItem *trackList);
        void rubberBandDraggingFinished(QQuickItem *trackList);

        void dragMovingStarted(QQuickItem *trackList, int index);
        void dragMovingCommitted(QQuickItem *trackList, int index);
        void dragMovingAborted(QQuickItem *trackList, int index);

        void muteEditingStarted(QQuickItem *trackList, int index);
        void muteEditingFinished(QQuickItem *trackList, int index);

        void soloEditingStarted(QQuickItem *trackList, int index);
        void soloEditingFinished(QQuickItem *trackList, int index);

        void recordEditingStarted(QQuickItem *trackList, int index);
        void recordEditingFinished(QQuickItem *trackList, int index);

        void nameEditingStarted(QQuickItem *trackList, int index);
        void nameEditingCommitted(QQuickItem *trackList, int index);
        void nameEditingAborted(QQuickItem *trackList, int index);

        void gainEditingStarted(QQuickItem *trackList, int index);
        void gainEditingCommitted(QQuickItem *trackList, int index);
        void gainEditingAborted(QQuickItem *trackList, int index);

        void panEditingStarted(QQuickItem *trackList, int index);
        void panEditingCommitted(QQuickItem *trackList, int index);
        void panEditingAborted(QQuickItem *trackList, int index);

        void heightAdjustingStarted(QQuickItem *trackList, int index);
        void heightAdjustingFinished(QQuickItem *trackList, int index);

        void levelMeterClicked(QQuickItem *trackList, int index);

        void hoverEntered(QQuickItem *trackList);
        void hoverExited(QQuickItem *trackList);
        void itemHoverEntered(QQuickItem *trackList, int index, ItemHoverTarget target);
        void itemHoverExited(QQuickItem *trackList, int index, ItemHoverTarget target);

        void itemColorIndicatorClicked(QQuickItem *trackList, int index);
        void itemLevelMeterClicked(QQuickItem *trackList, int index);

        void doubleClicked(QQuickItem *trackList);
        void itemDoubleClicked(QQuickItem *trackList, int index);

        void contextMenuRequested(QQuickItem *trackList);
        void itemContextMenuRequested(QQuickItem *trackList, int index);

        void copyItemsRequested(QQuickItem *trackList, int targetIndex);

    private:
        Interaction m_interaction;
        ItemInteraction m_itemInteraction;
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(TrackListInteractionController::Interaction)
    Q_DECLARE_OPERATORS_FOR_FLAGS(TrackListInteractionController::ItemInteraction)

}

#endif //SCOPIC_FLOW_TRACKLISTINTERACTIONCONTROLLER_H
