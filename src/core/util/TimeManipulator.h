#ifndef SCOPIC_FLOW_TIMEMANIPULATOR_H
#define SCOPIC_FLOW_TIMEMANIPULATOR_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>
#include <ScopicFlowCore/ScopicFlowNamespace.h>

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
        Q_PROPERTY(QObject *parent READ parent WRITE setParent NOTIFY parentChanged)
        Q_PROPERTY(double viewSize READ viewSize WRITE setViewSize RESET resetViewSize NOTIFY viewSizeChanged)
    public:
        explicit TimeManipulator(QObject *parent = nullptr);
        ~TimeManipulator() override;

        TimeViewModel *timeViewModel() const;
        void setTimeViewModel(TimeViewModel *timeViewModel);

        TimeLayoutViewModel *timeLayoutViewModel() const;
        void setTimeLayoutViewModel(TimeLayoutViewModel *timeLayoutViewModel);

        QObject *parent() const;
        void setParent(QObject *parent);

        double viewSize() const;
        void setViewSize(double viewSize);
        void resetViewSize();

        Q_INVOKABLE void moveViewBy(double delta, bool animated = false, bool restrictEnd = false);
        Q_INVOKABLE void zoomViewBy(double ratio, double center, bool animated = false, bool restrictEnd = false);
        Q_INVOKABLE void ensureVisible(int position, int length, double leftPadding = 0, double rightPadding = 0, bool animated = false, bool restrictEnd = false);

        Q_INVOKABLE int alignTick(int tick, ScopicFlow::AlignOption alignOption = ScopicFlow::AO_Round) const;
        Q_INVOKABLE int mapToTick(double position) const;
        Q_INVOKABLE double mapToPosition(int tick) const;

    signals:
        void timeViewModelChanged();
        void timeLayoutViewModelChanged();
        void parentChanged();
        void viewSizeChanged();

    private:
        QScopedPointer<TimeManipulatorPrivate> d_ptr;

    };

}

#endif //SCOPIC_FLOW_TIMEMANIPULATOR_H
