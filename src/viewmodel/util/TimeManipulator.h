#ifndef SCOPIC_FLOW_TIMEMANIPULATOR_H
#define SCOPIC_FLOW_TIMEMANIPULATOR_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class TimeViewModel;
    class TimeLayoutViewModel;

    class TimeManipulatorPrivate;

    class SCOPIC_FLOW_EXPORT TimeManipulator : public QObject {
        Q_OBJECT
        Q_DECLARE_PRIVATE(TimeManipulator)
        Q_PROPERTY(TimeViewModel *timeViewModel READ timeViewModel WRITE setTimeViewModel NOTIFY timeViewModelChanged)
        Q_PROPERTY(TimeLayoutViewModel *timeLayoutViewModel READ timeLayoutViewModel WRITE setTimeLayoutViewModel NOTIFY timeLayoutViewModelChanged)
        Q_PROPERTY(double viewSize READ viewSize WRITE setViewSize NOTIFY viewSizeChanged)
    public:
        explicit TimeManipulator(QObject *parent = nullptr);
        ~TimeManipulator() override;

        TimeViewModel *timeViewModel() const;
        void setTimeViewModel(TimeViewModel *timeViewModel);

        TimeLayoutViewModel *timeLayoutViewModel() const;
        void setTimeLayoutViewModel(TimeLayoutViewModel *timeLayoutViewModel);

        double viewSize() const;
        void setViewSize(double viewSize);

        Q_INVOKABLE void moveViewBy(double delta, bool animated = false, bool restrictEnd = false);
        Q_INVOKABLE void zoomViewBy(double ratio, double center, bool animated = false, bool restrictEnd = false);
        Q_INVOKABLE void ensureVisible(int position, int length, double leftPadding = 0, double rightPadding = 0, bool animated = false, bool restrictEnd = false);

    signals:
        void timeViewModelChanged();
        void timeLayoutViewModelChanged();
        void viewSizeChanged();

    private:
        QScopedPointer<TimeManipulatorPrivate> d_ptr;

    };

}

#endif //SCOPIC_FLOW_TIMEMANIPULATOR_H
