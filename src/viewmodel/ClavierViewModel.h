#ifndef SCOPIC_FLOW_CLAVIERVIEWMODEL_H
#define SCOPIC_FLOW_CLAVIERVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT ClavierViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(double start READ start WRITE setStart NOTIFY startChanged)
        Q_PROPERTY(double pixelDensity READ pixelDensity WRITE setPixelDensity NOTIFY pixelDensityChanged)
        Q_PROPERTY(double maximumPixelDensity READ maximumPixelDensity WRITE setMaximumPixelDensity NOTIFY maximumPixelDensityChanged)
        Q_PROPERTY(double minimumPixelDensity READ minimumPixelDensity WRITE setMinimumPixelDensity NOTIFY minimumPixelDensityChanged)
        Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
        Q_PROPERTY(sflow::ClavierViewModel::AccidentalType accidentalType READ accidentalType WRITE setAccidentalType NOTIFY accidentalTypeChanged)
        Q_PROPERTY(sflow::ClavierViewModel::LabelStrategy labelStrategy READ labelStrategy WRITE setLabelStrategy NOTIFY labelStrategyChanged)
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
        sflow::ClavierViewModel::AccidentalType accidentalType() const;
        void setAccidentalType(AccidentalType accidentalType);

        enum LabelStrategy {
            All,
            C,
            None
        };
        Q_ENUM(LabelStrategy)
        sflow::ClavierViewModel::LabelStrategy labelStrategy() const;
        void setLabelStrategy(LabelStrategy labelStrategy);

    signals:
        void startChanged(double start);
        void pixelDensityChanged(double pixelDensity);
        void maximumPixelDensityChanged(double maximumPixelDensity);
        void minimumPixelDensityChanged(double minimumPixelDensity);
        void cursorPositionChanged(int cursorPosition);
        void accidentalTypeChanged(sflow::ClavierViewModel::AccidentalType accidentalType);
        void labelStrategyChanged(sflow::ClavierViewModel::LabelStrategy labelStrategy);

    private:
        double m_start;
        double m_pixelDensity;
        double m_maximumPixelDensity;
        double m_minimumPixelDensity;
        int m_cursorPosition;
        AccidentalType m_accidentalType;
        LabelStrategy m_labelStrategy;
    };

} // sflow

#endif //SCOPIC_FLOW_CLAVIERVIEWMODEL_H
