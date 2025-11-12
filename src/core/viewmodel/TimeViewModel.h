#ifndef SCOPIC_FLOW_TIMEVIEWMODEL_H
#define SCOPIC_FLOW_TIMEVIEWMODEL_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace SVS {
    class MusicTimeline;
}

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT TimeViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(double start READ start WRITE setStart NOTIFY startChanged)
        Q_PROPERTY(double end READ end WRITE setEnd NOTIFY endChanged)
        Q_PROPERTY(const SVS::MusicTimeline* timeline READ timeline WRITE setTimeline NOTIFY timelineChanged)
    public:
        explicit TimeViewModel(QObject *parent = nullptr);
        ~TimeViewModel() override;

        double start() const;
        void setStart(double start);

        double end() const;
        void setEnd(double end);

        const SVS::MusicTimeline *timeline() const;
        void setTimeline(const SVS::MusicTimeline *timeline);

    Q_SIGNALS:
        void startChanged();
        void endChanged();
        void timelineChanged();

    private:
        double m_start;
        double m_end;
        const SVS::MusicTimeline *m_timeline;
    };

}

#endif //SCOPIC_FLOW_TIMEVIEWMODEL_H
