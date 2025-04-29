#include "AnchoredCurveQuickItem_p.h"
#include "AnchoredCurveQuickItem_p_p.h"

#include <QSGFlatColorMaterial>
#include <QSGNode>
#include <QSGVertexColorMaterial>
#include <limits>

#include <ScopicFlow/TimeViewModel.h>
#include <ScopicFlow/TimeLayoutViewModel.h>
#include <ScopicFlow/AnchorViewModel.h>
#include <ScopicFlow/ParameterRangeViewModel.h>
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
        Q_Q(AnchoredCurveQuickItem);
        if (!itemPositions.contains(item))
            return;
        curve.removeAnchor(itemPositions.value(item));
        positionItems.remove(itemPositions.value(item));
        itemPositions.remove(item);
        q->update();
    }
    void AnchoredCurveQuickItemPrivate::handleItemUpdated(QObject *item) {
        Q_Q(AnchoredCurveQuickItem);
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
        q->update();
    }
    void AnchoredCurveQuickItemPrivate::handleItemUpdatedSlot() {
        Q_Q(AnchoredCurveQuickItem);
        handleItemUpdated(q->sender());
    }
    int AnchoredCurveQuickItemPrivate::getPaintPosition(int i) {
        Q_Q(AnchoredCurveQuickItem);
        auto viewPosition = timeViewModel ? timeViewModel->start() : 0;
        auto viewLength = timeLayoutViewModel ? q->width() / timeLayoutViewModel->pixelDensity() : 0;
        auto left = std::round(viewPosition + i * viewLength / q->width());
        if (!handle)
            return left;
        auto right = std::round(viewPosition + (i + 1) * viewLength / q->width());
        auto list = handle->slice(left, right - left);
        if (list.isEmpty())
            return left;
        return list.first()->property("position").toInt();
    }



    AnchoredCurveQuickItem::AnchoredCurveQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new AnchoredCurveQuickItemPrivate) {
        Q_D(AnchoredCurveQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents);
        connect(this, &QQuickItem::widthChanged, this, [=] {
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
            connect(timeViewModel, SIGNAL(startChanged()), this, SLOT(update()));
        }
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
            connect(timeLayoutViewModel, SIGNAL(pixelDensityChanged()), this, SLOT(update()));
        }
        emit timeLayoutViewModelChanged();
        update();
    }
    PointSequenceViewModel *AnchoredCurveQuickItem::anchoredCurveViewModel() const {
        Q_D(const AnchoredCurveQuickItem);
        return d->anchoredCurveViewModel;
    }
    void AnchoredCurveQuickItem::setAnchoredCurveViewModel(
        PointSequenceViewModel *anchoredCurveViewModel) {
        Q_D(AnchoredCurveQuickItem);
        if (d->anchoredCurveViewModel == anchoredCurveViewModel)
            return;
        if (d->anchoredCurveViewModel) {
            disconnect(d->handle, nullptr, this, nullptr);
        }
        d->anchoredCurveViewModel = anchoredCurveViewModel;
        d->handle = nullptr;
        d->curve = {};
        if (anchoredCurveViewModel) {
            d->handle = anchoredCurveViewModel->property("handle")
                            .value<PointSequenceViewModelQmlHandle *>();
            connect(d->handle, &SliceableViewModelQmlHandle::itemInserted, this,
                    [=](QObject *item) {
                        d->handleItemInserted(item);
                        update();
                    });
            connect(d->handle, &SliceableViewModelQmlHandle::itemAboutToRemove, this,
                    [=](QObject *item) {
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
    ParameterRangeViewModel *AnchoredCurveQuickItem::parameterRangeViewModel() const {
        Q_D(const AnchoredCurveQuickItem);
        return d->parameterRangeViewModel;
    }
    void AnchoredCurveQuickItem::setParameterRangeViewModel(ParameterRangeViewModel *parameterRangeViewModel) {
        Q_D(AnchoredCurveQuickItem);
        if (d->parameterRangeViewModel == parameterRangeViewModel)
            return;
        if (d->parameterRangeViewModel) {
            disconnect(d->parameterRangeViewModel, nullptr, this, nullptr);
        }
        d->parameterRangeViewModel = parameterRangeViewModel;
        if (d->parameterRangeViewModel) {
            connect(d->parameterRangeViewModel, SIGNAL(topValueChanged()), this, SLOT(update()));
            connect(d->parameterRangeViewModel, SIGNAL(bottomValueChanged()), this, SLOT(update()));
        }
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
        auto topValue = d->parameterRangeViewModel ? d->parameterRangeViewModel->topValue() : 0;
        auto bottomValue = d->parameterRangeViewModel ? d->parameterRangeViewModel->bottomValue() : 0;
        auto mappedZeroValue = (topValue - 0.0) / (topValue - bottomValue) * height();
        for (int i = 0; i < ceilWidth; i++) {
            auto position = d->getPaintPosition(i);
            bool hasValue;
            double prevX = -1;
            auto value = d->curve.value(position, &hasValue, &prevX);
            auto mappedValue = (topValue - value) / (topValue - bottomValue) * height();
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