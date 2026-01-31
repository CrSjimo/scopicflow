#ifndef SCOPIC_FLOW_CLIPVIEWMODEL_H
#define SCOPIC_FLOW_CLIPVIEWMODEL_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QColor>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT ClipViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(QUrl iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
        Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
        Q_PROPERTY(int clipStart READ clipStart WRITE setClipStart NOTIFY clipStartChanged)
        Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength NOTIFY maxLengthChanged)
        Q_PROPERTY(int trackIndex READ trackIndex WRITE setTrackIndex NOTIFY trackIndexChanged)
        Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
        Q_PROPERTY(bool mute READ isMute WRITE setMute NOTIFY muteChanged)
        Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
        Q_PROPERTY(bool overlapped READ isOverlapped WRITE setOverlapped NOTIFY overlappedChanged)
    public:
        explicit ClipViewModel(QObject *parent = nullptr);
        ~ClipViewModel() override;

        QString name() const;
        void setName(const QString &name);

        QUrl iconSource() const;
        void setIconSource(const QUrl &iconSource);

        int position() const;
        void setPosition(int position);

        int length() const;
        void setLength(int length);

        int clipStart() const;
        void setClipStart(int clipStart);

        int maxLength() const;
        void setMaxLength(int maxLength);

        int trackIndex() const;
        void setTrackIndex(int trackIndex);

        QColor color() const;
        void setColor(const QColor &color);

        bool isMute() const;
        void setMute(bool mute);

        bool isSelected() const;
        void setSelected(bool selected);

        bool isOverlapped() const;
        void setOverlapped(bool overlapped);

    Q_SIGNALS:
        void nameChanged();
        void iconSourceChanged();
        void positionChanged();
        void lengthChanged();
        void clipStartChanged();
        void maxLengthChanged();
        void trackIndexChanged();
        void colorChanged();
        void muteChanged();
        void selectedChanged();
        void overlappedChanged();

    private:
        QString m_name;
        QUrl m_iconSource;
        int m_position;
        int m_length;
        int m_clipStart;
        int m_maxLength;
        int m_trackIndex;
        QColor m_color;
        bool m_mute;
        bool m_selected;
        bool m_overlapped;
    };

}

#endif //SCOPIC_FLOW_CLIPVIEWMODEL_H
