#ifndef SCOPIC_FLOW_DYNAMICMIXINGEDITORINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_DYNAMICMIXINGEDITORINTERACTIONCONTROLLER_H

#include <QList>
#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class DynamicMixingAnchorViewModel;
    class DynamicMixingViewModel;

    class SCOPIC_FLOW_CORE_EXPORT DynamicMixingEditorInteractionController : public QObject {
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
        enum Interaction {
            None,
            Pointer,
            AddAnchor,
            DeleteAnchor,
            RubberBandSelect = 0x10000,
        };
        Q_ENUM(Interaction)

        explicit DynamicMixingEditorInteractionController(QObject *parent = nullptr);
        ~DynamicMixingEditorInteractionController() override;

        bool isClickSelectable() const;
        void setClickSelectable(bool clickSelectable);

        Interaction primaryItemInteraction() const;
        void setPrimaryItemInteraction(Interaction interaction);
        Interaction secondaryItemInteraction() const;
        void setSecondaryItemInteraction(Interaction interaction);
        Interaction primarySceneInteraction() const;
        void setPrimarySceneInteraction(Interaction interaction);
        Interaction secondarySceneInteraction() const;
        void setSecondarySceneInteraction(Interaction interaction);
        Interaction primarySelectInteraction() const;
        void setPrimarySelectInteraction(Interaction interaction);
        Interaction secondarySelectInteraction() const;
        void setSecondarySelectInteraction(Interaction interaction);

        Q_INVOKABLE virtual DynamicMixingAnchorViewModel *createAnchor(int position,
                                                                       const QList<double> &ratio);
        Q_INVOKABLE virtual bool insertAnchor(DynamicMixingViewModel *viewModel,
                                              DynamicMixingAnchorViewModel *item);
        Q_INVOKABLE virtual DynamicMixingAnchorViewModel *createAndInsertAnchor(
            DynamicMixingViewModel *viewModel,
            int position,
            const QList<double> &ratio);
        Q_INVOKABLE virtual bool removeAnchor(DynamicMixingViewModel *viewModel,
                                              DynamicMixingAnchorViewModel *item);

    Q_SIGNALS:
        void clickSelectableChanged();
        void primaryItemInteractionChanged();
        void secondaryItemInteractionChanged();
        void primarySceneInteractionChanged();
        void secondarySceneInteractionChanged();
        void primarySelectInteractionChanged();
        void secondarySelectInteractionChanged();

        void anchorInsertionStarted(QQuickItem *editor, int position, const QList<double> &ratio);
        void anchorInsertionCommitted(QQuickItem *editor, DynamicMixingAnchorViewModel *item);
        void anchorInsertionAborted(QQuickItem *editor);

        void anchorMovingStarted(QQuickItem *editor, DynamicMixingAnchorViewModel *item);
        void anchorMovingCommitted(QQuickItem *editor, DynamicMixingAnchorViewModel *item);
        void anchorMovingAborted(QQuickItem *editor, DynamicMixingAnchorViewModel *item);

        void anchorDeletionStarted(QQuickItem *editor, DynamicMixingAnchorViewModel *item);
        void anchorDeletionCommitted(QQuickItem *editor, DynamicMixingAnchorViewModel *item);
        void anchorDeletionAborted(QQuickItem *editor, DynamicMixingAnchorViewModel *item);

        void rubberBandDraggingStarted(QQuickItem *editor);
        void rubberBandDraggingCommitted(QQuickItem *editor);
        void rubberBandDraggingAborted(QQuickItem *editor);

        void hoverEntered(QQuickItem *editor, int position);
        void hoverMoved(QQuickItem *editor, int position);
        void hoverExited(QQuickItem *editor);
        void itemHoverEntered(QQuickItem *editor, DynamicMixingAnchorViewModel *item);
        void itemHoverExited(QQuickItem *editor, DynamicMixingAnchorViewModel *item);
        void contextMenuRequested(QQuickItem *editor, int position);
        void itemContextMenuRequested(QQuickItem *editor, DynamicMixingAnchorViewModel *item);

    private:
        bool m_clickSelectable = true;
        Interaction m_primaryItemInteraction = None;
        Interaction m_secondaryItemInteraction = None;
        Interaction m_primarySceneInteraction = None;
        Interaction m_secondarySceneInteraction = None;
        Interaction m_primarySelectInteraction = None;
        Interaction m_secondarySelectInteraction = None;
    };

}

#endif // SCOPIC_FLOW_DYNAMICMIXINGEDITORINTERACTIONCONTROLLER_H
