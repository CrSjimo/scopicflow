#ifndef SCOPIC_FLOW_TIMELAYOUTVIEWMODEL_H
#define SCOPIC_FLOW_TIMELAYOUTVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT TimeLayoutViewModel : public QQmlPropertyMap {
        Q_OBJECT

    public:
        explicit TimeLayoutViewModel(QObject *parent = nullptr);
        ~TimeLayoutViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, pixelDensity, pixelDensity, setPixelDensity, resetPixelDensity)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, maximumPixelDensity, maximumPixelDensity, setMaximumPixelDensity, resetMaximumPixelDensity)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, minimumPixelDensity, minimumPixelDensity, setMinimumPixelDensity, resetMinimumPixelDensity)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, cursorPosition, cursorPosition, setCursorPosition, resetCursorPosition)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, positionAlignment, positionAlignment, setPositionAlignment, resetPositionAlignment)

    };

}

#endif //SCOPIC_FLOW_TIMELAYOUTVIEWMODEL_H
