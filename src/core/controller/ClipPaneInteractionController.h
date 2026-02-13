#ifndef SCOPIC_FLOW_CLIPPANEINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_CLIPPANEINTERACTIONCONTROLLER_H

#include <qqmlintegration.h>

#include <QObject>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class ClipViewModel;
    class RangeSequenceViewModel;

    class SCOPIC_FLOW_CORE_EXPORT ClipPaneInteractionController : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(bool clickSelectable READ isClickSelectable WRITE setClickSelectable NOTIFY clickSelectableChanged)
        Q_PROPERTY(Interaction primaryItemInteraction READ primaryItemInteraction WRITE setPrimaryItemInteraction NOTIFY primaryItemInteractionChanged)
        Q_PROPERTY(Interaction secondaryItemInteraction READ secondaryItemInteraction WRITE setSecondaryItemInteraction NOTIFY secondaryItemInteractionChanged)
        Q_PROPERTY(Interaction primarySceneInteraction READ primarySceneInteraction WRITE setPrimarySceneInteraction NOTIFY primarySceneInteractionChanged)
        Q_PROPERTY(Interaction secondarySceneInteraction READ secondarySceneInteraction WRITE setSecondarySceneInteraction NOTIFY secondarySceneInteractionChanged)
        Q_PROPERTY(Interaction primarySelectInteraction READ primarySelectInteraction WRITE setPrimarySelectInteraction NOTIFY primarySelectInteractionChanged)
        Q_PROPERTY(Interaction secondarySelectInteraction READ secondarySelectInteraction WRITE setSecondarySelectInteraction NOTIFY secondarySelectInteractionChanged)
    public:
        explicit ClipPaneInteractionController(QObject *parent = nullptr);
        ~ClipPaneInteractionController() override;

        bool isClickSelectable() const;
        void setClickSelectable(bool clickSelectable);

        enum Interaction {
            None,
            Move,
            CopyAndMove,
            Split,
            RubberBandSelect = 0x10000,
            TimeRangeSelect,
            Draw,
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

        enum MoveFlag {
            MF_Move,
            MF_CopyAndMove,
        };
        Q_ENUM(MoveFlag)

        Q_INVOKABLE virtual ClipViewModel *createAndInsertClipOnDrawing(RangeSequenceViewModel *clipSequenceViewModel, int position, int trackIndex);

        enum AdjustLengthEdge {
            LeftEdge,
            RightEdge,
        };
        Q_ENUM(AdjustLengthEdge)

    Q_SIGNALS:
        void clickSelectableChanged();
        void primaryItemInteractionChanged();
        void secondaryItemInteractionChanged();
        void primarySceneInteractionChanged();
        void secondarySceneInteractionChanged();
        void primarySelectInteractionChanged();
        void secondarySelectInteractionChanged();

        void rubberBandDraggingStarted(QQuickItem *clipPane);
        void rubberBandDraggingCommitted(QQuickItem *clipPane);
        void rubberBandDraggingAborted(QQuickItem *clipPane);

        void movingStarted(QQuickItem *clipPane, ClipViewModel *item, MoveFlag flag);
        void movingCommitted(QQuickItem *clipPane, ClipViewModel *item);
        void movingAborted(QQuickItem *clipPane, ClipViewModel *item);

        void adjustLengthStarted(QQuickItem *clipPane, ClipViewModel *item, AdjustLengthEdge edge);
        void adjustLengthCommitted(QQuickItem *clipPane, ClipViewModel *item, AdjustLengthEdge edge);
        void adjustLengthAborted(QQuickItem *clipPane, ClipViewModel *item, AdjustLengthEdge edge);

        void drawingStarted(QQuickItem *clipPane);
        void drawingCommitted(QQuickItem *clipPane);
        void drawingAborted(QQuickItem *clipPane);

        void hoverEntered(QQuickItem *clipPane, int position, int trackIndex);
        void hoverMoved(QQuickItem *clipPane, int position, int trackIndex);
        void hoverExited(QQuickItem *clipPane);

        void itemHoverEntered(QQuickItem *clipPane, ClipViewModel *item);
        void itemHoverExited(QQuickItem *clipPane, ClipViewModel *item);

        void doubleClicked(QQuickItem *clipPane, int position, int trackIndex);
        void itemDoubleClicked(QQuickItem *clipPane, ClipViewModel *item);

        void contextMenuRequested(QQuickItem *clipPane, int position, int trackIndex);
        void itemContextMenuRequested(QQuickItem *clipPane, ClipViewModel *item);

        void splitAboutToStart(QQuickItem *clipPane);
        void splitStarted(QQuickItem *clipPane, int position);
        void splitCommitted(QQuickItem *clipPane, int position);
        void splitAborted(QQuickItem *clipPane);

    private:
        bool m_clickSelectable;
        Interaction m_primaryItemInteraction;
        Interaction m_secondaryItemInteraction;
        Interaction m_primarySceneInteraction;
        Interaction m_secondarySceneInteraction;
        Interaction m_primarySelectInteraction;
        Interaction m_secondarySelectInteraction;
    };

}

#endif //SCOPIC_FLOW_CLIPPANEINTERACTIONCONTROLLER_H
