#ifndef SCOPIC_FLOW_DYNAMICMIXINGANCHORVIEWMODEL_H
#define SCOPIC_FLOW_DYNAMICMIXINGANCHORVIEWMODEL_H

#include <QList>
#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class DynamicMixingViewModel;
    class DynamicMixingViewModelPrivate;

    class SCOPIC_FLOW_CORE_EXPORT DynamicMixingAnchorViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(QList<double> ratio READ ratio WRITE setRatio NOTIFY ratioChanged)
        Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
        Q_PROPERTY(DynamicMixingViewModel *dynamicMixingViewModel READ dynamicMixingViewModel NOTIFY dynamicMixingViewModelChanged)

    public:
        explicit DynamicMixingAnchorViewModel(QObject *parent = nullptr);
        ~DynamicMixingAnchorViewModel() override;

        int position() const;
        void setPosition(int position);

        QList<double> ratio() const;
        void setRatio(const QList<double> &ratio);

        bool isSelected() const;
        void setSelected(bool selected);

        DynamicMixingViewModel *dynamicMixingViewModel() const;

    Q_SIGNALS:
        void positionChanged();
        void ratioChanged();
        void selectedChanged();
        void dynamicMixingViewModelChanged();

    private:
        friend class DynamicMixingViewModel;
        friend class DynamicMixingViewModelPrivate;

        void setPositionDirect(int position);
        void setDynamicMixingViewModel(DynamicMixingViewModel *viewModel);

        int m_position = 0;
        QList<double> m_ratio;
        bool m_selected = false;
        DynamicMixingViewModel *m_dynamicMixingViewModel = nullptr;
    };

}

#endif // SCOPIC_FLOW_DYNAMICMIXINGANCHORVIEWMODEL_H
