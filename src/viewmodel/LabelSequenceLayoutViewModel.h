#ifndef SCOPIC_FLOW_LABELSEQUENCELAYOUTVIEWMODEL_H
#define SCOPIC_FLOW_LABELSEQUENCELAYOUTVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT LabelSequenceLayoutViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(bool editing READ editing WRITE setEditing NOTIFY editingChanged)
    public:
        explicit LabelSequenceLayoutViewModel(QObject *parent = nullptr);
        ~LabelSequenceLayoutViewModel() override;

        bool editing() const;
        void setEditing(bool editing);

    signals:
        void editingChanged(bool editing);

    private:
        bool m_editing;

    };

}

#endif //SCOPIC_FLOW_LABELSEQUENCELAYOUTVIEWMODEL_H
