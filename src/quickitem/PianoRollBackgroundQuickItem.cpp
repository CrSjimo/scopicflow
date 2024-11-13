#include "PianoRollBackgroundQuickItem_p.h"
#include "PianoRollBackgroundQuickItem_p_p.h"

#include <QSGGeometryNode>
#include <QSGSimpleRectNode>

#include <SVSCraftCore/musictimeline.h>

namespace sflow {

    PianoRollBackgroundQuickItem::PianoRollBackgroundQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new PianoRollBackgroundQuickItemPrivate) {
        Q_D(PianoRollBackgroundQuickItem);
        d->q_ptr = this;
    }
    PianoRollBackgroundQuickItem::~PianoRollBackgroundQuickItem() = default;

    TimeAlignmentViewModel *PianoRollBackgroundQuickItem::timeAlignmentViewModel() const {
        Q_D(const PianoRollBackgroundQuickItem);
        return d->timeAlignmentViewModel;
    }
    void PianoRollBackgroundQuickItem::setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel) {
        Q_D(PianoRollBackgroundQuickItem);
        if (d->timeAlignmentViewModel == viewModel) {
            return;
        }
        if (d->timeAlignmentViewModel) {
            disconnect(d->timeAlignmentViewModel, nullptr, this, nullptr);
        }
        if (d->timeline) {
            disconnect(d->timeline, nullptr, this, nullptr);
        }
        d->timeAlignmentViewModel = viewModel;
        d->timeline = nullptr;
        if (viewModel) {
            d->timeline = viewModel->timeline();
            connect(viewModel, &TimeAlignmentViewModel::startChanged, this, &QQuickItem::update);
            connect(viewModel, &TimeAlignmentViewModel::pixelDensityChanged, this, &QQuickItem::update);
            connect(viewModel, &TimeAlignmentViewModel::positionAlignmentChanged, this, &QQuickItem::update);
            connect(viewModel, &TimeAlignmentViewModel::timelineChanged, this, [=] {
                if (d->timeline) {
                    disconnect(d->timeline, nullptr, this, nullptr);
                }
                d->timeline = viewModel->timeline();
                if (d->timeline) {
                    connect(d->timeline, &SVS::MusicTimeline::changed, this, &QQuickItem::update);
                }
            });
            if (d->timeline) {
                connect(d->timeline, &SVS::MusicTimeline::changed, this, &QQuickItem::update);
            }
        }
        update();
    }
    QColor PianoRollBackgroundQuickItem::scaleColor() const {
        Q_D(const PianoRollBackgroundQuickItem);
        return d->scaleColor;
    }
    void PianoRollBackgroundQuickItem::setScaleColor(const QColor &color) {
        Q_D(PianoRollBackgroundQuickItem);
        if (d->scaleColor != color) {
            d->scaleColor = color;
            emit scaleColorChanged(color);
        }
    }
    QColor PianoRollBackgroundQuickItem::barScaleColor() const {
        Q_D(const PianoRollBackgroundQuickItem);
        return d->barScaleColor;
    }
    void PianoRollBackgroundQuickItem::setBarScaleColor(const QColor &color) {
        Q_D(PianoRollBackgroundQuickItem);
        if (d->barScaleColor != color) {
            d->barScaleColor = color;
            emit barScaleColorChanged(color);
        }
    }
    QColor PianoRollBackgroundQuickItem::segmentScaleColor() const {
        Q_D(const PianoRollBackgroundQuickItem);
        return d->segmentScaleColor;
    }
    void PianoRollBackgroundQuickItem::setSegmentScaleColor(const QColor &color) {
        Q_D(PianoRollBackgroundQuickItem);
        if (d->segmentScaleColor != color) {
            d->segmentScaleColor = color;
            emit segmentScaleColorChanged(color);
        }
    }
    QSGNode *PianoRollBackgroundQuickItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *update_paint_node_data) {
        Q_D(PianoRollBackgroundQuickItem);
        if (!d->timeAlignmentViewModel) {
            delete node;
            return nullptr;
        }
        QSGGeometryNode *lineNode;
        if (!node) {
            lineNode = new QSGGeometryNode;
        } else {
            lineNode = static_cast<QSGGeometryNode *>(node);
        }

        enum ScaleType {
            Normal,
            Bar,
            Segment,
        };
        QList<QPair<float, ScaleType>> xList;
        int startTick = static_cast<int>(d->timeAlignmentViewModel->start()) / d->timeAlignmentViewModel->positionAlignment() * d->timeAlignmentViewModel->positionAlignment();
        int endTick = d->timeAlignmentViewModel->start() + width() / d->timeAlignmentViewModel->pixelDensity();

        int deltaTick = d->timeAlignmentViewModel->positionAlignment();
        const int minimumScaleDistance = 4;

        for (int t = startTick; t <= endTick; t++) {

        }



        auto barScaleColor = d->barScaleColor.isValid() ? d->barScaleColor : Qt::black;
        auto scaleColor = d->scaleColor.isValid() ? d->scaleColor : Qt::black;
        auto segmentScaleColor = d->segmentScaleColor.isValid() ? d->segmentScaleColor : Qt::black;


        auto lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), xList.size() * 2);
        lineGeometry->setDrawingMode(QSGGeometry::DrawLines);
        lineGeometry->setLineWidth(1);
        for (int i = 0; i < xList.size(); i++) {
            const auto &[x, isEmphasized] = xList[i];
            lineGeometry->vertexDataAsPoint2D()[i * 2].set(x, height());
            lineGeometry->vertexDataAsPoint2D()[i * 2 + 1].set(x, height() - 32 * (isEmphasized ? 0.5 : 0.25));
        }
        lineNode->setGeometry(lineGeometry);
        lineNode->setFlag(QSGNode::OwnsGeometry);
        auto material = new QSGFlatColorMaterial;
        // material->setColor(foregroundColor);
        lineNode->setMaterial(material);
        lineNode->setFlag(QSGNode::OwnsMaterial);
        lineNode->setFlag(QSGNode::OwnedByParent);

        return lineNode;
    }
    
}