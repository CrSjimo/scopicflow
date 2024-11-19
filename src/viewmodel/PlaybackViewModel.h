#ifndef SCOPIC_FLOW_PLAYBACKVIEWMODEL_H
#define SCOPIC_FLOW_PLAYBACKVIEWMODEL_H

#include <QObject>
#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT PlaybackViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(int primaryPosition READ primaryPosition WRITE setPrimaryPosition NOTIFY primaryPositionChanged)
        Q_PROPERTY(int secondaryPositon READ secondaryPosition WRITE setSecondaryPosition NOTIFY secondaryPositionChanged)
        Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    public:
        explicit PlaybackViewModel(QObject *parent = nullptr);
        ~PlaybackViewModel() override;

        int primaryPosition() const;
        void setPrimaryPosition(int primaryPosition);

        int secondaryPosition() const;
        void setSecondaryPosition(int secondaryPosition);

        int cursorPosition() const;
        void setCursorPosition(int cursorPosition);

    signals:
        void primaryPositionChanged(int primaryPosition);
        void secondaryPositionChanged(int secondaryPosition);
        void cursorPositionChanged(int cursorPosition);

    private:
        int m_primaryPosition;
        int m_secondaryPosition;
        int m_cursorPosition;
    };

}

#endif //SCOPIC_FLOW_PLAYBACKVIEWMODEL_H
