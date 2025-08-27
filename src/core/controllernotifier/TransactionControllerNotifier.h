#ifndef SCOPIC_FLOW_TRANSACTIONCONTROLLERNOTIFIER_H
#define SCOPIC_FLOW_TRANSACTIONCONTROLLERNOTIFIER_H

#include <QObject>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT TransactionControllerNotifier : public QObject {
        Q_OBJECT
    public:
        explicit TransactionControllerNotifier(QObject *parent = nullptr);
        ~TransactionControllerNotifier() override;
    signals:
        void transactionAboutToBegin();
        void transactionCommitted();
        void transactionAborted();
    };

}

#endif //SCOPIC_FLOW_TRANSACTIONCONTROLLERNOTIFIER_H
