#ifndef SCOPIC_FLOW_CLAVIERMANIPULATOR_H
#define SCOPIC_FLOW_CLAVIERMANIPULATOR_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class ClavierViewModel;

    class ClavierManipulatorPrivate;

    class SCOPIC_FLOW_CORE_EXPORT ClavierManipulator : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(ClavierManipulator)
        Q_PROPERTY(ClavierViewModel *clavierViewModel READ clavierViewModel WRITE setClavierViewModel NOTIFY clavierViewModelChanged)
        Q_PROPERTY(QObject *parent READ parent WRITE setParent NOTIFY parentChanged)
        Q_PROPERTY(double viewSize READ viewSize WRITE setViewSize RESET resetViewSize NOTIFY viewSizeChanged)
        Q_PROPERTY(double bottomExpansion READ bottomExpansion WRITE setBottomExpansion NOTIFY bottomExpansionChanged)
        Q_PROPERTY(double topExpansion READ topExpansion WRITE setTopExpansion NOTIFY topExpansionChanged)
    public:
        explicit ClavierManipulator(QObject *parent = nullptr);
        ~ClavierManipulator() override;

        ClavierViewModel *clavierViewModel() const;
        void setClavierViewModel(ClavierViewModel *clavierViewModel);

        QObject *parent() const;
        void setParent(QObject *parent);

        double viewSize() const;
        void setViewSize(double viewSize);
        void resetViewSize();

        double bottomExpansion() const;
        void setBottomExpansion(double bottomExpansion);

        double topExpansion() const;
        void setTopExpansion(double topExpansion);

        Q_INVOKABLE void moveViewBy(double deltaY, bool animated = false);
        Q_INVOKABLE void zoomViewBy(double ratio, double centerY, bool animated = false);

        Q_INVOKABLE int mapToKey(double y) const;

    signals:
        void clavierViewModelChanged();
        void parentChanged();
        void viewSizeChanged();
        void bottomExpansionChanged();
        void topExpansionChanged();

    private:
        QScopedPointer<ClavierManipulatorPrivate> d_ptr;

    };

}

#endif //SCOPIC_FLOW_CLAVIERMANIPULATOR_H
