#include "PianoRollBackgroundQuickItem_p.h"
#include "PianoRollBackgroundQuickItem_p_p.h"

#include <QSGGeometryNode>
#include <QSGVertexColorMaterial>

#include <SVSCraftCore/musictimeline.h>

namespace sflow {

    double PianoRollBackgroundQuickItemPrivate::tickToX(int tick) const {
        if (!timeAlignmentViewModel)
            return 0;
        auto deltaTick = tick - timeAlignmentViewModel->start();
        return deltaTick * timeAlignmentViewModel->pixelDensity();
    }

    PianoRollBackgroundQuickItem::PianoRollBackgroundQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new PianoRollBackgroundQuickItemPrivate) {
        Q_D(PianoRollBackgroundQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents, true);
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
    QColor PianoRollBackgroundQuickItem::beatScaleColor() const {
        Q_D(const PianoRollBackgroundQuickItem);
        return d->beatScaleColor;
    }
    void PianoRollBackgroundQuickItem::setBeatScaleColor(const QColor &color) {
        Q_D(PianoRollBackgroundQuickItem);
        if (d->beatScaleColor != color) {
            d->beatScaleColor = color;
            emit beatScaleColorChanged(color);
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
        if (!d->timeAlignmentViewModel || !d->timeline) {
            delete node;
            return nullptr;
        }
        QSGGeometryNode *barScaleNode;
        if (!node) {
            barScaleNode = new QSGGeometryNode;
        } else {
            barScaleNode = static_cast<QSGGeometryNode *>(node);
        }

        d->xList.clear();

        int startTick = static_cast<int>(d->timeAlignmentViewModel->start()) / d->timeAlignmentViewModel->positionAlignment() * d->timeAlignmentViewModel->positionAlignment();
        int endTick = d->timeAlignmentViewModel->start() + width() / d->timeAlignmentViewModel->pixelDensity();

        int startBar = d->timeline->create(0, 0, startTick).measure();
        int endBar = d->timeline->create(0, 0, endTick).measure() + 1;
        for (int bar = startBar; bar <= endBar; bar++) {
            auto currentTimeSignature = d->timeline->timeSignatureAt(bar);
            int calculatedSegmentRatio = 0;
            static const double minimumScaleDistance = 4;
            int ticksPerBeat = currentTimeSignature.ticksPerBeat(480);
            int ticksPerBar = currentTimeSignature.ticksPerBar(480);
            if (d->timeAlignmentViewModel->positionAlignment() == 1 || ticksPerBeat % d->timeAlignmentViewModel->positionAlignment() != 0) {
                calculatedSegmentRatio = 1;
            } else {
                calculatedSegmentRatio = ticksPerBeat / d->timeAlignmentViewModel->positionAlignment();
            }
            while (calculatedSegmentRatio && ticksPerBeat / calculatedSegmentRatio * d->timeAlignmentViewModel->pixelDensity() < minimumScaleDistance) {
                int nextRatio = calculatedSegmentRatio & -calculatedSegmentRatio;
                if (nextRatio == calculatedSegmentRatio)
                    nextRatio >>= 1;
                calculatedSegmentRatio = nextRatio;
            }
            d->xList.emplace_back(d->tickToX(d->timeline->create(bar, 0, 0).totalTick()), PianoRollBackgroundQuickItemPrivate::Bar);
            if (calculatedSegmentRatio == 0)
                continue;
            for (int tick = ticksPerBeat / calculatedSegmentRatio; tick < ticksPerBar; tick += ticksPerBeat / calculatedSegmentRatio) {
                auto musicTime = d->timeline->create(bar, 0, tick);
                if (musicTime.tick() == 0) {
                    d->xList.emplace_back(d->tickToX(musicTime.totalTick()), PianoRollBackgroundQuickItemPrivate::Beat);
                } else {
                    d->xList.emplace_back(d->tickToX(musicTime.totalTick()), PianoRollBackgroundQuickItemPrivate::Segment);
                }
            }
        }

        auto barScaleColor = d->barScaleColor.isValid() ? d->barScaleColor : Qt::black;
        auto beatScaleColor = d->beatScaleColor.isValid() ? d->beatScaleColor : Qt::black;
        auto segmentScaleColor = d->segmentScaleColor.isValid() ? d->segmentScaleColor : Qt::black;


        auto barScaleGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), d->xList.size() * 2);
        barScaleGeometry->setDrawingMode(QSGGeometry::DrawLines);
        barScaleGeometry->setLineWidth(1);
        for (int i = 0; i < d->xList.size(); i++) {
            const auto &[x, type] = d->xList.at(i);
            auto color = type == PianoRollBackgroundQuickItemPrivate::Bar ? barScaleColor : type == PianoRollBackgroundQuickItemPrivate::Beat ? beatScaleColor : segmentScaleColor;
            barScaleGeometry->vertexDataAsColoredPoint2D()[i * 2].set(x, 0, color.red(), color.green(), color.blue(), 255);
            barScaleGeometry->vertexDataAsColoredPoint2D()[i * 2 + 1].set(x, height(), color.red(), color.green(), color.blue(), 255);
        }
        barScaleNode->setGeometry(barScaleGeometry);
        barScaleNode->setFlag(QSGNode::OwnsGeometry);
        barScaleNode->setMaterial(new QSGVertexColorMaterial);

        return barScaleNode;
    }
    
}