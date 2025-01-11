#ifndef SCOPIC_FLOW_SCOPEDINTERMEDIATE_P_H
#define SCOPIC_FLOW_SCOPEDINTERMEDIATE_P_H

#include <QVariant>

namespace sflow {
    class ScopedIntermediate {
        QObject *m_viewModel;
    public:
        explicit ScopedIntermediate(QObject *viewModel) {
            m_viewModel = viewModel;
            viewModel->setProperty("intermediate", true);
        }
        ~ScopedIntermediate() {
            m_viewModel->setProperty("intermediate", false);
        }
    };
}

#endif //SCOPIC_FLOW_SCOPEDINTERMEDIATE_P_H
