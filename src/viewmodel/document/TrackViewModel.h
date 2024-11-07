#ifndef SCOPIC_FLOW_TRACKVIEWMODEL_H
#define SCOPIC_FLOW_TRACKVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT TrackViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)
        Q_PROPERTY(bool solo READ solo WRITE setSolo NOTIFY soloChanged)
        Q_PROPERTY(bool record READ record WRITE setRecord NOTIFY recordChanged)
        Q_PROPERTY(double gain READ gain WRITE setGain NOTIFY gainChanged)
        Q_PROPERTY(double pan READ pan WRITE setPan NOTIFY panChanged)
        Q_PROPERTY(double leftLevel READ leftLevel WRITE setLeftLevel NOTIFY leftLevelChanged)
        Q_PROPERTY(double rightLevel READ rightLevel WRITE setRightLevel NOTIFY rightLevelChanged)
        Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
        Q_PROPERTY(double rowHeight READ rowHeight WRITE setRowHeight NOTIFY rowHeightChanged)

    public:
        explicit TrackViewModel(QObject *parent = nullptr);
        ~TrackViewModel() override;

        QString name() const;
        void setName(const QString &value);

        bool mute() const;
        void setMute(bool mute);

        bool solo() const;
        void setSolo(bool solo);

        bool record() const;
        void setRecord(bool record);

        double gain() const;
        void setGain(double gain);

        double pan() const;
        void setPan(double pan);

        double leftLevel() const;
        void setLeftLevel(double leftLevel);

        double rightLevel() const;
        void setRightLevel(double rightLevel);

        bool selected() const;
        void setSelected(bool selected);

        double rowHeight() const;
        void setRowHeight(double rowHeight);

    signals:
        void nameChanged(const QString &name);
        void muteChanged(bool mute);
        void soloChanged(bool solo);
        void recordChanged(bool record);
        void gainChanged(double gain);
        void panChanged(double pan);
        void leftLevelChanged(double leftLevel);
        void rightLevelChanged(double rightLevel);
        void selectedChanged(bool selected);
        void rowHeightChanged(double rowHeight);

    private:
        QString m_name;
        bool m_mute;
        bool m_solo;
        bool m_record;
        bool m_selected;
        double m_gain;
        double m_pan;
        double m_leftLevel;
        double m_rightLevel;
        double m_rowHeight;
    };

} // sflow

#endif //SCOPIC_FLOW_TRACKVIEWMODEL_H
