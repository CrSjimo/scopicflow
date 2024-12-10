#ifndef SCOPIC_FLOW_TIMEITEMMODEL_P_H
#define SCOPIC_FLOW_TIMEITEMMODEL_P_H

#include <map>

#include <QObject>
#include <QHash>

namespace sflow {

class TimeItemModel : public QObject {
        Q_OBJECT
    public:
        explicit TimeItemModel(QObject *parent = nullptr);
        ~TimeItemModel() override;

        void insertItem(QObject *item, int position);
        void removeItem(QObject *item);

        QObject *nextItem(QObject *item);
        QObject *previousItem(QObject *item);

    private:
        std::multimap<int, QObject *> m_items;
        QHash<QObject *, int> m_positions;
    };
} // sflow

#endif //SCOPIC_FLOW_TIMEITEMMODEL_P_H
