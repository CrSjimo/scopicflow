#ifndef SCOPIC_FLOW_PLAYBACKVIEWMODEL_H
#define SCOPIC_FLOW_PLAYBACKVIEWMODEL_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT PlaybackViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(int primaryPosition READ primaryPosition WRITE setPrimaryPosition NOTIFY primaryPositionChanged)
        Q_PROPERTY(int secondaryPosition READ secondaryPosition WRITE setSecondaryPosition NOTIFY secondaryPositionChanged)
        Q_PROPERTY(int loopStart READ loopStart WRITE setLoopStart NOTIFY loopStartChanged)
        Q_PROPERTY(int loopLength READ loopLength WRITE setLoopLength NOTIFY loopLengthChanged)
    public:
        explicit PlaybackViewModel(QObject *parent = nullptr);
        ~PlaybackViewModel() override;

        int primaryPosition() const;
        void setPrimaryPosition(int primaryPosition);

        int secondaryPosition() const;
        void setSecondaryPosition(int secondaryPosition);

        int loopStart() const;
        void setLoopStart(int loopStart);

        int loopLength() const;
        void setLoopLength(int loopLength);

    Q_SIGNALS:
        void primaryPositionChanged();
        void secondaryPositionChanged();
        void loopStartChanged();
        void loopLengthChanged();

    private:
        int m_primaryPosition;
        int m_secondaryPosition;
        int m_loopStart;
        int m_loopLength;
    };

}

#endif //SCOPIC_FLOW_PLAYBACKVIEWMODEL_H
