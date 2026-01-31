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
        Q_PROPERTY(bool clickSelectable READ isClickSelectable WRITE setClickSelectable NOTIFY clickSelectableChanged)
        Q_PROPERTY(Interaction primaryItemInteraction READ primaryItemInteraction WRITE setPrimaryItemInteraction NOTIFY primaryItemInteractionChanged)
        Q_PROPERTY(Interaction secondaryItemInteraction READ secondaryItemInteraction WRITE setSecondaryItemInteraction NOTIFY secondaryItemInteractionChanged)
        Q_PROPERTY(Interaction primarySceneInteraction READ primarySceneInteraction WRITE setPrimarySceneInteraction NOTIFY primarySceneInteractionChanged)
        Q_PROPERTY(Interaction secondarySceneInteraction READ secondarySceneInteraction WRITE setSecondarySceneInteraction NOTIFY secondarySceneInteractionChanged)
        Q_PROPERTY(Interaction primarySelectInteraction READ primarySelectInteraction WRITE setPrimarySelectInteraction NOTIFY primarySelectInteractionChanged)
        Q_PROPERTY(Interaction secondarySelectInteraction READ secondarySelectInteraction WRITE setSecondarySelectInteraction NOTIFY secondarySelectInteractionChanged)
        Q_PROPERTY(ItemAction itemAction READ itemAction WRITE setItemAction NOTIFY itemActionChanged)

    public:
        explicit TrackListInteractionController(QObject *parent = nullptr);
        ~TrackListInteractionController() override;

        bool isClickSelectable() const;
        void setClickSelectable(bool clickSelectable);

        enum Interaction {
            None,
            DragMove,
            DragCopy,
            RubberBandSelect = 0x10000,
        };
        Q_ENUM(Interaction)
        Interaction primaryItemInteraction() const;
        void setPrimaryItemInteraction(Interaction itemInteraction);
        Interaction secondaryItemInteraction() const;
        void setSecondaryItemInteraction(Interaction itemInteraction);
        Interaction primarySceneInteraction() const;
        void setPrimarySceneInteraction(Interaction sceneInteraction);
        Interaction secondarySceneInteraction() const;
        void setSecondarySceneInteraction(Interaction sceneInteraction);
        Interaction primarySelectInteraction() const;
        void setPrimarySelectInteraction(Interaction sceneInteraction);
        Interaction secondarySelectInteraction() const;
        void setSecondarySelectInteraction(Interaction sceneInteraction);

        enum ItemActionFlag {
            EditMute = 0x1,
            EditSolo = 0x2,
            EditRecord = 0x4,
            EditName = 0x8,
            EditGain = 0x10,
            EditPan = 0x20,
            AdjustHeight = 0x40,
            EditMultiChannelOutput = 0x80,
        };
        Q_ENUM(ItemActionFlag)
        Q_DECLARE_FLAGS(ItemAction, ItemActionFlag)

        void setItemAction(ItemAction itemInteraction);
        ItemAction itemAction() const;

        enum ItemHoverTarget {
            ItemBackground,
            ColorIndicator,
            MuteButton,
            SoloButton,
            RecordButton,
            MultiChannelOutputButton,
            NameLabel,
            GainSlider,
            GainLabel,
            PanDial,
            PanLabel,
            LevelMeter,
            HeightAdjustmentHandle,
        };
        Q_ENUM(ItemHoverTarget)

    Q_SIGNALS:
        void clickSelectableChanged();
        void primaryItemInteractionChanged();
        void secondaryItemInteractionChanged();
        void primarySceneInteractionChanged();
        void secondarySceneInteractionChanged();
        void primarySelectInteractionChanged();
        void secondarySelectInteractionChanged();
        void itemActionChanged();

        void rubberBandDraggingStarted(QQuickItem *trackList);
        void rubberBandDraggingCommitted(QQuickItem *trackList);
        void rubberBandDraggingAborted(QQuickItem *trackList);

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

        void multiChannelOutputEditingStarted(QQuickItem *trackList, int index);
        void multiChannelOutputEditingFinished(QQuickItem *trackList, int index);

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
        bool m_clickSelectable;
        Interaction m_primaryItemInteraction;
        Interaction m_secondaryItemInteraction;
        Interaction m_primarySceneInteraction;
        Interaction m_secondarySceneInteraction;
        Interaction m_primarySelectInteraction;
        Interaction m_secondarySelectInteraction;
        ItemAction m_itemAction;
    };
    Q_DECLARE_OPERATORS_FOR_FLAGS(TrackListInteractionController::ItemAction)

}

#endif //SCOPIC_FLOW_TRACKLISTINTERACTIONCONTROLLER_H
