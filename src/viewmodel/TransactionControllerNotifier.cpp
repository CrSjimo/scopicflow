#include "TransactionControllerNotifier.h"

namespace sflow {
    TransactionControllerNotifier::TransactionControllerNotifier(QObject *parent) : QObject(parent) {
    }
    TransactionControllerNotifier::~TransactionControllerNotifier() = default;
}