#ifndef SCOPIC_FLOW_TIMEVIEWMODEL_H
#define SCOPIC_FLOW_TIMEVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace SVS {
    class MusicTimeline;
}

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT TimeViewModel : public QQmlPropertyMap {
        Q_OBJECT
        Q_PROPERTY(double start READ start WRITE setStart NOTIFY startChanged)
        Q_PROPERTY(double end READ end WRITE setEnd NOTIFY endChanged)

    public:
        explicit TimeViewModel(QObject *parent = nullptr);
        ~TimeViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, start, start, setStart, resetStart)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, end, end, setEnd, resetEnd)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(SVS::MusicTimeline *, timeline, timeline, setTimeline, resetTimeline)
    };

} // sflow

#endif //SCOPIC_FLOW_TIMEVIEWMODEL_H
