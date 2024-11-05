#ifndef SCOPIC_FLOW_CLAVIERVIEWMODEL_H
#define SCOPIC_FLOW_CLAVIERVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT ClavierViewModel : public QObject {
        Q_OBJECT
    public:
        explicit ClavierViewModel(QObject *parent = nullptr);
        ~ClavierViewModel() override;

        double start() const;
        void setStart(double start);

        double pixelDensity() const;
        void setPixelDensity(double pixelDensity);

        double maximumPixelDensity() const;
        void setMaximumPixelDensity(double maximumPixelDensity);

        double minimumPixelDensity() const;
        void setMinimumPixelDensity(double minimumPixelDensity);

        int cursorPosition() const;
        void setCursorPosition(int cursorPosition);

        enum AccidentalType {
            Flat,
            Sharp,
        };
        Q_ENUM(AccidentalType)
        AccidentalType accidentalType() const;
        void setAccidentalType(AccidentalType accidentalType);

    signals:
        void startChanged(double start);
        void pixelDensityChanged(double pixelDensity);
        void maximumPixelDensityChanged(double maximumPixelDensity);
        void minimumPixelDensityChanged(double minimumPixelDensity);
        void cursorPositionChanged(int cursorPosition);
        void accidentalTypeChanged(AccidentalType accidentalType);

    private:
        double m_start;
        double m_pixelDensity;
        double m_maximumPixelDensity;
        double m_minimumPixelDensity;
        int m_cursorPosition;
        AccidentalType m_accidentalType;
    };

} // sflow

#endif //SCOPIC_FLOW_CLAVIERVIEWMODEL_H
