#ifndef SCOPIC_FLOW_TRACKVIEWMODEL_H
#define SCOPIC_FLOW_TRACKVIEWMODEL_H

#include <QObject>
#include <QColor>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT TrackViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(bool mute READ isMute WRITE setMute NOTIFY muteChanged)
        Q_PROPERTY(bool solo READ isSolo WRITE setSolo NOTIFY soloChanged)
        Q_PROPERTY(bool record READ isRecord WRITE setRecord NOTIFY recordChanged)
        Q_PROPERTY(double gain READ gain WRITE setGain NOTIFY gainChanged)
        Q_PROPERTY(double pan READ pan WRITE setPan NOTIFY panChanged)
        Q_PROPERTY(double leftLevel READ leftLevel WRITE setLeftLevel NOTIFY leftLevelChanged)
        Q_PROPERTY(double rightLevel READ rightLevel WRITE setRightLevel NOTIFY rightLevelChanged)
        Q_PROPERTY(bool leftClipping READ isLeftClipping WRITE setLeftClipping NOTIFY leftClippingChanged)
        Q_PROPERTY(bool rightClipping READ isRightClipping WRITE setRightClipping NOTIFY rightClippingChanged)
        Q_PROPERTY(double rowHeight READ rowHeight WRITE setRowHeight NOTIFY rowHeightChanged)
        Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
        Q_PROPERTY(bool multiChannelOutput READ multiChannelOutput WRITE setMultiChannelOutput NOTIFY multiChannelOutputChanged)
        Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
    public:
        explicit TrackViewModel(QObject *parent = nullptr);
        ~TrackViewModel() override;

        QString name() const;
        void setName(const QString &name);

        bool isMute() const;
        void setMute(bool mute);

        bool isSolo() const;
        void setSolo(bool solo);

        bool isRecord() const;
        void setRecord(bool record);

        double gain() const;
        void setGain(double gain);

        double pan() const;
        void setPan(double pan);

        double leftLevel() const;
        void setLeftLevel(double leftLevel);

        double rightLevel() const;
        void setRightLevel(double rightLevel);

        bool isLeftClipping() const;
        void setLeftClipping(bool clipping);

        bool isRightClipping() const;
        void setRightClipping(bool clipping);

        double rowHeight() const;
        void setRowHeight(double rowHeight);

        QColor color() const;
        void setColor(const QColor &color);

        bool multiChannelOutput() const;
        void setMultiChannelOutput(bool enabled);

        bool isRecordEnabled() const;
        void setRecordEnabled(bool enabled);

        bool isSelected() const;
        void setSelected(bool selected);

    Q_SIGNALS:
        void nameChanged();
        void muteChanged();
        void soloChanged();
        void recordChanged();
        void gainChanged();
        void panChanged();
        void leftLevelChanged();
        void rightLevelChanged();
        void leftClippingChanged();
        void rightClippingChanged();
        void rowHeightChanged();
        void colorChanged();
        void multiChannelOutputChanged();
        void recordEnabledChanged();
        void selectedChanged();

    private:
        QString m_name;
        bool m_mute;
        bool m_solo;
        bool m_record;
        double m_gain;
        double m_pan;
        double m_leftLevel;
        double m_rightLevel;
        bool m_leftClipping;
        bool m_rightClipping;
        double m_rowHeight;
        QColor m_color;
        bool m_multiChannelOutput;
        bool m_recordEnabled;
        bool m_selected;
    };

}

#endif //SCOPIC_FLOW_TRACKVIEWMODEL_H
