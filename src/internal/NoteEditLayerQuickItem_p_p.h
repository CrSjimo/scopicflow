#ifndef SCOPIC_FLOW_NOTEEDITLAYERQUICKITEM_P_P_H
#define SCOPIC_FLOW_NOTEEDITLAYERQUICKITEM_P_P_H

#include <array>

#include <QAbstractListModel>
#include <QHash>
#include <QMetaObject>
#include <QPointF>
#include <QPointer>
#include <QSet>
#include <QSizeF>
#include <QString>
#include <QVector>

#include <interval-tree/interval_tree.hpp>

#include <ScopicFlowInternal/private/NoteEditLayerQuickItem_p.h>

namespace sflow {

    class NoteViewModel;

    struct NoteEditLayerRecord {
        NoteViewModel *model = nullptr;
        QString lyric;
        QString additionalText;
        int position = 0;
        int length = 0;
        int key = 0;
        int nextNotePosition = 0;
        int nextNoteKey = 0;
        bool selected = false;
        bool overlapped = false;
        bool additionalTextHighlighted = false;
        quint64 insertionOrder = 0;
        mutable QSizeF additionalTextSize;
        mutable quint64 additionalTextMetricsRevision = 0;
    };

    struct NoteEditLayerInterval : lib_interval_tree::interval<int> {
        NoteEditLayerInterval();
        NoteEditLayerInterval(int position, int length, NoteViewModel *model);

        bool operator==(const NoteEditLayerInterval &other) const;

        NoteViewModel *model = nullptr;
    };

    struct NoteEditLayerMarker {
        QPointer<NoteViewModel> model;
        NoteEditLayerQuickItem::MarkerType type = NoteEditLayerQuickItem::ShortNoteMarker;
        QPointF center;
        bool selected = false;

        bool operator==(const NoteEditLayerMarker &other) const = default;
    };

    class NoteEditLayerMarkerModel : public QAbstractListModel {
    public:
        enum Role {
            ModelObjectRole = Qt::UserRole + 1,
            MarkerTypeRole,
            CenterXRole,
            CenterYRole,
            SelectedRole,
        };

        explicit NoteEditLayerMarkerModel(QObject *parent = nullptr);

        int rowCount(const QModelIndex &parent = {}) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QHash<int, QByteArray> roleNames() const override;

        void setMarkers(QVector<NoteEditLayerMarker> markers);
        void clear();

    private:
        QVector<NoteEditLayerMarker> m_markers;
    };

    class NoteEditLayerQuickItemPrivate {
        Q_DECLARE_PUBLIC(NoteEditLayerQuickItem)

    public:
        using IntervalTree = lib_interval_tree::interval_tree<NoteEditLayerInterval>;

        explicit NoteEditLayerQuickItemPrivate(NoteEditLayerQuickItem *q);
        ~NoteEditLayerQuickItemPrivate();

        void resetNoteSequenceViewModel(RangeSequenceViewModel *viewModel);
        void attachItem(NoteViewModel *model);
        void detachItem(NoteViewModel *model);
        void detachAllItems();
        void updateRecord(NoteViewModel *model, bool geometryChanged, bool selectorGeometryChanged);

        void insertIntoIndex(const NoteEditLayerRecord &record);
        void removeFromIndex(const NoteEditLayerRecord &record);
        QSet<NoteViewModel *> queryIndex(int minimumKey, int maximumKey, int position, int length) const;

        void updateViewport(bool forceResidentRange, bool geometryChanged);
        void clearResidentItems();
        void scheduleResidentTrim();
        void scheduleMarkerRebuild();
        void rebuildMarkers();
        void updateSelectorRegistration(NoteViewModel *model);
        void removeSelectorRegistration(NoteViewModel *model);
        void resetSelectorRegistrations();
        void bumpGeometryRevision();

        bool hasValidLayout() const;
        QRectF itemRect(const NoteEditLayerRecord &record) const;
        QSizeF additionalTextSize(const NoteEditLayerRecord &record) const;
        qreal additionalTextHeight() const;
        QRectF additionalTextRect(const NoteEditLayerRecord &record, bool clipped) const;
        int visualLayer(const NoteEditLayerRecord &record) const;
        const NoteEditLayerRecord *hitTestRecord(const QPointF &point) const;
        const NoteEditLayerRecord *hitTestAdditionalTextRecord(const QPointF &point) const;

        NoteEditLayerQuickItem *q_ptr = nullptr;
        QPointer<RangeSequenceViewModel> noteSequenceViewModel;
        QPointer<TimeViewModel> timeViewModel;
        QPointer<TimeLayoutViewModel> timeLayoutViewModel;
        QPointer<ClavierViewModel> clavierViewModel;
        QPointer<SelectionController> selectionController;
        QPointer<RubberBandSelector> rubberBandSelector;
        QPointer<NoteViewModel> lyricEditingItem;
        QPointer<NoteViewModel> additionalTextEditingItem;
        QMetaObject::Connection lyricEditingItemDestroyedConnection;
        QMetaObject::Connection additionalTextEditingItemDestroyedConnection;

        QColor fillColor;
        QColor selectedFillColor;
        QColor selectedBorderColor;
        QColor overlappedBorderColor;
        QColor textColor;
        QColor additionalTextColor;
        QColor highlightedAdditionalTextColor;
        QFont font;
        bool active = true;
        bool transparentDisplay = false;
        bool thumbnailDisplay = false;
        bool editScopeFocused = false;
        bool additionalTextAbove = false;
        double viewportWidth = 0;
        double viewportHeight = 0;
        int shortNoteThreshold = 0;
        quint64 geometryRevision = 0;
        quint64 fontRevision = 1;
        mutable qreal additionalTextLineHeight = 0;
        mutable quint64 additionalTextLineHeightRevision = 0;
        quint64 nextInsertionOrder = 0;
        quint64 residentTrimGeneration = 0;

        QHash<NoteViewModel *, NoteEditLayerRecord> records;
        QHash<NoteViewModel *, QVector<QMetaObject::Connection>> recordConnections;
        std::array<IntervalTree, 128> intervalTrees;
        QSet<NoteViewModel *> residentItems;
        QSet<NoteViewModel *> selectorItems;
        bool resettingSequence = false;
        bool markerRebuildPending = false;
        bool geometryRevisionPending = false;
        bool residentRangeValid = false;
        double residentPositionStart = 0;
        double residentPositionEnd = 0;
        double residentKeyStart = 0;
        double residentKeyEnd = 0;

        NoteEditLayerMarkerModel markerModel;
    };

}

#endif // SCOPIC_FLOW_NOTEEDITLAYERQUICKITEM_P_P_H
