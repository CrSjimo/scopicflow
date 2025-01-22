#ifndef SCOPIC_FLOW_SLICEABLEVIEWMODELQMLHANDLE_P_H
#define SCOPIC_FLOW_SLICEABLEVIEWMODELQMLHANDLE_P_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT SliceableViewModelQmlHandle : public QObject {
        Q_OBJECT
    public:
        explicit SliceableViewModelQmlHandle(QObject *parent = nullptr);
        ~SliceableViewModelQmlHandle() override;

        virtual QObjectList slice(int position, int length) = 0;
        virtual int itemPosition(QObject *item) const = 0;
        virtual int itemLength(QObject *item) const;

    signals:
        void itemInserted(QObject *item);
        void itemRemoved(QObject *item);
        void itemUpdated(QObject *item);
    };
}

#endif //SCOPIC_FLOW_SLICEABLEVIEWMODELQMLHANDLE_P_H
