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
        Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
        Q_PROPERTY(PhonemeViewModel *nextPhoneme READ nextPhoneme WRITE setNextPhoneme NOTIFY nextPhonemeChanged)
        Q_PROPERTY(NoteViewModel *associatedNote READ associatedNote WRITE setAssociatedNote NOTIFY associatedNoteChanged)
        Q_PROPERTY(bool isEdited READ isEdited WRITE setEdited NOTIFY editedChanged)
        Q_PROPERTY(double sequencePosition READ sequencePosition NOTIFY sequencePositionChanged)

    public:
        explicit PhonemeViewModel(QObject *parent = nullptr);
        ~PhonemeViewModel() override;

        double position() const;
        void setPosition(double position);

        QString content() const;
        void setContent(const QString &content);

        PhonemeViewModel *nextPhoneme() const;
        void setNextPhoneme(PhonemeViewModel *nextPhoneme);

        NoteViewModel *associatedNote() const;
        void setAssociatedNote(NoteViewModel *associatedNote);

        bool isEdited() const;
        void setEdited(bool edited);

        // PointSequenceViewModel instances used by PhonemeSequence should index this property.
        double sequencePosition() const;

    Q_SIGNALS:
        void positionChanged();
        void contentChanged();
        void nextPhonemeChanged();
        void associatedNoteChanged();
        void editedChanged();
        void sequencePositionChanged();

    private:
        double m_position = 0.0;
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
