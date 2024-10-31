#ifndef SCOPIC_FLOW_TIMEVIEWMODEL_H
#define SCOPIC_FLOW_TIMEVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace SVS {
    class MusicTimeline;
}

namespace sflow {

    class SCOPIC_FLOW_EXPORT TimeViewModel : public QObject {
        Q_OBJECT
    public:
        explicit TimeViewModel(QObject *parent = nullptr);
        ~TimeViewModel() override;

        double start() const;
        void setStart(double start);

        double pixelDensity() const;
        void setPixelDensity(double pixelDensity);

        int primaryPosition() const;
        void setPrimaryPosition(int primaryPosition);

        int secondaryPosition() const;
        void setSecondaryPosition(int secondaryPosition);

        int cursorPosition() const;
        void setCursorPosition(int cursorPosition);

        SVS::MusicTimeline *timeline() const;
        void setTimeline(SVS::MusicTimeline *timeline);

    signals:
        void startChanged(double start);
        void pixelDensityChanged(double pixelDensity);
        void primaryPositionChanged(int primaryPosition);
        void secondaryPositionChanged(int secondaryPosition);
        void cursorPositionChanged(int cursorPosition);
        void timelineChanged();

    private:
        double m_start;
        double m_pixelDensity;
        int m_primaryPosition;
        int m_secondaryPosition;
        int m_cursorPosition;
        SVS::MusicTimeline *m_timeline;
    };

} // sflow

#endif //SCOPIC_FLOW_TIMEVIEWMODEL_H
