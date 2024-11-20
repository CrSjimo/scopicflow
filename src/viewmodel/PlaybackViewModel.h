#ifndef SCOPIC_FLOW_PLAYBACKVIEWMODEL_H
#define SCOPIC_FLOW_PLAYBACKVIEWMODEL_H

#include <QObject>
#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT PlaybackViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(int primaryPosition READ primaryPosition WRITE setPrimaryPosition NOTIFY primaryPositionChanged)
        Q_PROPERTY(int secondaryPositon READ secondaryPosition WRITE setSecondaryPosition NOTIFY secondaryPositionChanged)
    public:
        explicit PlaybackViewModel(QObject *parent = nullptr);
        ~PlaybackViewModel() override;

        int primaryPosition() const;
        void setPrimaryPosition(int primaryPosition);

        int secondaryPosition() const;
        void setSecondaryPosition(int secondaryPosition);

    signals:
        void primaryPositionChanged(int primaryPosition);
        void secondaryPositionChanged(int secondaryPosition);

    private:
        int m_primaryPosition;
        int m_secondaryPosition;
    };

}

#endif //SCOPIC_FLOW_PLAYBACKVIEWMODEL_H
