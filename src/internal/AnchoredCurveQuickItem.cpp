#include "AnchoredCurveQuickItem_p.h"
#include "AnchoredCurveQuickItem_p_p.h"

#include <QSGFlatColorMaterial>
#include <QSGNode>
#include <QSGVertexColorMaterial>
#include <limits>

#include <ScopicFlow/TimeViewModel.h>
#include <ScopicFlow/TimeLayoutViewModel.h>
#include <ScopicFlow/AnchorViewModel.h>
#include <ScopicFlow/PointSequenceViewModel.h>
#include <ScopicFlow/private/PointSequenceViewModel_p.h>

namespace sflow {

    void AnchoredCurveQuickItemPrivate::handleItemInserted(QObject *item, bool updateDirty) {
        Q_Q(AnchoredCurveQuickItem);
        auto x = item->property("x").value<int>();
        auto y = item->property("y").value<int>();
        auto interpolationMode = ([](ScopicFlow::AnchorType anchorType) -> SVS::AnchoredCurve::Anchor::InterpolationMode {
            switch (anchorType) {
                case ScopicFlow::AT_Break:
                    return SVS::AnchoredCurve::Anchor::Break;
                case ScopicFlow::AT_Free:
                case ScopicFlow::AT_Linear:
                    return SVS::AnchoredCurve::Anchor::Linear;
                case ScopicFlow::AT_Zero:
                    return SVS::AnchoredCurve::Anchor::Zero;
                case ScopicFlow::AT_Pchip:
                    return SVS::AnchoredCurve::Anchor::Pchip;
                default:
                    return SVS::AnchoredCurve::Anchor::Break;
            }
        })(item->property("anchorType").value<ScopicFlow::AnchorType>());
        SVS::AnchoredCurve::Anchor anchor(x, y, interpolationMode);
        curve.addAnchor(anchor);
        if (!updateDirty)
            return;
        calculateDirty(item);
    }
    void AnchoredCurveQuickItemPrivate::handleItemRemoved(QObject *item) {
        auto x = item->property("x").value<int>();
        curve.removeAnchor(x);
        calculateDirty(item);
    }
    void AnchoredCurveQuickItemPrivate::calculateDirty(QObject *item) {
        auto handle = anchoredCurveViewModel->property("handle").value<PointSequenceViewModelQmlHandle *>();
        int left = item->property("position").value<int>();
        if (auto prev1 = handle->previousItem(item); prev1) {
            left = prev1->property("position").value<int>();
            if (auto prev2 = handle->previousItem(prev1); prev2) {
                left = prev2->property("position").value<int>();
            }
        }
        int right = item->property("position").value<int>();
        if (auto next1 = handle->nextItem(item); next1) {
            right = next1->property("position").value<int>();
            if (auto next2 = handle->nextItem(next1); next2) {
                right = next2->property("position").value<int>();
            }
        }
        auto dirtyEnd = qMax(dirtyPosition + dirtyLength, right);
        dirtyPosition = qMin(dirtyPosition, left);
        dirtyLength = dirtyEnd - dirtyPosition;
    }



