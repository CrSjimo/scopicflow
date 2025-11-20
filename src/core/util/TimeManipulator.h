#ifndef SCOPIC_FLOW_TIMEMANIPULATOR_H
#define SCOPIC_FLOW_TIMEMANIPULATOR_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>
#include <ScopicFlowCore/ScopicFlowNamespace.h>

class QQuickItem;

namespace sflow {

    class TimeViewModel;
    class TimeLayoutViewModel;

    class TimeManipulatorPrivate;

    class SCOPIC_FLOW_CORE_EXPORT TimeManipulator : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(TimeManipulator)
        Q_PROPERTY(TimeViewModel *timeViewModel READ timeViewModel WRITE setTimeViewModel NOTIFY timeViewModelChanged)
        Q_PROPERTY(TimeLayoutViewModel *timeLayoutViewModel READ timeLayoutViewModel WRITE setTimeLayoutViewModel NOTIFY timeLayoutViewModelChanged)
        Q_PROPERTY(QQuickItem *target READ target WRITE setTarget NOTIFY targetChanged)
        Q_PROPERTY(double viewSize READ viewSize WRITE setViewSize RESET resetViewSize NOTIFY viewSizeChanged)
    public:
        explicit TimeManipulator(QObject *parent = nullptr);
        ~TimeManipulator() override;

        TimeViewModel *timeViewModel() const;
        void setTimeViewModel(TimeViewModel *timeViewModel);

        TimeLayoutViewModel *timeLayoutViewModel() const;
        void setTimeLayoutViewModel(TimeLayoutViewModel *timeLayoutViewModel);

        QQuickItem *target() const;
        void setTarget(QQuickItem *target);

        double viewSize() const;
        void setViewSize(double viewSize);
        void resetViewSize();

        Q_INVOKABLE void moveViewBy(double delta, bool animated = false, bool restrictEnd = false);
        Q_INVOKABLE void zoomViewBy(double ratio, double center, bool animated = false, bool restrictEnd = false);
        Q_INVOKABLE void ensureVisible(int position, int length, double leftPadding = 0, double rightPadding = 0, bool animated = false, bool restrictEnd = false);

        Q_INVOKABLE int alignPosition(int position, ScopicFlow::AlignOption alignOption = ScopicFlow::AO_Round) const;
        Q_INVOKABLE int mapToPosition(double x) const;
        Q_INVOKABLE double mapToX(int position) const;

    signals:
        void timeViewModelChanged();
        void timeLayoutViewModelChanged();
        void targetChanged();
        void viewSizeChanged();

    private:
        QScopedPointer<TimeManipulatorPrivate> d_ptr;

    };

}

#endif //SCOPIC_FLOW_TIMEMANIPULATOR_H
