#ifndef SCOPIC_FLOW_LABELVIEWMODEL_H
#define SCOPIC_FLOW_LABELVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT LabelViewModel : public QQmlPropertyMap {
        Q_OBJECT
    public:
        explicit LabelViewModel(QObject *parent = nullptr);
        ~LabelViewModel() override;

        int position() const;
        QString content() const;
        bool selected() const;
        bool intermediate() const;

    public slots:
        SFLOW_VIEWMODEL_SET_FUNC void setPosition(int position);
        SFLOW_VIEWMODEL_SET_FUNC void setContent(const QString &content);
        SFLOW_VIEWMODEL_SET_FUNC void setSelected(bool selected);
        SFLOW_VIEWMODEL_SET_FUNC void setIntermediate(bool intermediate);

    signals:
        SFLOW_VIEWMODEL_SIGNAL void positionChanged(int position);
        SFLOW_VIEWMODEL_SIGNAL void contentChanged(const QString &content);
        SFLOW_VIEWMODEL_SIGNAL void selectedChanged(bool selected);
        SFLOW_VIEWMODEL_SIGNAL void intermediateChanged(bool intermediate);

    };

} // sflow

#endif //SCOPIC_FLOW_LABELVIEWMODEL_H
