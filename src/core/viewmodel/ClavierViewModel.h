#ifndef SCOPIC_FLOW_CLAVIERVIEWMODEL_H
#define SCOPIC_FLOW_CLAVIERVIEWMODEL_H

#include <QQmlPropertyMap>

#include <SVSCraftCore/MusicPitch.h>

#include <ScopicFlowCore/ScopicFlowNamespace.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT ClavierViewModel : public QQmlPropertyMap {
        Q_OBJECT

    public:
        explicit ClavierViewModel(QObject *parent = nullptr);
        ~ClavierViewModel() override;

        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, start, start, setStart, resetStart)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, pixelDensity, pixelDensity, setPixelDensity, resetPixelDensity)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, maximumPixelDensity, maximumPixelDensity, setMaximumPixelDensity, resetMaximumPixelDensity)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(double, minimumPixelDensity, minimumPixelDensity, setMinimumPixelDensity, resetMinimumPixelDensity)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(int, cursorPosition, cursorPosition, setCursorPosition, resetCursorPosition)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(SVS::MusicPitch::Accidental, accidentalType, accidentalType, setAccidentalType, resetAccidentalType)
        SFLOW_VIEWMODEL_DECLARE_PROPERTY(ScopicFlow::LabelStrategy, labelStrategy, labelStrategy, setLabelStrategy, resetLabelStrategy)
    };

} // sflow

#endif //SCOPIC_FLOW_CLAVIERVIEWMODEL_H
