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

        double end() const;
        void setEnd(double end);

        double pixelDensity() const;
        void setPixelDensity(double pixelDensity);

        double maximumPixelDensity() const;
        void setMaximumPixelDensity(double maximumPixelDensity);

        double minimumPixelDensity() const;
        void setMinimumPixelDensity(double minimumPixelDensity);

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
        void endChanged(double end);
        void pixelDensityChanged(double pixelDensity);
        void maximumPixelDensityChanged(double maximumPixelDensity);
        void minimumPixelDensityChanged(double minimumPixelDensity);
        void primaryPositionChanged(int primaryPosition);
        void secondaryPositionChanged(int secondaryPosition);
        void cursorPositionChanged(int cursorPosition);
        void timelineChanged();

    private:
        double m_start;
        double m_end;
        double m_pixelDensity;
        double m_maximumPixelDensity;
        double m_minimumPixelDensity;
        int m_primaryPosition;
        int m_secondaryPosition;
        int m_cursorPosition;
        SVS::MusicTimeline *m_timeline;
    };

} // sflow

#endif //SCOPIC_FLOW_TIMEVIEWMODEL_H
