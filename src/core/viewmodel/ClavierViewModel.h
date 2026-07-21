#ifndef SCOPIC_FLOW_CLAVIERVIEWMODEL_H
#define SCOPIC_FLOW_CLAVIERVIEWMODEL_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT ClavierViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(double start READ start WRITE setStart NOTIFY startChanged)
        Q_PROPERTY(double pixelDensity READ pixelDensity WRITE setPixelDensity NOTIFY pixelDensityChanged)
        Q_PROPERTY(double maximumPixelDensity READ maximumPixelDensity WRITE setMaximumPixelDensity NOTIFY maximumPixelDensityChanged)
        Q_PROPERTY(double minimumPixelDensity READ minimumPixelDensity WRITE setMinimumPixelDensity NOTIFY minimumPixelDensityChanged)
        Q_PROPERTY(double cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
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

        double cursorPosition() const;
        void setCursorPosition(double cursorPosition);

    Q_SIGNALS:
        void startChanged();
        void pixelDensityChanged();
        void maximumPixelDensityChanged();
        void minimumPixelDensityChanged();
        void cursorPositionChanged();

    private:
        double m_start;
        double m_pixelDensity;
        double m_maximumPixelDensity;
        double m_minimumPixelDensity;
        double m_cursorPosition;
    };

}

#endif //SCOPIC_FLOW_CLAVIERVIEWMODEL_H
