#include "LabelSequenceViewModel.h"

#include <algorithm>
#include <iterator>

#include <ScopicFlow/LabelViewModel.h>
#include <ScopicFlow/private/SelectableViewModelManipulatorInterface_p.h>

namespace sflow {

    class LabelSequenceViewModelManipulatorInterface : public SelectableViewModelManipulatorInterface {
        Q_OBJECT
    public:
        Q_INVOKABLE explicit LabelSequenceViewModelManipulatorInterface(QObject *viewModel, QObject *parent) : SelectableViewModelManipulatorInterface(parent) {
            m_viewModel = static_cast<LabelSequenceViewModel *>(viewModel);
        }
        void setSelected(const QVariant &item, bool selected) override {
            item.value<LabelViewModel *>()->setSelected(selected);
        }
        bool isSelected(const QVariant &item) const override {
            return item.value<LabelViewModel *>()->selected();
        }
        QVariant nextItem(const QVariant &item) const override {
            return QVariant::fromValue(m_viewModel->nextItem(item.value<LabelViewModel *>()));
        }
        QVariant previousItem(const QVariant &item) const override {
            return QVariant::fromValue(m_viewModel->previousItem(item.value<LabelViewModel *>()));
        }
        QVariant firstItem() const override {
            return QVariant::fromValue(m_viewModel->m_labels.cbegin()->second);
        }
        QVariant lastItem() const override {
            return QVariant::fromValue(m_viewModel->m_labels.crbegin()->second);
        }
        QVariant currentItem() const override {
            return QVariant::fromValue(m_viewModel->m_currentItem);
        }
        void setCurrentItem(const QVariant &item) override {
            m_viewModel->setCurrentItem(item.value<LabelViewModel *>());
        }
        QVariantList selection() const override {
            QVariantList ret;
            ret.reserve(m_viewModel->m_selection.size());
            std::transform(m_viewModel->m_selection.cbegin(), m_viewModel->m_selection.cend(), std::back_inserter(ret), [](auto item) {
                return QVariant::fromValue(item);
            });
            return ret;
        }
        int compareOrder(const QVariant &item1, const QVariant &item2) const override {
            return item1.value<LabelViewModel *>()->position() - item2.value<LabelViewModel *>()->position();
        }
        bool isValidItem(const QVariant &item) const override {
            return static_cast<bool>(item.value<LabelViewModel *>());
        }
        qsizetype getId(const QVariant &item) const override {
            return reinterpret_cast<qsizetype>(item.value<LabelViewModel *>());
        }
        QVariant fromId(qsizetype id) const override {
            return QVariant::fromValue(reinterpret_cast<LabelViewModel *>(id));
        }
        QObject *viewModel() const override {
            return m_viewModel;
        }

    private:
        LabelSequenceViewModel *m_viewModel;
    };

    namespace {
        struct _ {
            _() {
                SelectableViewModelManipulatorInterface::registerViewModelInterface(LabelSequenceViewModel::staticMetaObject.className(), &LabelSequenceViewModelManipulatorInterface::staticMetaObject);
            }
        } _;
    }

    LabelSequenceViewModel::LabelSequenceViewModel(QObject *parent) : QObject(parent), m_currentItem(nullptr) {
    }
    LabelSequenceViewModel::~LabelSequenceViewModel() = default;

    LabelViewModel *LabelSequenceViewModel::currentItem() const {
        return m_currentItem;
    }
    void LabelSequenceViewModel::setCurrentItem(LabelViewModel *item) {
        if (m_currentItem != item) {
            m_currentItem = item;
            emit currentItemChanged(item);
        }
    }
    int LabelSequenceViewModel::count() const {
        return m_labels.size();
    }
    void LabelSequenceViewModel::insertLabels(const QList<LabelViewModel *> &labels) {
        bool hasSelectedFlag = false;
        for (auto label : labels) {
            m_labels.insert({label->position(), label});
            m_positions.insert(label, label->position());
            if (label->selected()) {
                hasSelectedFlag = true;
                m_selection.insert(label);
            }
            connect(label, &LabelViewModel::positionChanged, this, [=] {
                auto oldPosition = m_positions.value(label);
                m_labels.erase(std::find_if(m_labels.find(oldPosition), m_labels.end(), [=](auto it) {
                    return it.second == label;
                }));
                m_labels.insert({label->position(), label});
                m_positions.insert(label, label->position());
            });
            connect(label, &LabelViewModel::selectedChanged, this, [=] {
                if (label->selected()) {
                    m_selection.insert(label);
                } else {
                    m_selection.remove(label);
                }
                emit selectionChanged();
            });
        }
        if (!labels.isEmpty())
            emit countChanged(m_labels.size());
        if (hasSelectedFlag)
            emit selectionChanged();
    }
    void LabelSequenceViewModel::removeLabels(const QList<LabelViewModel *> &labels) {
        bool hasSelectedFlag = false;
        bool hasCurrentFlag = false;
        for (auto label : labels) {
            m_labels.erase(std::find_if(m_labels.find(label->position()), m_labels.end(), [=](auto it) {
                return it.second == label;
            }));
            m_positions.remove(label);
            if (label->selected()) {
                hasSelectedFlag = true;
                m_selection.remove(label);
            }
            if (label == m_currentItem) {
                hasCurrentFlag = true;
                m_currentItem = nullptr;
            }
            delete label;
        }
        if (!labels.isEmpty())
            emit countChanged(m_labels.size());
        if (hasSelectedFlag)
            emit selectionChanged();
        if (hasCurrentFlag)
            emit currentItemChanged(m_currentItem);
    }
    QList<LabelViewModel *> LabelSequenceViewModel::labels() const {
        QList<LabelViewModel *> ret;
        std::transform(m_labels.begin(), m_labels.end(), std::back_inserter(ret), [=](auto it) {
            return it.second;
        });
        return ret;
    }
    QList<LabelViewModel *> LabelSequenceViewModel::selection() const {
        return m_selection.values();
    }
    LabelViewModel *LabelSequenceViewModel::nextItem(LabelViewModel *item) const {
        auto it = std::find_if(m_labels.find(item->position()), m_labels.end(), [=](auto it) {
            return it.second == item;
        });
        if (it == m_labels.end()) {
            return nullptr;
        }
        if (++it == m_labels.end()) {
            return nullptr;
        }
        return it->second;
    }
    LabelViewModel *LabelSequenceViewModel::previousItem(LabelViewModel *item) const {
        auto it = std::find_if(m_labels.find(item->position()), m_labels.end(), [=](auto it) {
            return it.second == item;
        });
        if (it == m_labels.end()) {
            return nullptr;
        }
        if (it == m_labels.begin()) {
            return nullptr;
        }
        return (--it)->second;
    }
}
#include "LabelSequenceViewModel.moc"