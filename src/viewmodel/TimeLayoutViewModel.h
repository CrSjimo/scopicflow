#ifndef SCOPIC_FLOW_TIMELAYOUTVIEWMODEL_H
#define SCOPIC_FLOW_TIMELAYOUTVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT TimeLayoutViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(double pixelDensity READ pixelDensity WRITE setPixelDensity NOTIFY pixelDensityChanged)
        Q_PROPERTY(double maximumPixelDensity READ maximumPixelDensity WRITE setMaximumPixelDensity NOTIFY maximumPixelDensityChanged)
        Q_PROPERTY(double minimumPixelDensity READ minimumPixelDensity WRITE setMinimumPixelDensity NOTIFY minimumPixelDensityChanged)
        Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
        Q_PROPERTY(int positionAlignment READ positionAlignment WRITE setPositionAlignment NOTIFY positionAlignmentChanged)

    public:
        explicit TimeLayoutViewModel(QObject *parent = nullptr);
        ~TimeLayoutViewModel() override;

        double pixelDensity() const;
        void setPixelDensity(double pixelDensity);

        double maximumPixelDensity() const;
        void setMaximumPixelDensity(double maximumPixelDensity);

        double minimumPixelDensity() const;
        void setMinimumPixelDensity(double minimumPixelDensity);

        int cursorPosition() const;
        void setCursorPosition(int cursorPosition);

        int positionAlignment() const;
        void setPositionAlignment(int positionAlignment);

    signals:
        void pixelDensityChanged(double pixelDensity);
        void maximumPixelDensityChanged(double maximumPixelDensity);
        void minimumPixelDensityChanged(double minimumPixelDensity);
        void cursorPositionChanged(int cursorPosition);
        void positionAlignmentChanged(int positionAlignment);

    private:
        double m_pixelDensity;
        double m_maximumPixelDensity;
        double m_minimumPixelDensity;
        int m_cursorPosition;
        int m_positionAlignment;

    };

}

#endif //SCOPIC_FLOW_TIMELAYOUTVIEWMODEL_H
