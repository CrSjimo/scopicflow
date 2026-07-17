#ifndef SCOPIC_FLOW_ANCHORPARAMETERVIEWMODEL_H
#define SCOPIC_FLOW_ANCHORPARAMETERVIEWMODEL_H

#include <QObject>
#include <QList>
#include <QVariant>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class ParameterAnchorViewModel;
    class AnchorParameterViewModelPrivate;
    class AnchorParameterViewModelSliceableManipulatorInterface;

    class SCOPIC_FLOW_CORE_EXPORT AnchorParameterViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(AnchorParameterViewModel)
        Q_PRIVATE_PROPERTY(d_func(), AnchorParameterViewModelSliceableManipulatorInterface *iSliceable MEMBER iSliceable CONSTANT)
        Q_PROPERTY(int count READ count NOTIFY countChanged)
        Q_PROPERTY(QObjectList items READ items NOTIFY itemsChanged)
        Q_PROPERTY(ParameterAnchorViewModel *movingItem READ movingItem NOTIFY movingItemChanged)

    public:
        explicit AnchorParameterViewModel(QObject *parent = nullptr);
        ~AnchorParameterViewModel() override;

        int count() const;
        QObjectList items() const;
        ParameterAnchorViewModel *movingItem() const;

        Q_INVOKABLE QObjectList slice(int position, int length) const;
        QObjectList effectiveSlice(int position, int length) const;
        Q_INVOKABLE ParameterAnchorViewModel *itemAtPosition(int position) const;
        Q_INVOKABLE ParameterAnchorViewModel *previousItem(ParameterAnchorViewModel *item) const;
        Q_INVOKABLE ParameterAnchorViewModel *nextItem(ParameterAnchorViewModel *item) const;
        Q_INVOKABLE QVariant valueAt(double position) const;
        QList<QVariant> valuesAt(const QList<double> &sortedPositions) const;

        Q_INVOKABLE bool insertItem(ParameterAnchorViewModel *item);
        Q_INVOKABLE bool removeItem(ParameterAnchorViewModel *item);
        Q_INVOKABLE bool moveItem(ParameterAnchorViewModel *item, int position);

        Q_INVOKABLE bool beginInteractiveMove(ParameterAnchorViewModel *item);
        bool beginInteractiveMove(const QList<ParameterAnchorViewModel *> &items,
                                  ParameterAnchorViewModel *primaryItem);
        bool updateInteractiveMove(const QList<ParameterAnchorViewModel *> &items,
                                   const QList<int> &positions,
                                   const QList<double> &values);
        Q_INVOKABLE QObjectList commitInteractiveMove(ParameterAnchorViewModel *item);
        Q_INVOKABLE bool abortInteractiveMove(ParameterAnchorViewModel *item);

    Q_SIGNALS:
        void countChanged(int count);
        void itemsChanged();
        void movingItemChanged();
        void itemUpdated(ParameterAnchorViewModel *item);
        void itemAboutToInsert(ParameterAnchorViewModel *item);
        void itemInserted(ParameterAnchorViewModel *item);
        void itemAboutToRemove(ParameterAnchorViewModel *item);
        void itemRemoved(ParameterAnchorViewModel *item);

    private:
        friend class ParameterAnchorViewModel;
        friend class AnchorParameterViewModelSliceableManipulatorInterface;

        QScopedPointer<AnchorParameterViewModelPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_ANCHORPARAMETERVIEWMODEL_H
