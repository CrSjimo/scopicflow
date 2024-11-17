#ifndef SCOPIC_FLOW_LABELVIEWMODEL_H
#define SCOPIC_FLOW_LABELVIEWMODEL_H

#include <QObject>
#include <QVariant>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT LabelViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(QVariant content READ content WRITE setContent NOTIFY contentChanged)
        Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
    public:
        explicit LabelViewModel(QObject *parent = nullptr);
        ~LabelViewModel() override;

        int position() const;
        void setPosition(int position);

        QVariant content() const;
        void setContent(const QVariant &content);

        bool selected() const;
        void setSelected(bool selected);

    signals:
        void positionChanged(int position);
        void contentChanged(const QVariant &content);
        void selectedChanged(bool selected);

    private:
        int m_position;
        bool m_selected;
        QVariant m_content;
    };

} // sflow

#endif //SCOPIC_FLOW_LABELVIEWMODEL_H
