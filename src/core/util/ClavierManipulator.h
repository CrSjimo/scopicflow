#ifndef SCOPIC_FLOW_CLAVIERMANIPULATOR_H
#define SCOPIC_FLOW_CLAVIERMANIPULATOR_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class ClavierViewModel;

    class ClavierManipulatorPrivate;

    class SCOPIC_FLOW_CORE_EXPORT ClavierManipulator : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(ClavierManipulator)
        Q_PROPERTY(ClavierViewModel *clavierViewModel READ clavierViewModel WRITE setClavierViewModel NOTIFY clavierViewModelChanged)
        Q_PROPERTY(QQuickItem *target READ target WRITE setTarget NOTIFY targetChanged)
        Q_PROPERTY(double viewSize READ viewSize WRITE setViewSize RESET resetViewSize NOTIFY viewSizeChanged)
    public:
        explicit ClavierManipulator(QObject *parent = nullptr);
        ~ClavierManipulator() override;

        ClavierViewModel *clavierViewModel() const;
        void setClavierViewModel(ClavierViewModel *clavierViewModel);

        QQuickItem *target() const;
        void setTarget(QQuickItem *target);

        double viewSize() const;
        void setViewSize(double viewSize);
        void resetViewSize();

        Q_INVOKABLE void moveViewBy(double delta, bool animated = false);
        Q_INVOKABLE void zoomViewBy(double ratio, double center, bool animated = false);
        Q_INVOKABLE void ensureVisible(int position, int length, double topPadding = 0, double bottomPadding = 0, bool animated = false);

        Q_INVOKABLE double mapToPosition(double y) const;
        Q_INVOKABLE double mapToY(int position) const;

    signals:
        void clavierViewModelChanged();
        void targetChanged();
        void viewSizeChanged();

    private:
        QScopedPointer<ClavierManipulatorPrivate> d_ptr;

    };

}

#endif //SCOPIC_FLOW_CLAVIERMANIPULATOR_H
