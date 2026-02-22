#ifndef SCOPIC_FLOW_TIMELAYOUTVIEWMODEL_H
#define SCOPIC_FLOW_TIMELAYOUTVIEWMODEL_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT TimeLayoutViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(double pixelDensity READ pixelDensity WRITE setPixelDensity NOTIFY pixelDensityChanged)
        Q_PROPERTY(double maximumPixelDensity READ maximumPixelDensity WRITE setMaximumPixelDensity NOTIFY maximumPixelDensityChanged)
        Q_PROPERTY(double minimumPixelDensity READ minimumPixelDensity WRITE setMinimumPixelDensity NOTIFY minimumPixelDensityChanged)
        Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
        Q_PROPERTY(int positionAlignment READ positionAlignment WRITE setPositionAlignment NOTIFY positionAlignmentChanged)
        Q_PROPERTY(int displayPositionAlignment READ displayPositionAlignment WRITE setDisplayPositionAlignment RESET resetDisplayPositionAlignment NOTIFY displayPositionAlignmentChanged)
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

        int displayPositionAlignment() const;
        void setDisplayPositionAlignment(int displayPositionAlignment);
        void resetDisplayPositionAlignment();

    Q_SIGNALS:
        void pixelDensityChanged();
        void maximumPixelDensityChanged();
        void minimumPixelDensityChanged();
        void cursorPositionChanged();
        void positionAlignmentChanged();
        void displayPositionAlignmentChanged();

    private:
        double m_pixelDensity;
        double m_maximumPixelDensity;
        double m_minimumPixelDensity;
        int m_cursorPosition;
        int m_positionAlignment;
        int m_displayPositionAlignment;
        bool m_isDisplayPositionExplicitlySet;
    };

}

#endif //SCOPIC_FLOW_TIMELAYOUTVIEWMODEL_H
