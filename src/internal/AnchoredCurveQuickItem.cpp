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

    void AnchoredCurveQuickItemPrivate::handleItemInserted(QObject *item) {
        Q_Q(AnchoredCurveQuickItem);
        QObject::connect(item, SIGNAL(positionChanged()), q, SLOT(handleItemUpdatedSlot()));
        QObject::connect(item, SIGNAL(anchorValueChanged()), q, SLOT(handleItemUpdatedSlot()));
        QObject::connect(item, SIGNAL(anchorTypeChanged()), q, SLOT(handleItemUpdatedSlot()));
        QObject::connect(item, SIGNAL(styleFlagChanged()), q, SLOT(update()));
        handleItemUpdated(item);
    }
    void AnchoredCurveQuickItemPrivate::handleItemRemoved(QObject *item) {
        if (!itemPositions.contains(item))
            return;
        curve.removeAnchor(itemPositions.value(item));
        positionItems.remove(itemPositions.value(item));
        itemPositions.remove(item);
        curveDirtyFlag = true;
    }
    void AnchoredCurveQuickItemPrivate::handleItemUpdated(QObject *item) {
        if (itemPositions.contains(item)) {
            curve.removeAnchor(itemPositions.value(item));
            positionItems.remove(itemPositions.value(item));
        }
        auto x = item->property("position").value<int>();
        auto y = item->property("anchorValue").value<int>();
        itemPositions.insert(item, x);
        positionItems.insert(x, item);
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
        curveDirtyFlag = true;
    }
    void AnchoredCurveQuickItemPrivate::handleItemUpdatedSlot() {
        Q_Q(AnchoredCurveQuickItem);
        handleItemUpdated(q->sender());
    }



    AnchoredCurveQuickItem::AnchoredCurveQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new AnchoredCurveQuickItemPrivate) {
        Q_D(AnchoredCurveQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents);
        connect(this, &QQuickItem::widthChanged, this, [=] {
            d->viewLength = d->timeLayoutViewModel ? width() / d->timeLayoutViewModel->pixelDensity() : 0;
            update();
        });
        connect(this, &QQuickItem::heightChanged, this, &QQuickItem::update);
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
        d->curveDirtyFlag = true;
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
                d->handleItemInserted(item);
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
    };
    QSGNode *AnchoredCurveQuickItem::updatePaintNode(QSGNode *node_, UpdatePaintNodeData *) {
        Q_D(AnchoredCurveQuickItem);
        auto node = static_cast<AnchoredCurveSGNode *>(node_);
        if (!node) {
            node = new AnchoredCurveSGNode;
            auto scaleNode = new QSGTransformNode;
            scaleNode->setFlag(QSGNode::OwnedByParent);
            node->appendChildNode(scaleNode);
            auto strokeNode = new QSGGeometryNode;
            strokeNode->setFlags(QSGNode::OwnedByParent | QSGNode::OwnsGeometry | QSGNode::OwnsMaterial);
            auto strokeGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 0);
            strokeGeometry->setDrawingMode(QSGGeometry::DrawLines);
            strokeNode->setGeometry(strokeGeometry);
            auto strokeMaterial = new QSGVertexColorMaterial;
            strokeNode->setMaterial(strokeMaterial);
            scaleNode->appendChildNode(strokeNode);
            auto fillNode = new QSGGeometryNode;
            fillNode->setFlags(QSGNode::OwnedByParent | QSGNode::OwnsGeometry | QSGNode::OwnsMaterial);
            auto fillGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
            fillGeometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
            fillNode->setGeometry(fillGeometry);
            auto fillMaterial = new QSGFlatColorMaterial;
            fillNode->setMaterial(fillMaterial);
            scaleNode->appendChildNode(fillNode);
        }
        auto scaleNode = static_cast<QSGTransformNode *>(node->childAtIndex(0));
        auto strokeNode = static_cast<QSGGeometryNode *>(scaleNode->childAtIndex(0));
        auto strokeGeometry = strokeNode->geometry();
        auto fillNode = static_cast<QSGGeometryNode *>(scaleNode->childAtIndex(1));
        auto fillGeometry = fillNode->geometry();
        auto fillMaterial = static_cast<QSGFlatColorMaterial *>(fillNode->material());

        if (d->fillColor != fillMaterial->color()) {
            fillMaterial->setColor(d->fillColor);
            fillNode->markDirty(QSGNode::DirtyMaterial);
        }

        auto ceilWidth = static_cast<int>(std::ceil(width()));
        strokeGeometry->allocate(ceilWidth * 2 - 1);
        fillGeometry->allocate(ceilWidth * 2);
        auto mappedZeroValue = (d->topValue - 0.0) / (d->topValue - d->bottomValue) * height();
        for (int i = 0; i < ceilWidth; i++) {
            auto position = std::round(d->viewPosition + i * d->viewLength / width());
            bool hasValue;
            double prevX = -1;
            auto value = d->curve.value(position, &hasValue, &prevX);
            auto mappedValue = (d->topValue - value) / (d->topValue - d->bottomValue) * height();
            auto item = d->positionItems.value(static_cast<int>(prevX));
            auto color = hasValue && !d->strokeColors.isEmpty() ? d->strokeColors.at(qBound(0, item ? item->property("styleFlag").toInt() : 0, d->strokeColors.size())) : Qt::transparent;
            if (i != 0) {
                auto &p = strokeGeometry->vertexDataAsColoredPoint2D()[2 * i - 1];
                const auto &p2 = strokeGeometry->vertexDataAsColoredPoint2D()[2 * i - 2];
                p.set(i, mappedValue, p2.r, p2.g, p2.b, p2.a);
            }
            strokeGeometry->vertexDataAsColoredPoint2D()[2 * i].set(i, mappedValue, color.red(), color.green(), color.blue(), color.alpha());
            fillGeometry->vertexDataAsPoint2D()[i * 2].set(i, mappedZeroValue);
            fillGeometry->vertexDataAsPoint2D()[i * 2 + 1].set(i, hasValue ? mappedValue : mappedZeroValue);
        }
        strokeNode->markDirty(QSGNode::DirtyGeometry);
        fillNode->markDirty(QSGNode::DirtyGeometry);

        return node;
    }
}

#include "moc_AnchoredCurveQuickItem_p.cpp"