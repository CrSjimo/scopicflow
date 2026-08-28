#ifndef SCOPIC_FLOW_RANGEINDICATORVIEWMODEL_H
#define SCOPIC_FLOW_RANGEINDICATORVIEWMODEL_H

#include <QObject>
#include <QString>
#include <qqmlintegration.h>

#include <SVSCraftCore/SVSCraftNamespace.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT RangeIndicatorViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
        Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
        Q_PROPERTY(SVS::SVSCraft::ControlType type READ type WRITE setType NOTIFY typeChanged)

    public:
        explicit RangeIndicatorViewModel(QObject *parent = nullptr);
        ~RangeIndicatorViewModel() override;

        int position() const;
        void setPosition(int position);

        int length() const;
        void setLength(int length);

        QString content() const;
        void setContent(const QString &content);

        SVS::SVSCraft::ControlType type() const;
        void setType(SVS::SVSCraft::ControlType type);

    Q_SIGNALS:
        void positionChanged();
        void lengthChanged();
        void contentChanged();
        void typeChanged();

    private:
        int m_position{};
        int m_length{};
        QString m_content;
        SVS::SVSCraft::ControlType m_type{SVS::SVSCraft::CT_Normal};
    };

}

#endif // SCOPIC_FLOW_RANGEINDICATORVIEWMODEL_H
