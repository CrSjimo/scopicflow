#ifndef SCOPIC_FLOW_PARAMETERRANGESELECTIONVIEWMODEL_H
#define SCOPIC_FLOW_PARAMETERRANGESELECTIONVIEWMODEL_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT ParameterRangeSelectionViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged)
        Q_PROPERTY(int start READ start NOTIFY rangeChanged)
        Q_PROPERTY(int end READ end NOTIFY rangeChanged)

    public:
        explicit ParameterRangeSelectionViewModel(QObject *parent = nullptr);
        ~ParameterRangeSelectionViewModel() override;

        bool hasSelection() const;
        int start() const;
        int end() const;

        Q_INVOKABLE void setRange(int start, int end);
        Q_INVOKABLE void clear();

    Q_SIGNALS:
        void hasSelectionChanged(bool hasSelection);
        void rangeChanged();

    private:
        int m_start = 0;
        int m_end = 0;
        bool m_hasSelection = false;
    };

}

#endif // SCOPIC_FLOW_PARAMETERRANGESELECTIONVIEWMODEL_H
