#include <limits>

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QMainWindow>
#include <QQuickView>
#include <QVBoxLayout>
#include <QQuickWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QMenu>
#include <QDialog>
#include <QPushButton>

#include <SVSCraftCore/musictimeline.h>

#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <ScopicFlow/TimelineWidget.h>

using namespace sflow;

static SVS::MusicTimeSignature promptTimeSignature(QWidget *parent, SVS::MusicTimeSignature initialValue) {
    QDialog dlg(parent);
    auto layout = new QVBoxLayout;
    auto numeratorSpinBox = new QSpinBox;
    numeratorSpinBox->setRange(1, 32);
    layout->addWidget(numeratorSpinBox);
    auto denominatorComboBox = new QComboBox;
    denominatorComboBox->addItems({"1", "2", "4", "8", "16", "32"});
    layout->addWidget(denominatorComboBox);
    auto okButton = new QPushButton("OK");
    okButton->setDefault(true);
    layout->addWidget(okButton);

    dlg.setLayout(layout);

    numeratorSpinBox->setValue(initialValue.numerator());
    denominatorComboBox->setCurrentText(QString::number(initialValue.denominator()));
    QObject::connect(okButton, &QPushButton::clicked, &dlg, &QDialog::accept);

    if (dlg.exec() == QDialog::Accepted) {
        return {numeratorSpinBox->value(), denominatorComboBox->currentText().toInt()};
    } else {
        return {0, 0};
    }


}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QMainWindow win;
    auto mainWidget = new QWidget;
    auto mainLayout = new QVBoxLayout;
    auto timelineWidget = new TimelineWidget;
    mainLayout->addWidget(timelineWidget);

    auto timeViewModel = new TimeAlignmentViewModel;
    timeViewModel->setTimeline(new SVS::MusicTimeline);
    timeViewModel->setPositionAlignment(240);
    timeViewModel->timeline()->setTimeSignature(3, {6, 8});

    timelineWidget->setTimeAlignmentViewModel(timeViewModel);

    mainWidget->setLayout(mainLayout);
    win.setCentralWidget(mainWidget);

    auto formLayout = new QFormLayout;

    auto startSpinBox = new QSpinBox;
    startSpinBox->setMaximum(std::numeric_limits<int>::max());
    formLayout->addRow("Start", startSpinBox);

    auto pixelDensitySpinBox = new QDoubleSpinBox;
    pixelDensitySpinBox->setRange(0.0005, 1);
    pixelDensitySpinBox->setDecimals(4);
    pixelDensitySpinBox->setValue(0.2);
    formLayout->addRow("Pixel density", pixelDensitySpinBox);

    auto primaryPositionSpinBox = new QSpinBox;
    primaryPositionSpinBox->setMaximum(std::numeric_limits<int>::max());
    formLayout->addRow("Primary position", primaryPositionSpinBox);

    auto secondaryPositionSpinBox = new QSpinBox;
    secondaryPositionSpinBox->setMaximum(std::numeric_limits<int>::max());
    formLayout->addRow("Second position", secondaryPositionSpinBox);

    auto positionAlignmentSpinBox = new QSpinBox;
    positionAlignmentSpinBox->setRange(1, std::numeric_limits<int>::max());
    positionAlignmentSpinBox->setValue(timeViewModel->positionAlignment());
    formLayout->addRow("Position alignment", positionAlignmentSpinBox);

    mainLayout->addLayout(formLayout);

    QObject::connect(startSpinBox, &QSpinBox::valueChanged, timeViewModel, &TimeViewModel::setStart);
    QObject::connect(pixelDensitySpinBox, &QDoubleSpinBox::valueChanged, timeViewModel, &TimeViewModel::setPixelDensity);
    QObject::connect(primaryPositionSpinBox, &QSpinBox::valueChanged, timeViewModel, &TimeViewModel::setPrimaryPosition);
    QObject::connect(timeViewModel, &TimeViewModel::primaryPositionChanged, primaryPositionSpinBox, &QSpinBox::setValue);
    QObject::connect(secondaryPositionSpinBox, &QSpinBox::valueChanged, timeViewModel, &TimeViewModel::setSecondaryPosition);
    QObject::connect(timeViewModel, &TimeViewModel::secondaryPositionChanged, secondaryPositionSpinBox, &QSpinBox::setValue);
    QObject::connect(positionAlignmentSpinBox, &QSpinBox::valueChanged, timeViewModel, &TimeAlignmentViewModel::setPositionAlignment);

    QObject::connect(timelineWidget, &TimelineWidget::positionIndicatorDoubleClicked, [=] {
        qDebug() << "Double clicked";
    });

    QObject::connect(timelineWidget, &TimelineWidget::contextMenuRequestedForTimeline, [=, &win](int tick) {
        QMenu menu(&win);
        auto musicTime = timeViewModel->timeline()->create(0, 0, tick);
        menu.addAction(QString("Set time signature at bar %1...").arg(musicTime.measure() + 1), [=, &win] {
            auto timeSignature = promptTimeSignature(&win, timeViewModel->timeline()->timeSignatureAt(musicTime.measure()));
            if (!timeSignature.isValid())
                return;
            timeViewModel->timeline()->setTimeSignature(musicTime.measure(), timeSignature);
        });
        auto removeAction = menu.addAction(QString("Remove time signature at bar %1").arg(musicTime.measure() + 1), [=] {
            timeViewModel->timeline()->removeTimeSignature(musicTime.measure());
        });
        menu.addSeparator();
        menu.addAction(QString("Position to %1").arg(musicTime.toString()), [=] {
            timeViewModel->setPrimaryPosition(tick);
            timeViewModel->setSecondaryPosition(tick);
        });
        menu.addAction(QString("Position to %1 and play").arg(musicTime.toString()), [=] {
            timeViewModel->setPrimaryPosition(tick);
            timeViewModel->setSecondaryPosition(tick);
            qDebug() << "Play";
        });
        removeAction->setEnabled(musicTime.measure() && timeViewModel->timeline()->nearestTimeSignatureTo(musicTime.measure()) == musicTime.measure());
        menu.exec(QCursor::pos());
    });

    QObject::connect(timelineWidget, &TimelineWidget::contextMenuRequestedForPositionIndicator, [=, &win] {
        QMenu menu(&win);
        menu.addAction("Menu on Time Indicator");
        menu.exec(QCursor::pos());
    });


    win.show();

    return a.exec();
}
