#ifndef SCOPIC_FLOW_PARAMETERANCHORVIEWMODEL_H
#define SCOPIC_FLOW_PARAMETERANCHORVIEWMODEL_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class AnchorParameterViewModel;

    class SCOPIC_FLOW_CORE_EXPORT ParameterAnchorViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
        Q_PROPERTY(InterpolationMode interpolationMode READ interpolationMode WRITE setInterpolationMode NOTIFY interpolationModeChanged)
        Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
        Q_PROPERTY(AnchorParameterViewModel *anchorParameterViewModel READ anchorParameterViewModel NOTIFY anchorParameterViewModelChanged)

    public:
        enum InterpolationMode {
            Hermite,
            Linear,
            None,
        };
        Q_ENUM(InterpolationMode)

        explicit ParameterAnchorViewModel(QObject *parent = nullptr);
        ~ParameterAnchorViewModel() override;

        int position() const;
        void setPosition(int position);

        double value() const;
        void setValue(double value);

        InterpolationMode interpolationMode() const;
        void setInterpolationMode(InterpolationMode interpolationMode);

        bool isSelected() const;
        void setSelected(bool selected);

        AnchorParameterViewModel *anchorParameterViewModel() const;

    Q_SIGNALS:
        void positionChanged();
        void valueChanged();
        void interpolationModeChanged();
        void selectedChanged();
        void anchorParameterViewModelChanged();

    private:
        friend class AnchorParameterViewModel;
        friend class AnchorParameterViewModelPrivate;

        void setPositionDirect(int position);
        void setAnchorParameterViewModel(AnchorParameterViewModel *viewModel);

        int m_position = 0;
        double m_value = 0.0;
        InterpolationMode m_interpolationMode = None;
        bool m_selected = false;
        AnchorParameterViewModel *m_anchorParameterViewModel = nullptr;
    };

}

#endif // SCOPIC_FLOW_PARAMETERANCHORVIEWMODEL_H
