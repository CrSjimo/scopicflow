#ifndef SCOPIC_FLOW_TIMEVIEWMODEL_H
#define SCOPIC_FLOW_TIMEVIEWMODEL_H

#include <qqmlintegration.h>
#include <QQmlPropertyMap>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace SVS {
    class MusicTimeline;
}

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT TimeViewModel : public QQmlPropertyMap {
        Q_OBJECT
        QML_ELEMENT
    public:
        explicit TimeViewModel(QObject *parent = nullptr);
        ~TimeViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, start, start, setStart, resetStart)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, end, end, setEnd, resetEnd)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(SVS::MusicTimeline *, timeline, timeline, setTimeline, resetTimeline)
    };

}

#endif //SCOPIC_FLOW_TIMEVIEWMODEL_H