    AnchoredCurveQuickItem::AnchoredCurveQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new AnchoredCurveQuickItemPrivate) {
        Q_D(AnchoredCurveQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents);
    }
    AnchoredCurveQuickItem::~AnchoredCurveQuickItem() = default;
    TimeViewModel *AnchoredCurveQuickItem::timeViewModel() const {
        Q_D(const AnchoredCurveQuickItem);
        return d->timeViewModel;
    }
    void AnchoredCurveQuickItem::setTimeViewModel(TimeViewModel *timeViewModel) {
        Q_D(AnchoredCurveQuickItem);
        if (d->timeViewModel == timeViewModel)
            return;
        if (d->timeViewModel) {
            disconnect(d->timeViewModel, nullptr, this, nullptr);
        }
        d->timeViewModel = timeViewModel;
        if (timeViewModel) {
            connect(timeViewModel, &TimeViewModel::startChanged, this, [=] {
                d->viewPosition = timeViewModel->start();
                update();
            });
        }
        d->viewPosition = timeViewModel ? timeViewModel->start() : 0;
        emit timeViewModelChanged();
        update();
    }
    TimeLayoutViewModel *AnchoredCurveQuickItem::timeLayoutViewModel() const {
        Q_D(const AnchoredCurveQuickItem);
        return d->timeLayoutViewModel;
    }
    void AnchoredCurveQuickItem::setTimeLayoutViewModel(TimeLayoutViewModel *timeLayoutViewModel) {
        Q_D(AnchoredCurveQuickItem);
        if (d->timeLayoutViewModel == timeLayoutViewModel)
            return;
        if (d->timeLayoutViewModel) {
            disconnect(d->timeLayoutViewModel, nullptr, this, nullptr);
        }
        d->timeLayoutViewModel = timeLayoutViewModel;
        if (timeLayoutViewModel) {
            connect(timeLayoutViewModel, &TimeLayoutViewModel::pixelDensityChanged, this, [=] {
                d->viewLength = width() / timeLayoutViewModel->pixelDensity();
                update();
            });
        }
        d->viewLength = timeLayoutViewModel ? width() / timeLayoutViewModel->pixelDensity() : 0;
        emit timeLayoutViewModelChanged();
        update();
    }
    PointSequenceViewModel *AnchoredCurveQuickItem::anchoredCurveViewModel() const {
        Q_D(const AnchoredCurveQuickItem);
        return d->anchoredCurveViewModel;
    }
    void AnchoredCurveQuickItem::setAnchoredCurveViewModel(PointSequenceViewModel *anchoredCurveViewModel) {
        Q_D(AnchoredCurveQuickItem);
        if (d->anchoredCurveViewModel == anchoredCurveViewModel)
            return;
        if (d->anchoredCurveViewModel) {
            disconnect(d->anchoredCurveViewModel->property("handle").value<SliceableViewModelQmlHandle *>(), nullptr, this, nullptr);
        }
        d->anchoredCurveViewModel = anchoredCurveViewModel;
        d->curve = {};
        d->dirtyPosition = 0;
        d->dirtyLength = std::numeric_limits<int>::max();
        if (anchoredCurveViewModel) {
            auto handle = anchoredCurveViewModel->property("handle").value<PointSequenceViewModelQmlHandle *>();
            connect(handle, &SliceableViewModelQmlHandle::itemInserted, this, [=](QObject *item) {
                d->handleItemInserted(item);
                update();
            });
            connect(handle, &SliceableViewModelQmlHandle::itemAboutToRemove, this, [=](QObject *item) {
                d->handleItemRemoved(item);
                update();
            });
            for (auto item : anchoredCurveViewModel->items()) {
                d->handleItemInserted(item, false);
            }
        }
        emit anchoredCurveViewModelChanged();
        update();

    }
    int AnchoredCurveQuickItem::topValue() const {
        Q_D(const AnchoredCurveQuickItem);
        return d->topValue;
    }
    void AnchoredCurveQuickItem::setTopValue(int topValue) {
        Q_D(AnchoredCurveQuickItem);
        if (d->topValue == topValue)
            return;
        d->topValue = topValue;
        update();
        emit topValueChanged();
    }
    int AnchoredCurveQuickItem::bottomValue() const {
        Q_D(const AnchoredCurveQuickItem);
        return d->bottomValue;
    }
    void AnchoredCurveQuickItem::setBottomValue(int bottomValue) {
        Q_D(AnchoredCurveQuickItem);
        if (d->bottomValue == bottomValue)
            return;
        d->bottomValue = bottomValue;
        update();
        emit bottomValueChanged();
    }
    QList<QColor> AnchoredCurveQuickItem::strokeColors() const {
        Q_D(const AnchoredCurveQuickItem);
        return d->strokeColors;
    }
    void AnchoredCurveQuickItem::setStrokeColors(const QList<QColor> &colors) {
        Q_D(AnchoredCurveQuickItem);
        d->strokeColors = colors;
        emit strokeColorsChanged();
        update();
    }
    QColor AnchoredCurveQuickItem::fillColor() const {
        Q_D(const AnchoredCurveQuickItem);
        return d->fillColor;
    }
    void AnchoredCurveQuickItem::setFillColor(const QColor &color) {
        Q_D(AnchoredCurveQuickItem);
        if (d->fillColor == color)
            return;
        d->fillColor = color;
        emit fillColorChanged();
    }
    class AnchoredCurveSGNode : public QSGNode {
    public:
        QList<QColor> strokeColors;
        QColor fillColor;
    };
    QSGNode *AnchoredCurveQuickItem::updatePaintNode(QSGNode *node_, UpdatePaintNodeData *) {
        Q_D(AnchoredCurveQuickItem);
        auto node = static_cast<AnchoredCurveSGNode *>(node_);
        if (!node) {
            node = new AnchoredCurveSGNode;
            auto scaleNode = new QSGTransformNode;
            scaleNode->setFlag(QSGNode::OwnedByParent);
            node->appendChildNode(scaleNode);
            auto strokeGroupNode = new QSGNode;
            strokeGroupNode->setFlags(QSGNode::OwnedByParent);
            scaleNode->appendChildNode(strokeGroupNode);
            auto fillNode = new QSGGeometryNode;
            fillNode->setFlags(QSGNode::OwnedByParent | QSGNode::OwnsGeometry | QSGNode::OwnsMaterial);
            auto fillMaterial = new QSGFlatColorMaterial;
            fillNode->setMaterial(fillMaterial);
            scaleNode->appendChildNode(fillNode);
        }
        auto scaleNode = static_cast<QSGTransformNode *>(node->childAtIndex(0));
        QMatrix4x4 scaleTransform;
        scaleTransform.translate(-d->viewPosition, d->bottomValue);
        scaleTransform.scale(width() / d->viewLength, -height() / (d->topValue - d->bottomValue));
        scaleNode->setMatrix(scaleTransform);
        auto strokeGroupNode = static_cast<QSGNode *>(scaleNode->childAtIndex(0));
        auto fillNode = static_cast<QSGGeometryNode *>(scaleNode->childAtIndex(1));
        if (d->strokeColors != node->strokeColors) {
            // update all strokes when then stroke color list changes
            d->dirtyPosition = 0;
            d->dirtyLength = std::numeric_limits<int>::max();
        }
        if (d->fillColor != node->fillColor) {
            auto fillMaterial = static_cast<QSGFlatColorMaterial *>(fillNode->material());
            fillMaterial->setColor(d->fillColor);
            node->fillColor = d->fillColor;
        }


        return node;
    }
}

#include "moc_AnchoredCurveQuickItem_p.cpp"