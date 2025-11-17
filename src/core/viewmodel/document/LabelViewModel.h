#ifndef SCOPIC_FLOW_LABELVIEWMODEL_H
#define SCOPIC_FLOW_LABELVIEWMODEL_H

#include <QObject>
#include <QString>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT LabelViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
        Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
    public:
        explicit LabelViewModel(QObject *parent = nullptr);
        ~LabelViewModel() override;

        int position() const;
        void setPosition(int position);

        QString content() const;
        void setContent(const QString &content);

        bool isSelected() const;
        void setSelected(bool selected);

    Q_SIGNALS:
        void positionChanged();
        void contentChanged();
        void selectedChanged();

    private:
        QString m_content;
        int m_position;
        bool m_selected;
    };

}

#endif //SCOPIC_FLOW_LABELVIEWMODEL_H