#include "LabelSequenceViewModel.h"

#include <algorithm>
#include <iterator>

#include <ScopicFlow/LabelViewModel.h>
#include <ScopicFlow/private/SelectableViewModelManipulator_p.h>

namespace sflow {

    class LabelSequenceViewModelManipulatorInterface : public SelectableViewModelManipulatorInterface {
        Q_OBJECT
    public:
        Q_INVOKABLE explicit LabelSequenceViewModelManipulatorInterface(QObject *viewModel, QObject *parent) : SelectableViewModelManipulatorInterface(parent) {
            m_viewModel = static_cast<LabelSequenceViewModel *>(viewModel);
        }
        void setSelected(QObject *item, bool selected) override {
            static_cast<LabelViewModel *>(item)->setSelected(selected);
        }
        bool isSelected(QObject *item) const override {
            return static_cast<LabelViewModel *>(item)->selected();
        }
        QObject *nextItem(QObject *item) const override {
            return m_viewModel->nextItem(static_cast<LabelViewModel *>(item));
        }
        QObject *previousItem(QObject *item) const override {
            return m_viewModel->previousItem(static_cast<LabelViewModel *>(item));
        }
        QObject *firstItem() const override {
            return m_viewModel->m_labels.cbegin()->second;
        }
        QObject *lastItem() const override {
            return m_viewModel->m_labels.crbegin()->second;
        }
        QObject *currentItem() const override {
            return m_viewModel->m_currentItem;
        }
        void setCurrentItem(QObject *item) override {
            m_viewModel->setCurrentItem(static_cast<LabelViewModel *>(item));
        }
        QObjectList selection() const override {
            QObjectList ret;
            ret.reserve(m_viewModel->m_selection.size());
            std::copy(m_viewModel->m_selection.cbegin(), m_viewModel->m_selection.cend(), std::back_inserter(ret));
            return ret;
        }
        int compareOrder(QObject *item1, QObject *item2) const override {
            return static_cast<LabelViewModel *>(item1)->position() - static_cast<LabelViewModel *>(item2)->position();
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
                SelectableViewModelManipulator::registerViewModelInterface(LabelSequenceViewModel::staticMetaObject.className(), &LabelSequenceViewModelManipulatorInterface::staticMetaObject);
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