#include "RubberBandSelector_p.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include <ScopicFlowCore/SelectionController.h>

#include <ScopicFlowInternal/private/RubberBandSelector_p_p.h>

namespace sflow {

    struct RubberBandSelectorItem {
        QObject *item;
        QRectF rect;
        quint64 id;
        std::array<qreal, 4> point{};
        std::size_t indexSlot = 0;
    };

    class RubberBandSelectorIndex {
    public:
        virtual ~RubberBandSelectorIndex() = default;

        virtual void insert(RubberBandSelectorItem *item) = 0;
        virtual void remove(RubberBandSelectorItem *item) = 0;
        virtual void query(const QRectF &rect, std::vector<RubberBandSelectorItem *> &result) const = 0;
    };

    namespace {

        constexpr qsizetype KdLeafCapacity = 16;

        std::pair<qreal, qreal> horizontalInterval(const QRectF &rect) {
            const auto normalized = rect.normalized();
            return {normalized.left(), normalized.right()};
        }

        std::pair<qreal, qreal> verticalInterval(const QRectF &rect) {
            const auto normalized = rect.normalized();
            return {normalized.top(), normalized.bottom()};
        }

        class LinearIntervalIndex final : public RubberBandSelectorIndex {
        public:
            explicit LinearIntervalIndex(Qt::Orientation orientation) : m_orientation(orientation) {
            }

            void insert(RubberBandSelectorItem *item) override {
                item->indexSlot = m_items.size();
                m_items.push_back(item);
            }

            void remove(RubberBandSelectorItem *item) override {
                const auto slot = item->indexSlot;
                if (slot + 1 != m_items.size()) {
                    m_items[slot] = m_items.back();
                    m_items[slot]->indexSlot = slot;
                }
                m_items.pop_back();
            }

            void query(const QRectF &rect, std::vector<RubberBandSelectorItem *> &result) const override {
                const auto [start, end] = interval(rect);
                if (start >= end)
                    return;
                for (auto *item : m_items) {
                    const auto [itemStart, itemEnd] = interval(item->rect);
                    if (itemStart < end && itemEnd > start)
                        result.push_back(item);
                }
            }

        private:
            std::pair<qreal, qreal> interval(const QRectF &rect) const {
                return m_orientation == Qt::Horizontal ? horizontalInterval(rect) : verticalInterval(rect);
            }

            Qt::Orientation m_orientation;
            std::vector<RubberBandSelectorItem *> m_items;
        };

    }

    struct KdNode {
        bool isLeaf = true;
        qsizetype count = 0;
        std::array<qreal, 4> minimum{};
        std::array<qreal, 4> maximum{};
        std::unique_ptr<KdNode> left;
        std::unique_ptr<KdNode> right;
        std::vector<RubberBandSelectorItem *> items;
    };

    namespace {

        class KdIndex final : public RubberBandSelectorIndex {
        public:
            explicit KdIndex(RubberBandSelector::ItemHint itemHint) : m_itemHint(itemHint), m_dimensions(itemHint == RubberBandSelector::None ? 4 : 3) {
            }

            void insert(RubberBandSelectorItem *item) override {
                if (!m_hasCommonExtent && m_itemHint == RubberBandSelector::SameWidth) {
                    m_commonExtent = item->rect.normalized().width();
                    m_hasCommonExtent = true;
                } else if (!m_hasCommonExtent && m_itemHint == RubberBandSelector::SameHeight) {
                    m_commonExtent = item->rect.normalized().height();
                    m_hasCommonExtent = true;
                }
                item->point = makePoint(item->rect);
                item->indexSlot = m_items.size();
                m_items.push_back(item);
                m_dirty = true;
            }

            void remove(RubberBandSelectorItem *item) override {
                const auto slot = item->indexSlot;
                if (slot + 1 != m_items.size()) {
                    m_items[slot] = m_items.back();
                    m_items[slot]->indexSlot = slot;
                }
                m_items.pop_back();
                m_dirty = true;
                if (m_items.empty())
                    m_hasCommonExtent = false;
            }

            void query(const QRectF &rect, std::vector<RubberBandSelectorItem *> &result) const override {
                const auto normalized = rect.normalized();
                if (normalized.width() <= 0 || normalized.height() <= 0)
                    return;
                rebuild();
                if (!m_root)
                    return;
                std::array<qreal, 4> minimum;
                std::array<qreal, 4> maximum;
                queryRange(normalized, minimum, maximum);
                queryNode(m_root.get(), minimum, maximum, normalized, result);
            }

        private:
            using ItemIterator = std::vector<RubberBandSelectorItem *>::iterator;

