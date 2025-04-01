#ifndef SCOPIC_FLOW_LABELSEQUENCEBEHAVIORVIEWMODEL_H
#define SCOPIC_FLOW_LABELSEQUENCEBEHAVIORVIEWMODEL_H

#include <QObject>
#include <QColor>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT LabelSequenceBehaviorViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(bool editing READ editing WRITE setEditing NOTIFY editingChanged)
        Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    public:
        explicit LabelSequenceBehaviorViewModel(QObject *parent = nullptr);
        ~LabelSequenceBehaviorViewModel() override;

        bool editing() const;
        void setEditing(bool editing);

        QColor color() const;
        void setColor(const QColor &color);

    signals:
        void editingChanged(bool editing);
        void colorChanged(const QColor &color);

    private:
        bool m_editing;
        QColor m_color;

    };

}

#endif //SCOPIC_FLOW_LABELSEQUENCEBEHAVIORVIEWMODEL_H
