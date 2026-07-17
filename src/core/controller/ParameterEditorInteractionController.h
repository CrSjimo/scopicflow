#ifndef SCOPIC_FLOW_PARAMETEREDITORINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_PARAMETEREDITORINTERACTIONCONTROLLER_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ParameterAnchorViewModel.h>
#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class AnchorParameterViewModel;

    class SCOPIC_FLOW_CORE_EXPORT ParameterEditorInteractionController : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(bool clickSelectable READ isClickSelectable WRITE setClickSelectable NOTIFY clickSelectableChanged)
        Q_PROPERTY(Interaction primaryItemInteraction READ primaryItemInteraction WRITE setPrimaryItemInteraction NOTIFY primaryItemInteractionChanged)
        Q_PROPERTY(Interaction secondaryItemInteraction READ secondaryItemInteraction WRITE setSecondaryItemInteraction NOTIFY secondaryItemInteractionChanged)
        Q_PROPERTY(Interaction primarySceneInteraction READ primarySceneInteraction WRITE setPrimarySceneInteraction NOTIFY primarySceneInteractionChanged)
        Q_PROPERTY(Interaction secondarySceneInteraction READ secondarySceneInteraction WRITE setSecondarySceneInteraction NOTIFY secondarySceneInteractionChanged)
        Q_PROPERTY(Interaction primarySelectInteraction READ primarySelectInteraction WRITE setPrimarySelectInteraction NOTIFY primarySelectInteractionChanged)
        Q_PROPERTY(Interaction secondarySelectInteraction READ secondarySelectInteraction WRITE setSecondarySelectInteraction NOTIFY secondarySelectInteractionChanged)
        Q_PROPERTY(ParameterAnchorViewModel::InterpolationMode newAnchorInterpolationMode READ newAnchorInterpolationMode WRITE setNewAnchorInterpolationMode NOTIFY newAnchorInterpolationModeChanged)
        Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
        Q_PROPERTY(double fillBaseline READ fillBaseline WRITE setFillBaseline NOTIFY fillBaselineChanged)
        Q_PROPERTY(bool referenceVisible READ isReferenceVisible WRITE setReferenceVisible NOTIFY referenceVisibleChanged)
        Q_PROPERTY(double referenceBaseline READ referenceBaseline WRITE setReferenceBaseline NOTIFY referenceBaselineChanged)
        Q_PROPERTY(bool defaultValueEnabled READ isDefaultValueEnabled WRITE setDefaultValueEnabled NOTIFY defaultValueEnabledChanged)
        Q_PROPERTY(double defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
        Q_PROPERTY(CurveDisplayMode originalAndDefaultCurveDisplayMode READ originalAndDefaultCurveDisplayMode WRITE setOriginalAndDefaultCurveDisplayMode NOTIFY originalAndDefaultCurveDisplayModeChanged)

    public:
        enum Interaction {
            None,
            Pencil,
            Eraser,
            Pointer,
            Pen,
            ConvertAnchor,
            FreeRangeSelect = 0x10000,
            AnchorRubberBandSelect,
            AnchorTimeRangeSelect,
        };
        Q_ENUM(Interaction)

        enum FreeEditOperation {
            DrawFree,
            EraseFree,
        };
        Q_ENUM(FreeEditOperation)

        enum FillMode {
            NoFill,
            TopFill,
            BottomFill,
            BaselineFill,
        };
        Q_ENUM(FillMode)

        enum CurveDisplayMode {
            CurveHidden,
            CurveDashed,
            CurveSolid,
        };
        Q_ENUM(CurveDisplayMode)

        explicit ParameterEditorInteractionController(QObject *parent = nullptr);
        ~ParameterEditorInteractionController() override;

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

        ParameterAnchorViewModel::InterpolationMode newAnchorInterpolationMode() const;
        void setNewAnchorInterpolationMode(ParameterAnchorViewModel::InterpolationMode interpolationMode);

        FillMode fillMode() const;
        void setFillMode(FillMode fillMode);
        double fillBaseline() const;
        void setFillBaseline(double fillBaseline);
        bool isReferenceVisible() const;
        void setReferenceVisible(bool referenceVisible);
        double referenceBaseline() const;
        void setReferenceBaseline(double referenceBaseline);
        bool isDefaultValueEnabled() const;
        void setDefaultValueEnabled(bool defaultValueEnabled);
        double defaultValue() const;
        void setDefaultValue(double defaultValue);
        CurveDisplayMode originalAndDefaultCurveDisplayMode() const;
        void setOriginalAndDefaultCurveDisplayMode(CurveDisplayMode mode);

        Q_INVOKABLE virtual ParameterAnchorViewModel *createAnchor(int position, double value);
        Q_INVOKABLE virtual bool insertAnchor(AnchorParameterViewModel *viewModel,
                                              ParameterAnchorViewModel *item);
        Q_INVOKABLE virtual ParameterAnchorViewModel *createAndInsertAnchor(AnchorParameterViewModel *viewModel,
                                                                            int position,
                                                                            double value);
        Q_INVOKABLE virtual bool removeAnchor(AnchorParameterViewModel *viewModel,
                                              ParameterAnchorViewModel *item);

    Q_SIGNALS:
        void clickSelectableChanged();
        void primaryItemInteractionChanged();
        void secondaryItemInteractionChanged();
        void primarySceneInteractionChanged();
        void secondarySceneInteractionChanged();
        void primarySelectInteractionChanged();
        void secondarySelectInteractionChanged();
        void newAnchorInterpolationModeChanged();
        void fillModeChanged();
        void fillBaselineChanged();
        void referenceVisibleChanged();
        void referenceBaselineChanged();
        void defaultValueEnabledChanged();
        void defaultValueChanged();
        void originalAndDefaultCurveDisplayModeChanged();

        void freeEditingStarted(QQuickItem *editor, FreeEditOperation operation);
        void freeEditingCommitted(QQuickItem *editor, FreeEditOperation operation);
        void freeEditingAborted(QQuickItem *editor, FreeEditOperation operation);

        void anchorInsertionStarted(QQuickItem *editor, int position, double value);
        void anchorInsertionCommitted(QQuickItem *editor, ParameterAnchorViewModel *item);
        void anchorInsertionAborted(QQuickItem *editor);

        void anchorMovingStarted(QQuickItem *editor, ParameterAnchorViewModel *item);
        void anchorMovingCommitted(QQuickItem *editor, ParameterAnchorViewModel *item);
        void anchorMovingAborted(QQuickItem *editor, ParameterAnchorViewModel *item);

        void anchorDeletionStarted(QQuickItem *editor, ParameterAnchorViewModel *item);
        void anchorDeletionCommitted(QQuickItem *editor, ParameterAnchorViewModel *item);
        void anchorDeletionAborted(QQuickItem *editor, ParameterAnchorViewModel *item);

        void anchorInterpolationChangingStarted(QQuickItem *editor, ParameterAnchorViewModel *item);
        void anchorInterpolationChangingCommitted(QQuickItem *editor, ParameterAnchorViewModel *item);
        void anchorInterpolationChangingAborted(QQuickItem *editor, ParameterAnchorViewModel *item);

        void freeRangeSelectingStarted(QQuickItem *editor);
        void freeRangeSelectingCommitted(QQuickItem *editor, int start, int end);
        void freeRangeSelectingAborted(QQuickItem *editor);

        void anchorRubberBandDraggingStarted(QQuickItem *editor);
        void anchorRubberBandDraggingCommitted(QQuickItem *editor);
        void anchorRubberBandDraggingAborted(QQuickItem *editor);

        void hoverEntered(QQuickItem *editor, int position, double transformedValue);
        void hoverMoved(QQuickItem *editor, int position, double transformedValue);
        void hoverExited(QQuickItem *editor);
        void itemHoverEntered(QQuickItem *editor, ParameterAnchorViewModel *item);
        void itemHoverExited(QQuickItem *editor, ParameterAnchorViewModel *item);
        void contextMenuRequested(QQuickItem *editor, int position, double transformedValue);
        void itemContextMenuRequested(QQuickItem *editor, ParameterAnchorViewModel *item);

    private:
        bool m_clickSelectable = true;
        Interaction m_primaryItemInteraction = None;
        Interaction m_secondaryItemInteraction = None;
        Interaction m_primarySceneInteraction = None;
        Interaction m_secondarySceneInteraction = None;
        Interaction m_primarySelectInteraction = None;
        Interaction m_secondarySelectInteraction = None;
        ParameterAnchorViewModel::InterpolationMode m_newAnchorInterpolationMode = ParameterAnchorViewModel::Hermite;
        FillMode m_fillMode = NoFill;
        double m_fillBaseline = 0.0;
        bool m_referenceVisible = false;
        double m_referenceBaseline = 0.0;
        bool m_defaultValueEnabled = false;
        double m_defaultValue = 0.0;
        CurveDisplayMode m_originalAndDefaultCurveDisplayMode = CurveHidden;
    };

}

#endif // SCOPIC_FLOW_PARAMETEREDITORINTERACTIONCONTROLLER_H
