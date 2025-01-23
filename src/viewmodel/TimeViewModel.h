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

    public:
        explicit TimeViewModel(QObject *parent = nullptr);
        ~TimeViewModel() override;

        double start() const;
        void setStart(double start);

        double end() const;
        void setEnd(double end);

        SVS::MusicTimeline *timeline() const;
        void setTimeline(SVS::MusicTimeline *timeline);

    signals:
        void startChanged(double start);
        void endChanged(double end);
        void timelineChanged();

    private:
        double m_start;
        double m_end;
        SVS::MusicTimeline *m_timeline;
    };

} // sflow

#endif //SCOPIC_FLOW_TIMEVIEWMODEL_H
