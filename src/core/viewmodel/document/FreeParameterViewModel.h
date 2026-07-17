#ifndef SCOPIC_FLOW_FREEPARAMETERVIEWMODEL_H
#define SCOPIC_FLOW_FREEPARAMETERVIEWMODEL_H

#include <QObject>
#include <QList>
#include <QVariant>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT FreeParameterViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(int step READ step CONSTANT)
        Q_PROPERTY(int size READ size NOTIFY sizeChanged)

    public:
        explicit FreeParameterViewModel(QObject *parent = nullptr);
        ~FreeParameterViewModel() override;

        static constexpr int step() { return 5; }

        int size() const;

        Q_INVOKABLE QVariant valueAtIndex(int index) const;
        Q_INVOKABLE QVariant valueAt(double position) const;
        Q_INVOKABLE QList<QVariant> slice(int index, int length) const;

        Q_INVOKABLE bool splice(int index, int removeCount, const QList<QVariant> &values);
        Q_INVOKABLE bool setValues(int index, const QList<QVariant> &values);

    Q_SIGNALS:
        void sizeChanged(int size);
        void valuesChanged(int index, int removedCount, int insertedCount);
        void aboutToSplice(int index, int removeCount, const QList<QVariant> &values);
        void spliced(int index, int removeCount, const QList<QVariant> &values);

    private:
        static bool normalizeValues(const QList<QVariant> &source, QList<QVariant> &target);

        QList<QVariant> m_values;
    };

}

#endif // SCOPIC_FLOW_FREEPARAMETERVIEWMODEL_H