            std::array<qreal, 4> makePoint(const QRectF &rect) const {
                const auto normalized = rect.normalized();
                if (m_itemHint == RubberBandSelector::SameWidth)
                    return {normalized.left(), normalized.top(), normalized.bottom(), 0};
                if (m_itemHint == RubberBandSelector::SameHeight)
                    return {normalized.left(), normalized.right(), normalized.top(), 0};
                return {normalized.left(), normalized.right(), normalized.top(), normalized.bottom()};
            }

            void queryRange(const QRectF &rect, std::array<qreal, 4> &minimum, std::array<qreal, 4> &maximum) const {
                const auto negativeInfinity = -std::numeric_limits<qreal>::infinity();
                const auto positiveInfinity = std::numeric_limits<qreal>::infinity();
                minimum.fill(negativeInfinity);
                maximum.fill(positiveInfinity);

                const auto normalized = rect.normalized();
                if (m_itemHint == RubberBandSelector::SameWidth) {
                    minimum[0] = normalized.left() - m_commonExtent;
                    maximum[0] = normalized.right();
                    maximum[1] = normalized.bottom();
                    minimum[2] = normalized.top();
                } else if (m_itemHint == RubberBandSelector::SameHeight) {
                    maximum[0] = normalized.right();
                    minimum[1] = normalized.left();
                    minimum[2] = normalized.top() - m_commonExtent;
                    maximum[2] = normalized.bottom();
                } else {
                    maximum[0] = normalized.right();
                    minimum[1] = normalized.left();
                    maximum[2] = normalized.bottom();
                    minimum[3] = normalized.top();
                }
            }

            static bool pointLess(const RubberBandSelectorItem *lhs, const RubberBandSelectorItem *rhs, qsizetype axis) {
                return lhs->point[axis] < rhs->point[axis] || (lhs->point[axis] == rhs->point[axis] && lhs->id < rhs->id);
            }

            std::unique_ptr<KdNode> build(ItemIterator first, ItemIterator last, qsizetype depth) const {
                auto node = std::make_unique<KdNode>();
                node->count = std::distance(first, last);

                if (node->count <= KdLeafCapacity) {
                    node->items.assign(first, last);
                    update(node.get());
                    return node;
                }

                node->isLeaf = false;
                const auto axis = depth % m_dimensions;
                auto middle = first + node->count / 2;
                std::nth_element(first, middle, last, [axis](const auto *lhs, const auto *rhs) { return pointLess(lhs, rhs, axis); });
                node->left = build(first, middle, depth + 1);
                node->right = build(middle, last, depth + 1);
                update(node.get());
                return node;
            }

            void rebuild() const {
                if (!m_dirty)
                    return;
                auto items = m_items;
                if (items.empty())
                    m_root.reset();
                else
                    m_root = build(items.begin(), items.end(), 0);
                m_dirty = false;
            }

            void update(KdNode *node) const {
                const auto positiveInfinity = std::numeric_limits<qreal>::infinity();
                const auto negativeInfinity = -positiveInfinity;
                node->minimum.fill(positiveInfinity);
                node->maximum.fill(negativeInfinity);

                if (node->isLeaf) {
                    node->count = std::ssize(node->items);
                    for (const auto *item : node->items) {
                        for (qsizetype axis = 0; axis < m_dimensions; ++axis) {
                            node->minimum[axis] = std::min(node->minimum[axis], item->point[axis]);
                            node->maximum[axis] = std::max(node->maximum[axis], item->point[axis]);
                        }
                    }
                    return;
                }

                node->count = node->left->count + node->right->count;
                for (qsizetype axis = 0; axis < m_dimensions; ++axis) {
                    node->minimum[axis] = std::min(node->left->minimum[axis], node->right->minimum[axis]);
                    node->maximum[axis] = std::max(node->left->maximum[axis], node->right->maximum[axis]);
                }
            }

            bool pointInRange(const RubberBandSelectorItem *item, const std::array<qreal, 4> &minimum, const std::array<qreal, 4> &maximum) const {
                for (qsizetype axis = 0; axis < m_dimensions; ++axis) {
                    if (item->point[axis] <= minimum[axis] || item->point[axis] >= maximum[axis])
                        return false;
                }
                return true;
            }

            bool nodeIntersectsRange(const KdNode *node, const std::array<qreal, 4> &minimum, const std::array<qreal, 4> &maximum) const {
                if (node->count == 0)
                    return false;
                for (qsizetype axis = 0; axis < m_dimensions; ++axis) {
                    if (node->maximum[axis] <= minimum[axis] || node->minimum[axis] >= maximum[axis])
                        return false;
                }
                return true;
            }

