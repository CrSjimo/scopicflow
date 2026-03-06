#ifndef SCOPIC_FLOW_CLIPVIEWMODEL_H
#define SCOPIC_FLOW_CLIPVIEWMODEL_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class RangeSequenceViewModel;

    class SCOPIC_FLOW_CORE_EXPORT ClipViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(QUrl iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
        Q_PROPERTY(RangeSequenceViewModel *associatedNoteSequence READ associatedNoteSequence WRITE setAssociatedNoteSequence NOTIFY associatedNoteSequenceChanged)
        Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
        Q_PROPERTY(int clipStart READ clipStart WRITE setClipStart NOTIFY clipStartChanged)
        Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength NOTIFY maxLengthChanged)
        Q_PROPERTY(int trackIndex READ trackIndex WRITE setTrackIndex NOTIFY trackIndexChanged)
        Q_PROPERTY(bool mute READ isMute WRITE setMute NOTIFY muteChanged)
        Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
        Q_PROPERTY(bool overlapped READ isOverlapped WRITE setOverlapped NOTIFY overlappedChanged)
        Q_PROPERTY(bool editing READ isEditing WRITE setEditing NOTIFY editingChanged)
    public:
        explicit ClipViewModel(QObject *parent = nullptr);
        ~ClipViewModel() override;

        QString name() const;
        void setName(const QString &name);

        QUrl iconSource() const;
        void setIconSource(const QUrl &iconSource);

        RangeSequenceViewModel *associatedNoteSequence() const;
        void setAssociatedNoteSequence(RangeSequenceViewModel *associatedNoteSequence);

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

        bool isMute() const;
        void setMute(bool mute);

        bool isSelected() const;
        void setSelected(bool selected);

        bool isOverlapped() const;
        void setOverlapped(bool overlapped);

        bool isEditing() const;
        void setEditing(bool editing);

    Q_SIGNALS:
        void nameChanged();
        void iconSourceChanged();
        void positionChanged();
        void lengthChanged();
        void clipStartChanged();
        void maxLengthChanged();
        void trackIndexChanged();
        void muteChanged();
        void selectedChanged();
        void overlappedChanged();
        void editingChanged();
        void associatedNoteSequenceChanged();

    private:
        QString m_name;
        QUrl m_iconSource;
        RangeSequenceViewModel *m_associatedNoteSequence;
        int m_position;
        int m_length;
        int m_clipStart;
        int m_maxLength;
        int m_trackIndex;
        bool m_mute;
        bool m_selected;
        bool m_overlapped;
        bool m_editing;
    };

}

#endif //SCOPIC_FLOW_CLIPVIEWMODEL_H
