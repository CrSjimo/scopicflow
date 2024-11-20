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
        Q_PROPERTY(double start READ start WRITE setStart NOTIFY startChanged)
        Q_PROPERTY(double end READ end WRITE setEnd NOTIFY endChanged)
        Q_PROPERTY(double pixelDensity READ pixelDensity WRITE setPixelDensity NOTIFY pixelDensityChanged)
        Q_PROPERTY(double maximumPixelDensity READ maximumPixelDensity WRITE setMaximumPixelDensity NOTIFY maximumPixelDensityChanged)
        Q_PROPERTY(double minimumPixelDensity READ minimumPixelDensity WRITE setMinimumPixelDensity NOTIFY minimumPixelDensityChanged)
        Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
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
        void cursorPositionChanged(int cursorPosition);
        void timelineChanged();

    private:
        double m_start;
        double m_end;
        double m_pixelDensity;
        double m_maximumPixelDensity;
        double m_minimumPixelDensity;
        int m_cursorPosition;
        SVS::MusicTimeline *m_timeline;
    };

} // sflow

#endif //SCOPIC_FLOW_TIMEVIEWMODEL_H
