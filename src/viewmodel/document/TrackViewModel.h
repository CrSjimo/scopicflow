#ifndef SCOPIC_FLOW_TRACKVIEWMODEL_H
#define SCOPIC_FLOW_TRACKVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT TrackViewModel : public QQmlPropertyMap {
        Q_OBJECT

    public:
        explicit TrackViewModel(QObject *parent = nullptr);
        ~TrackViewModel() override;

        QString name() const;

        bool mute() const;

        bool solo() const;

        bool record() const;

        double gain() const;

        double pan() const;

        bool intermediate() const;

        double leftLevel() const;

        double rightLevel() const;

        bool selected() const;

        double rowHeight() const;

        QColor color() const;

    public slots:
        SFLOW_VIEWMODEL_SET_FUNC void setName(const QString &value);
        SFLOW_VIEWMODEL_SET_FUNC void setMute(bool mute);
        SFLOW_VIEWMODEL_SET_FUNC void setSolo(bool solo);
        SFLOW_VIEWMODEL_SET_FUNC void setRecord(bool record);
        SFLOW_VIEWMODEL_SET_FUNC void setGain(double gain);
        SFLOW_VIEWMODEL_SET_FUNC void setPan(double pan);
        SFLOW_VIEWMODEL_SET_FUNC void setIntermediate(bool intermediate);
        SFLOW_VIEWMODEL_SET_FUNC void setLeftLevel(double leftLevel);
        SFLOW_VIEWMODEL_SET_FUNC void setRightLevel(double rightLevel);
        SFLOW_VIEWMODEL_SET_FUNC void setSelected(bool selected);
        SFLOW_VIEWMODEL_SET_FUNC void setRowHeight(double rowHeight);
        SFLOW_VIEWMODEL_SET_FUNC void setColor(const QColor &color);

    signals:
        SFLOW_VIEWMODEL_SIGNAL void nameChanged(const QString &name);
        SFLOW_VIEWMODEL_SIGNAL void muteChanged(bool mute);
        SFLOW_VIEWMODEL_SIGNAL void soloChanged(bool solo);
        SFLOW_VIEWMODEL_SIGNAL void recordChanged(bool record);
        SFLOW_VIEWMODEL_SIGNAL void gainChanged(double gain);
        SFLOW_VIEWMODEL_SIGNAL void panChanged(double pan);
        SFLOW_VIEWMODEL_SIGNAL void intermediateChanged(bool intermediate);
        SFLOW_VIEWMODEL_SIGNAL void selectedChanged(bool selected);
        SFLOW_VIEWMODEL_SIGNAL void rowHeightChanged(double rowHeight);

    };

} // sflow

#endif //SCOPIC_FLOW_TRACKVIEWMODEL_H
