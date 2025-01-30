#ifndef SCOPIC_FLOW_NOTEVIEWMODEL_H
#define SCOPIC_FLOW_NOTEVIEWMODEL_H

#include <QQmlPropertyMap>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT NoteViewModel : public QQmlPropertyMap {
        Q_OBJECT
    public:
        explicit NoteViewModel(QObject *parent = nullptr);
        ~NoteViewModel() override;

        int position() const;

        int length() const;

        int key() const;

        QString lyric() const;

        bool selected() const;

        bool intermediate() const;

        bool overlapped() const;

    public slots:
        SFLOW_VIEWMODEL_SET_FUNC void setPosition(int position);
        SFLOW_VIEWMODEL_SET_FUNC void setLength(int length);
        SFLOW_VIEWMODEL_SET_FUNC void setKey(int key);
        SFLOW_VIEWMODEL_SET_FUNC void setLyric(const QString &lyric);
        SFLOW_VIEWMODEL_SET_FUNC void setSelected(bool selected);
        SFLOW_VIEWMODEL_SET_FUNC void setIntermediate(bool intermediate);

    private slots:
        SFLOW_VIEWMODEL_SET_FUNC void setOverlapped(bool overlapped);

    signals:
        SFLOW_VIEWMODEL_SIGNAL void positionChanged(int position);
        SFLOW_VIEWMODEL_SIGNAL void lengthChanged(int length);
        SFLOW_VIEWMODEL_SIGNAL void keyChanged(int key);
        SFLOW_VIEWMODEL_SIGNAL void lyricChanged(const QString &lyric);
        SFLOW_VIEWMODEL_SIGNAL void selectedChanged(bool selected);
        SFLOW_VIEWMODEL_SIGNAL void intermediateChanged(bool intermediate);
    };

} // sflow

#endif //SCOPIC_FLOW_NOTEVIEWMODEL_H
