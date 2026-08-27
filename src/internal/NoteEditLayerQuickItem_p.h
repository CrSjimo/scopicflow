#ifndef SCOPIC_FLOW_NOTEEDITLAYERQUICKITEM_P_H
#define SCOPIC_FLOW_NOTEEDITLAYERQUICKITEM_P_H

#include <QAbstractItemModel>
#include <QColor>
#include <QFont>
#include <QQuickItem>
#include <QRectF>
#include <QScopedPointer>
#include <qqmlintegration.h>

#include <ScopicFlowInternal/private/PointerInteractionTypes_p.h>

namespace sflow {

    class ClavierViewModel;
    class NoteEditLayerQuickItemPrivate;
    class NoteViewModel;
    class RangeSequenceViewModel;
    class RubberBandSelector;
    class SelectionController;
    class TimeLayoutViewModel;
    class TimeViewModel;

    class NoteEditLayerQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(NoteEditLayerItem)
        Q_DECLARE_PRIVATE(NoteEditLayerQuickItem)
        Q_PROPERTY(RangeSequenceViewModel *noteSequenceViewModel READ noteSequenceViewModel WRITE setNoteSequenceViewModel NOTIFY noteSequenceViewModelChanged)
        Q_PROPERTY(TimeViewModel *timeViewModel READ timeViewModel WRITE setTimeViewModel NOTIFY timeViewModelChanged)
        Q_PROPERTY(TimeLayoutViewModel *timeLayoutViewModel READ timeLayoutViewModel WRITE setTimeLayoutViewModel NOTIFY timeLayoutViewModelChanged)
        Q_PROPERTY(ClavierViewModel *clavierViewModel READ clavierViewModel WRITE setClavierViewModel NOTIFY clavierViewModelChanged)
        Q_PROPERTY(SelectionController *selectionController READ selectionController WRITE setSelectionController NOTIFY selectionControllerChanged)
        Q_PROPERTY(RubberBandSelector *rubberBandSelector READ rubberBandSelector WRITE setRubberBandSelector NOTIFY rubberBandSelectorChanged)
        Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor NOTIFY fillColorChanged)
        Q_PROPERTY(QColor selectedFillColor READ selectedFillColor WRITE setSelectedFillColor NOTIFY selectedFillColorChanged)
        Q_PROPERTY(QColor selectedBorderColor READ selectedBorderColor WRITE setSelectedBorderColor NOTIFY selectedBorderColorChanged)
        Q_PROPERTY(QColor overlappedBorderColor READ overlappedBorderColor WRITE setOverlappedBorderColor NOTIFY overlappedBorderColorChanged)
        Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
        Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
        Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
        Q_PROPERTY(bool transparentDisplay READ isTransparentDisplay WRITE setTransparentDisplay NOTIFY transparentDisplayChanged)
        Q_PROPERTY(bool thumbnailDisplay READ isThumbnailDisplay WRITE setThumbnailDisplay NOTIFY thumbnailDisplayChanged)
        Q_PROPERTY(bool editScopeFocused READ isEditScopeFocused WRITE setEditScopeFocused NOTIFY editScopeFocusedChanged)
        Q_PROPERTY(NoteViewModel *lyricEditingItem READ lyricEditingItem WRITE setLyricEditingItem NOTIFY lyricEditingItemChanged)
        Q_PROPERTY(double viewportWidth READ viewportWidth WRITE setViewportWidth NOTIFY viewportWidthChanged)
        Q_PROPERTY(double viewportHeight READ viewportHeight WRITE setViewportHeight NOTIFY viewportHeightChanged)
        Q_PROPERTY(int shortNoteThreshold READ shortNoteThreshold WRITE setShortNoteThreshold NOTIFY shortNoteThresholdChanged)
        Q_PROPERTY(bool additionalTextAbove READ isAdditionalTextAbove WRITE setAdditionalTextAbove NOTIFY additionalTextAboveChanged)
        Q_PROPERTY(quint64 geometryRevision READ geometryRevision NOTIFY geometryRevisionChanged)
        Q_PROPERTY(QAbstractItemModel *shortMarkerModel READ shortMarkerModel CONSTANT)

    public:
        enum MarkerType {
            ShortNoteMarker,
            ShortRestMarker,
        };
        Q_ENUM(MarkerType)

        explicit NoteEditLayerQuickItem(QQuickItem *parent = nullptr);
        ~NoteEditLayerQuickItem() override;

        RangeSequenceViewModel *noteSequenceViewModel() const;
        void setNoteSequenceViewModel(RangeSequenceViewModel *noteSequenceViewModel);

        TimeViewModel *timeViewModel() const;
        void setTimeViewModel(TimeViewModel *timeViewModel);

        TimeLayoutViewModel *timeLayoutViewModel() const;
        void setTimeLayoutViewModel(TimeLayoutViewModel *timeLayoutViewModel);

        ClavierViewModel *clavierViewModel() const;
        void setClavierViewModel(ClavierViewModel *clavierViewModel);

        SelectionController *selectionController() const;
        void setSelectionController(SelectionController *selectionController);

        RubberBandSelector *rubberBandSelector() const;
        void setRubberBandSelector(RubberBandSelector *rubberBandSelector);

        QColor fillColor() const;
        void setFillColor(const QColor &fillColor);

        QColor selectedFillColor() const;
        void setSelectedFillColor(const QColor &selectedFillColor);

        QColor selectedBorderColor() const;
        void setSelectedBorderColor(const QColor &selectedBorderColor);

        QColor overlappedBorderColor() const;
        void setOverlappedBorderColor(const QColor &overlappedBorderColor);

        QColor textColor() const;
        void setTextColor(const QColor &textColor);

        QFont font() const;
        void setFont(const QFont &font);

        bool isActive() const;
        void setActive(bool active);

        bool isTransparentDisplay() const;
        void setTransparentDisplay(bool transparentDisplay);

        bool isThumbnailDisplay() const;
        void setThumbnailDisplay(bool thumbnailDisplay);

        bool isEditScopeFocused() const;
        void setEditScopeFocused(bool editScopeFocused);

        NoteViewModel *lyricEditingItem() const;
        void setLyricEditingItem(NoteViewModel *lyricEditingItem);

        double viewportWidth() const;
        void setViewportWidth(double viewportWidth);

        double viewportHeight() const;
        void setViewportHeight(double viewportHeight);

        int shortNoteThreshold() const;
        void setShortNoteThreshold(int shortNoteThreshold);

        bool isAdditionalTextAbove() const;
        void setAdditionalTextAbove(bool additionalTextAbove);

        quint64 geometryRevision() const;
        QAbstractItemModel *shortMarkerModel() const;

        Q_INVOKABLE PointerHit hitTest(const QPointF &point, QQuickItem *coordinateSpace = nullptr) const;
        Q_INVOKABLE QRectF itemRect(QObject *model) const;
        bool contains(const QPointF &point) const override;

        QRectF mapToTickKeyRect(const QRectF &rect) const;

    Q_SIGNALS:
        void noteSequenceViewModelChanged();
        void timeViewModelChanged();
        void timeLayoutViewModelChanged();
        void clavierViewModelChanged();
        void selectionControllerChanged();
        void rubberBandSelectorChanged();
        void fillColorChanged();
        void selectedFillColorChanged();
        void selectedBorderColorChanged();
        void overlappedBorderColorChanged();
        void textColorChanged();
        void fontChanged();
        void activeChanged();
        void transparentDisplayChanged();
        void thumbnailDisplayChanged();
        void editScopeFocusedChanged();
        void lyricEditingItemChanged();
        void viewportWidthChanged();
        void viewportHeightChanged();
        void shortNoteThresholdChanged();
        void additionalTextAboveChanged();
        void geometryRevisionChanged();

    protected:
        QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<NoteEditLayerQuickItemPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_NOTEEDITLAYERQUICKITEM_P_H
