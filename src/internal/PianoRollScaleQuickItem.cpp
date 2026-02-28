#include "PianoRollScaleQuickItem_p.h"
#include "PianoRollScaleQuickItem_p_p.h"

#include <QSGGeometryNode>
#include <QSGVertexColorMaterial>

#include <SVSCraftCore/MusicTimeline.h>
#include <SVSCraftCore/MusicTime.h>
#include <SVSCraftCore/MusicTimeSignature.h>

#include <ScopicFlowCore/TimeViewModel.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>

namespace sflow {

    double PianoRollScaleQuickItemPrivate::tickToX(int tick) const {
        if (!timeViewModel || !timeLayoutViewModel)
            return 0;
        auto deltaTick = tick - timeViewModel->start();
        return deltaTick * timeLayoutViewModel->pixelDensity();
    }
    void PianoRollScaleQuickItemPrivate::updateTimeline() {
        Q_Q(PianoRollScaleQuickItem);
        if (timeline) {
            QObject::disconnect(timeline, nullptr, q, nullptr);
        }
        timeline = timeViewModel->timeline();
        if (timeline) {
            QObject::connect(timeline, &SVS::MusicTimeline::changed, q, &QQuickItem::update);
        }
    }

    PianoRollScaleQuickItem::PianoRollScaleQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new PianoRollScaleQuickItemPrivate) {
        Q_D(PianoRollScaleQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents, true);
    }
    PianoRollScaleQuickItem::~PianoRollScaleQuickItem() = default;

    TimeViewModel *PianoRollScaleQuickItem::timeViewModel() const {
        Q_D(const PianoRollScaleQuickItem);
        return d->timeViewModel;
    }
    void PianoRollScaleQuickItem::setTimeViewModel(TimeViewModel *viewModel) {
        Q_D(PianoRollScaleQuickItem);
        if (d->timeViewModel == viewModel) {
            return;
        }
        if (d->timeViewModel) {
            disconnect(d->timeViewModel, nullptr, this, nullptr);
        }
        if (d->timeline) {
            disconnect(d->timeline, nullptr, this, nullptr);
        }
        d->timeViewModel = viewModel;
        d->timeline = nullptr;
        if (viewModel) {
            d->timeline = viewModel->timeline();
            connect(viewModel, SIGNAL(startChanged()), this, SLOT(update()));
            connect(viewModel, SIGNAL(timelineChanged()), this, SLOT(updateTimeline()));
            if (d->timeline) {
                connect(d->timeline, &SVS::MusicTimeline::changed, this, &QQuickItem::update);
            }
        }
        emit timeViewModelChanged();
        update();
    }
    TimeLayoutViewModel *PianoRollScaleQuickItem::timeLayoutViewModel() const {
        Q_D(const PianoRollScaleQuickItem);
        return d->timeLayoutViewModel;
    }
    void PianoRollScaleQuickItem::setTimeLayoutViewModel(TimeLayoutViewModel *viewModel) {
        Q_D(PianoRollScaleQuickItem);
        if (d->timeLayoutViewModel == viewModel) {
            return;
        }
        if (d->timeLayoutViewModel) {
            disconnect(d->timeLayoutViewModel, nullptr, this, nullptr);
        }
        d->timeLayoutViewModel = viewModel;
        if (viewModel) {
            connect(viewModel, SIGNAL(pixelDensityChanged()), this, SLOT(update()));
            connect(viewModel, SIGNAL(displayPositionAlignmentChanged()), this, SLOT(update()));
        }
        emit timeLayoutViewModelChanged();
        update();
    }
    QColor PianoRollScaleQuickItem::beatScaleColor() const {
        Q_D(const PianoRollScaleQuickItem);
        return d->beatScaleColor;
    }
    void PianoRollScaleQuickItem::setBeatScaleColor(const QColor &color) {
        Q_D(PianoRollScaleQuickItem);
        if (d->beatScaleColor != color) {
            d->beatScaleColor = color;
            emit beatScaleColorChanged();
            update();
        }
    }
    QColor PianoRollScaleQuickItem::barScaleColor() const {
        Q_D(const PianoRollScaleQuickItem);
        return d->barScaleColor;
    }
    void PianoRollScaleQuickItem::setBarScaleColor(const QColor &color) {
        Q_D(PianoRollScaleQuickItem);
        if (d->barScaleColor != color) {
            d->barScaleColor = color;
            emit barScaleColorChanged();
            update();
        }
    }
    QColor PianoRollScaleQuickItem::segmentScaleColor() const {
        Q_D(const PianoRollScaleQuickItem);
        return d->segmentScaleColor;
    }
    void PianoRollScaleQuickItem::setSegmentScaleColor(const QColor &color) {
        Q_D(PianoRollScaleQuickItem);
        if (d->segmentScaleColor != color) {
            d->segmentScaleColor = color;
            emit segmentScaleColorChanged();
            update();
        }
    }
    QSGNode *PianoRollScaleQuickItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *update_paint_node_data) {
        Q_D(PianoRollScaleQuickItem);
        if (!d->timeViewModel || !d->timeLayoutViewModel || !d->timeline) {
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

        int startTick = static_cast<int>(d->timeViewModel->start()) / d->timeLayoutViewModel->displayPositionAlignment() * d->timeLayoutViewModel->displayPositionAlignment();
        int endTick = d->timeViewModel->start() + width() / d->timeLayoutViewModel->pixelDensity();

        int startBar = d->timeline->create(0, 0, startTick).measure();
        int endBar = d->timeline->create(0, 0, endTick).measure() + 1;
        for (int bar = startBar; bar <= endBar; bar++) {
            auto currentTimeSignature = d->timeline->timeSignatureAt(bar);
            int calculatedSegmentRatio = 0;
            static const double minimumScaleDistance = 8;
            int ticksPerBeat = currentTimeSignature.ticksPerBeat(d->timeline->ticksPerQuarterNote());
            int ticksPerBar = currentTimeSignature.ticksPerBar(d->timeline->ticksPerQuarterNote());
            if (d->timeLayoutViewModel->displayPositionAlignment() == 1 || ticksPerBeat % d->timeLayoutViewModel->displayPositionAlignment() != 0) {
                calculatedSegmentRatio = 1;
            } else {
                calculatedSegmentRatio = ticksPerBeat / d->timeLayoutViewModel->displayPositionAlignment();
            }
            while (calculatedSegmentRatio && ticksPerBeat / calculatedSegmentRatio * d->timeLayoutViewModel->pixelDensity() < minimumScaleDistance) {
                int nextRatio = calculatedSegmentRatio & -calculatedSegmentRatio;
                if (nextRatio == calculatedSegmentRatio)
                    nextRatio >>= 1;
                calculatedSegmentRatio = nextRatio;
            }
            d->xList.emplace_back(d->tickToX(d->timeline->create(bar, 0, 0).totalTick()), PianoRollScaleQuickItemPrivate::Bar);
            if (calculatedSegmentRatio == 0)
                continue;
            for (int tick = ticksPerBeat / calculatedSegmentRatio; tick < ticksPerBar; tick += ticksPerBeat / calculatedSegmentRatio) {
                auto musicTime = d->timeline->create(bar, 0, tick);
                if (musicTime.tick() == 0) {
                    d->xList.emplace_back(d->tickToX(musicTime.totalTick()), PianoRollScaleQuickItemPrivate::Beat);
                } else {
                    d->xList.emplace_back(d->tickToX(musicTime.totalTick()), PianoRollScaleQuickItemPrivate::Segment);
                }
            }
        }

        auto barScaleColor = d->barScaleColor.isValid() ? d->barScaleColor : Qt::black;
        auto beatScaleColor = d->beatScaleColor.isValid() ? d->beatScaleColor : Qt::black;
        auto segmentScaleColor = d->segmentScaleColor.isValid() ? d->segmentScaleColor : Qt::black;


        auto barScaleGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), d->xList.size() * 4);
        barScaleGeometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
        for (int i = 0; i < d->xList.size(); i++) {
            const auto &[x, type] = d->xList.at(i);
            auto color = type == PianoRollScaleQuickItemPrivate::Bar ? barScaleColor : type == PianoRollScaleQuickItemPrivate::Beat ? beatScaleColor : segmentScaleColor;
            auto y1 = i % 2 ? 0 : height();
            auto y2 = i % 2 ? height() : 0;
            barScaleGeometry->vertexDataAsColoredPoint2D()[i * 4].set(x - 0.5f, y1, color.red(), color.green(), color.blue(), color.alpha());
            barScaleGeometry->vertexDataAsColoredPoint2D()[i * 4 + 1].set(x + 0.5f, y1, color.red(), color.green(), color.blue(), color.alpha());
            barScaleGeometry->vertexDataAsColoredPoint2D()[i * 4 + 2].set(x - 0.5f, y2, color.red(), color.green(), color.blue(), color.alpha());
            barScaleGeometry->vertexDataAsColoredPoint2D()[i * 4 + 3].set(x + 0.5f, y2, color.red(), color.green(), color.blue(), color.alpha());
        }
        barScaleNode->setGeometry(barScaleGeometry);
        barScaleNode->setFlag(QSGNode::OwnsGeometry);
        barScaleNode->setMaterial(new QSGVertexColorMaterial);
        barScaleNode->setFlag(QSGNode::OwnsMaterial);

        return barScaleNode;
    }
    
}

#include "moc_PianoRollScaleQuickItem_p.cpp"
