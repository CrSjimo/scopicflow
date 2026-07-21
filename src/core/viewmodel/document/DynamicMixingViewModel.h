#ifndef SCOPIC_FLOW_DYNAMICMIXINGVIEWMODEL_H
#define SCOPIC_FLOW_DYNAMICMIXINGVIEWMODEL_H

#include <QList>
#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class DynamicMixingAnchorViewModel;
    class DynamicMixingViewModelPrivate;
    class DynamicMixingViewModelSliceableManipulatorInterface;

    class SCOPIC_FLOW_CORE_EXPORT DynamicMixingViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(DynamicMixingViewModel)
        Q_PRIVATE_PROPERTY(d_func(), DynamicMixingViewModelSliceableManipulatorInterface *iSliceable MEMBER iSliceable CONSTANT)
        Q_PROPERTY(int voiceCount READ voiceCount WRITE setVoiceCount NOTIFY voiceCountChanged)
        Q_PROPERTY(int count READ count NOTIFY countChanged)
        Q_PROPERTY(QObjectList items READ items NOTIFY itemsChanged)
        Q_PROPERTY(DynamicMixingAnchorViewModel *movingItem READ movingItem NOTIFY movingItemChanged)

    public:
        explicit DynamicMixingViewModel(QObject *parent = nullptr);
        ~DynamicMixingViewModel() override;

        int voiceCount() const;
        void setVoiceCount(int voiceCount);

        int count() const;
        QObjectList items() const;
        DynamicMixingAnchorViewModel *movingItem() const;

        Q_INVOKABLE QObjectList slice(int position, int length) const;
        QObjectList effectiveSlice(int position, int length) const;
        Q_INVOKABLE QObjectList itemsAtPosition(int position) const;
        Q_INVOKABLE DynamicMixingAnchorViewModel *itemAtPosition(int position) const;
        Q_INVOKABLE DynamicMixingAnchorViewModel *itemAtOrBeforePosition(int position) const;
        Q_INVOKABLE DynamicMixingAnchorViewModel *itemAtOrAfterPosition(int position) const;
        Q_INVOKABLE DynamicMixingAnchorViewModel *previousItem(DynamicMixingAnchorViewModel *item) const;
        Q_INVOKABLE DynamicMixingAnchorViewModel *nextItem(DynamicMixingAnchorViewModel *item) const;
        Q_INVOKABLE QList<double> ratioAt(double position) const;
        QList<double> effectiveRatio(const DynamicMixingAnchorViewModel *item) const;

        Q_INVOKABLE bool contains(DynamicMixingAnchorViewModel *item) const;
        Q_INVOKABLE bool insertItem(DynamicMixingAnchorViewModel *item);
        Q_INVOKABLE bool removeItem(DynamicMixingAnchorViewModel *item);
        Q_INVOKABLE bool moveItem(DynamicMixingAnchorViewModel *item, int position);

        Q_INVOKABLE bool beginInteractiveMove(DynamicMixingAnchorViewModel *item);
        bool beginInteractiveMove(const QList<DynamicMixingAnchorViewModel *> &items,
                                  DynamicMixingAnchorViewModel *primaryItem);
        bool updateInteractiveMove(const QList<DynamicMixingAnchorViewModel *> &items,
                                   const QList<int> &positions,
                                   const QList<QList<double>> &ratios);
        Q_INVOKABLE QObjectList commitInteractiveMove(DynamicMixingAnchorViewModel *item);
        Q_INVOKABLE bool abortInteractiveMove(DynamicMixingAnchorViewModel *item);

    Q_SIGNALS:
        void voiceCountChanged();
        void countChanged(int count);
        void itemsChanged();
        void movingItemChanged();
        void itemUpdated(DynamicMixingAnchorViewModel *item);
        void itemAboutToInsert(DynamicMixingAnchorViewModel *item);
        void itemInserted(DynamicMixingAnchorViewModel *item);
        void itemAboutToRemove(DynamicMixingAnchorViewModel *item);
        void itemRemoved(DynamicMixingAnchorViewModel *item);

    private:
        friend class DynamicMixingAnchorViewModel;
        friend class DynamicMixingViewModelSliceableManipulatorInterface;

        QScopedPointer<DynamicMixingViewModelPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_DYNAMICMIXINGVIEWMODEL_H
