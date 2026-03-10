#ifndef SCOPIC_FLOW_NOTEVIEWMODEL_H
#define SCOPIC_FLOW_NOTEVIEWMODEL_H

#include <QObject>
#include <QString>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT NoteViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QString lyric READ lyric WRITE setLyric NOTIFY lyricChanged)
        Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
        Q_PROPERTY(int key READ key WRITE setKey NOTIFY keyChanged)
        Q_PROPERTY(QString additionalText READ additionalText WRITE setAdditionalText NOTIFY additionalTextChanged)
        Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
        Q_PROPERTY(bool overlapped READ isOverlapped WRITE setOverlapped NOTIFY overlappedChanged)
        Q_PROPERTY(bool additionalTextHighlighted READ isAdditionalTextHighlighted WRITE setAdditionalTextHighlighted NOTIFY additionalTextHighlightedChanged)
        Q_PROPERTY(int nextNotePosition READ nextNotePosition WRITE setNextNotePosition NOTIFY nextNotePositionChanged)
        Q_PROPERTY(int nextNoteKey READ nextNoteKey WRITE setNextNoteKey NOTIFY nextNoteKeyChanged)
    public:
        explicit NoteViewModel(QObject *parent = nullptr);
        ~NoteViewModel() override;

        QString lyric() const;
        void setLyric(const QString &lyric);

        int position() const;
        void setPosition(int position);

        int length() const;
        void setLength(int length);

        int key() const;
        void setKey(int key);

        QString additionalText() const;
        void setAdditionalText(const QString &additionalText);

        bool isAdditionalTextHighlighted() const;
        void setAdditionalTextHighlighted(bool additionalTextHighlighted);

        bool isSelected() const;
        void setSelected(bool selected);

        bool isOverlapped() const;
        void setOverlapped(bool overlapped);

        int nextNotePosition() const;
        void setNextNotePosition(int nextNotePosition);

        int nextNoteKey() const;
        void setNextNoteKey(int nextNoteKey);

    Q_SIGNALS:
        void lyricChanged();
        void positionChanged();
        void lengthChanged();
        void keyChanged();
        void additionalTextChanged();
        void additionalTextHighlightedChanged();
        void selectedChanged();
        void overlappedChanged();
        void nextNotePositionChanged();
        void nextNoteKeyChanged();

    private:
        QString m_lyric;
        int m_position;
        int m_length;
        int m_key;
        QString m_additionalText;
        bool m_additionalTextHighlighted;
        bool m_selected;
        bool m_overlapped;
        int m_nextNotePosition;
        int m_nextNoteKey;
    };

}

#endif //SCOPIC_FLOW_NOTEVIEWMODEL_H
