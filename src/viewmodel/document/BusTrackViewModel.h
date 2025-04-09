#ifndef SCOPIC_FLOW_BUSTRACKVIEWMODEL_H
#define SCOPIC_FLOW_BUSTRACKVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT BusTrackViewModel : public QQmlPropertyMap {
        Q_OBJECT
        Q_PROPERTY(bool bus READ isBus CONSTANT)

    public:
        explicit BusTrackViewModel(QObject *parent = nullptr);
        ~BusTrackViewModel() override;

        static constexpr bool isBus() { return true; }

        QString name() const;

        bool mute() const;

        QVariant route() const;

        double gain() const;

        double pan() const;

        bool intermediate() const;

        double leftLevel() const;

        double rightLevel() const;

        bool selected() const;

        QColor color() const;

    public slots:
        SFLOW_VIEWMODEL_SET_FUNC void setName(const QString &name);
        SFLOW_VIEWMODEL_SET_FUNC void setMute(bool mute);
        SFLOW_VIEWMODEL_SET_FUNC void setRoute(const QVariant &route);
        SFLOW_VIEWMODEL_SET_FUNC void setGain(double gain);
        SFLOW_VIEWMODEL_SET_FUNC void setPan(double pan);
        SFLOW_VIEWMODEL_SET_FUNC void setIntermediate(bool intermediate);
        SFLOW_VIEWMODEL_SET_FUNC void setLeftLevel(double leftLevel);
        SFLOW_VIEWMODEL_SET_FUNC void setRightLevel(double rightLevel);
        SFLOW_VIEWMODEL_SET_FUNC void setSelected(bool selected);
        SFLOW_VIEWMODEL_SET_FUNC void setColor(const QColor &color);

    signals:
        SFLOW_VIEWMODEL_SIGNAL void nameChanged(const QString &name);
        SFLOW_VIEWMODEL_SIGNAL void muteChanged(bool mute);
        SFLOW_VIEWMODEL_SIGNAL void routeChanged(const QVariant &route);
        SFLOW_VIEWMODEL_SIGNAL void gainChanged(double gain);
        SFLOW_VIEWMODEL_SIGNAL void panChanged(double pan);
        SFLOW_VIEWMODEL_SIGNAL void intermediateChanged(bool intermediate);
        SFLOW_VIEWMODEL_SIGNAL void leftLevelChanged(double leftLevel);
        SFLOW_VIEWMODEL_SIGNAL void rightLevelChanged(double rightLevel);
        SFLOW_VIEWMODEL_SIGNAL void selectedChanged(bool selected);
        SFLOW_VIEWMODEL_SIGNAL void colorChanged(const QColor &color);

    };

}

#endif //SCOPIC_FLOW_BUSTRACKVIEWMODEL_H