            void queryNode(const KdNode *node, const std::array<qreal, 4> &minimum, const std::array<qreal, 4> &maximum, const QRectF &rect, std::vector<RubberBandSelectorItem *> &result) const {
                if (!nodeIntersectsRange(node, minimum, maximum))
                    return;
                if (node->isLeaf) {
                    for (auto *item : node->items) {
                        if (pointInRange(item, minimum, maximum) && rect.intersects(item->rect.normalized()))
                            result.push_back(item);
                    }
                    return;
                }
                queryNode(node->left.get(), minimum, maximum, rect, result);
                queryNode(node->right.get(), minimum, maximum, rect, result);
            }

            RubberBandSelector::ItemHint m_itemHint;
            qsizetype m_dimensions;
            qreal m_commonExtent = 0;
            bool m_hasCommonExtent = false;
            mutable bool m_dirty = true;
            std::vector<RubberBandSelectorItem *> m_items;
            mutable std::unique_ptr<KdNode> m_root;
        };

        std::unique_ptr<RubberBandSelectorIndex> createIndex(RubberBandSelector::ItemHint itemHint) {
            if (itemHint == RubberBandSelector::IgnoreDimensionX)
                return std::make_unique<LinearIntervalIndex>(Qt::Vertical);
            if (itemHint == RubberBandSelector::IgnoreDimensionY)
                return std::make_unique<LinearIntervalIndex>(Qt::Horizontal);
            return std::make_unique<KdIndex>(itemHint);
        }

    }

    RubberBandSelectorPrivate::RubberBandSelectorPrivate() = default;

    RubberBandSelectorPrivate::~RubberBandSelectorPrivate() = default;

    RubberBandSelectorIndex *RubberBandSelectorPrivate::ensureIndex() {
        if (!index)
            index = createIndex(itemHint);
        return index.get();
    }

    RubberBandSelector::RubberBandSelector(QObject *parent) : QObject(parent), d_ptr(new RubberBandSelectorPrivate) {
        Q_D(RubberBandSelector);
        d->q_ptr = this;
    }

    RubberBandSelector::~RubberBandSelector() = default;

    SelectionController *RubberBandSelector::selectionController() const {
        Q_D(const RubberBandSelector);
        return d->selectionController;
    }

    void RubberBandSelector::setSelectionController(SelectionController *selectionController) {
        Q_D(RubberBandSelector);
        d->selectionController = selectionController;
        Q_EMIT selectionControllerChanged();
    }

    RubberBandSelector::ItemHint RubberBandSelector::itemHint() const {
        Q_D(const RubberBandSelector);
        return d->itemHint;
    }

    void RubberBandSelector::setItemHint(ItemHint itemHint) {
        Q_D(RubberBandSelector);
        if (d->itemHint == itemHint)
            return;
        d->itemHint = itemHint;
        d->index.reset();
        Q_EMIT itemHintChanged();
    }

    void RubberBandSelector::insertItem(QObject *item, const QRectF &rect) {
        Q_D(RubberBandSelector);
        if (!item)
            return;

        const auto existing = d->items.find(item);
        if (existing != d->items.end()) {
            d->ensureIndex()->remove(existing->second.get());
            existing->second->rect = rect;
            d->ensureIndex()->insert(existing->second.get());
            return;
        }

        auto entry = std::make_unique<RubberBandSelectorItem>();
        entry->item = item;
        entry->rect = rect;
        entry->id = ++d->nextItemId;
        auto *entryPointer = entry.get();
        d->items.emplace(item, std::move(entry));
        d->ensureIndex()->insert(entryPointer);
        connect(item, &QObject::destroyed, this, [this, item] { removeItem(item); });
    }

    void RubberBandSelector::removeItem(QObject *item) {
        Q_D(RubberBandSelector);
        if (!item)
            return;
        const auto it = d->items.find(item);
        if (it == d->items.end())
            return;
        d->ensureIndex()->remove(it->second.get());
        d->items.erase(it);
        disconnect(item, nullptr, this, nullptr);
    }

    void RubberBandSelector::select(const QRectF &rect) {
        Q_D(RubberBandSelector);
        if (!d->selectionController)
            return;
        Q_EMIT selectionAboutToEnd(rect);
        std::vector<RubberBandSelectorItem *> selectedItems;
        d->ensureIndex()->query(rect, selectedItems);
        for (const auto *item : selectedItems)
            d->selectionController->select(item->item, SelectionController::Toggle);
    }

}

#include "moc_RubberBandSelector_p.cpp"
