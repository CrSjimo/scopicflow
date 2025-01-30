#ifndef SCOPIC_FLOW_LABELSEQUENCEBEHAVIORVIEWMODEL_H
#define SCOPIC_FLOW_LABELSEQUENCEBEHAVIORVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT LabelSequenceBehaviorViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(bool editing READ editing WRITE setEditing NOTIFY editingChanged)
    public:
        explicit LabelSequenceBehaviorViewModel(QObject *parent = nullptr);
        ~LabelSequenceBehaviorViewModel() override;

        bool editing() const;
        void setEditing(bool editing);

    signals:
        void editingChanged(bool editing);

    private:
        bool m_editing;

    };

}

#endif //SCOPIC_FLOW_LABELSEQUENCEBEHAVIORVIEWMODEL_H
