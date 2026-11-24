#ifndef SCOPIC_FLOW_PHONEMEVIEWMODEL_H
#define SCOPIC_FLOW_PHONEMEVIEWMODEL_H

#include <QMetaObject>
#include <QObject>
#include <QPointer>
#include <QString>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class NoteViewModel;

    class SCOPIC_FLOW_CORE_EXPORT PhonemeViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(double notePosition READ notePosition WRITE setNotePosition NOTIFY notePositionChanged)
        Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
        Q_PROPERTY(PhonemeViewModel *nextPhoneme READ nextPhoneme WRITE setNextPhoneme NOTIFY nextPhonemeChanged)
        Q_PROPERTY(NoteViewModel *associatedNote READ associatedNote WRITE setAssociatedNote NOTIFY associatedNoteChanged)
        Q_PROPERTY(bool isEdited READ isEdited WRITE setEdited NOTIFY editedChanged)
        Q_PROPERTY(double position READ position NOTIFY positionChanged)

    public:
        explicit PhonemeViewModel(QObject *parent = nullptr);
        ~PhonemeViewModel() override;

        double notePosition() const;
        void setNotePosition(double notePosition);

        QString content() const;
        void setContent(const QString &content);

        PhonemeViewModel *nextPhoneme() const;
        void setNextPhoneme(PhonemeViewModel *nextPhoneme);

        NoteViewModel *associatedNote() const;
        void setAssociatedNote(NoteViewModel *associatedNote);

        bool isEdited() const;
        void setEdited(bool edited);

        double position() const;

    Q_SIGNALS:
        void notePositionChanged();
        void contentChanged();
        void nextPhonemeChanged();
        void associatedNoteChanged();
        void editedChanged();
        void positionChanged();

    private:
        double m_notePosition = 0.0;
        QString m_content;
        QPointer<PhonemeViewModel> m_nextPhoneme;
        QPointer<NoteViewModel> m_associatedNote;
        bool m_edited = false;
        QMetaObject::Connection m_nextPhonemeDestroyedConnection;
        QMetaObject::Connection m_associatedNotePositionConnection;
        QMetaObject::Connection m_associatedNoteDestroyedConnection;
    };

}

#endif // SCOPIC_FLOW_PHONEMEVIEWMODEL_H
