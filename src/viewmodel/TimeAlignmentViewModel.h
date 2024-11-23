#ifndef SCOPIC_FLOW_TIMEALIGNMENTVIEWMODEL_H
#define SCOPIC_FLOW_TIMEALIGNMENTVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/TimeViewModel.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT TimeAlignmentViewModel : public TimeViewModel {
        Q_OBJECT
        Q_PROPERTY(int positionAlignment READ positionAlignment WRITE setPositionAlignment NOTIFY positionAlignmentChanged)
    public:
        explicit TimeAlignmentViewModel(QObject *parent = nullptr);
        ~TimeAlignmentViewModel() override;

        int positionAlignment() const;
        void setPositionAlignment(int positionAlignment);

        int lengthQuantization() const;
        void setLengthQuantization(int lengthQuantization);

        int lengthHint() const;
        void setLengthHint(int lengthHint);

    signals:
        void positionAlignmentChanged(int positionAlignment);
        void lengthQuantizationChanged(int lengthQuantization);
        void lengthHintChanged(int lengthHint);

    private:
        int m_positionAlignment;
        int m_lengthQuantization;
        int m_lengthHint;
    };

}

#endif //SCOPIC_FLOW_TIMEALIGNMENTVIEWMODEL_H
