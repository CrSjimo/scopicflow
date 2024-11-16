#include <QApplication>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QQuickView>
#include <QMainWindow>
#include <QMessageBox>
#include <QMenu>
#include <QCheckBox>
#include <QDoubleSpinBox>

#include <ScopicFlow/TrackListWidget.h>
#include <ScopicFlow/TrackViewModel.h>
#include <ScopicFlow/TrackListViewModel.h>

using namespace sflow;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    auto format = QSurfaceFormat::defaultFormat();
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    auto mainWidget = new QWidget;
    auto mainLayout = new QHBoxLayout;

    auto trackList = new TrackListWidget;
    TrackListViewModel trackListViewModel;

    QList<TrackViewModel *> tracks;
    for (int i = 0; i < 8; i++) {
        auto trackViewModel = new TrackViewModel;
        trackViewModel->setName("test" + QString::number(i));
        tracks.append(trackViewModel);
    }
    trackListViewModel.insertTracks(0, tracks);

    trackList->setTrackListViewModel(&trackListViewModel);

    mainLayout->addWidget(trackList, 1);

    auto formLayout = new QFormLayout;
    auto nameEdit = new QLineEdit;
    formLayout->addRow("Name", nameEdit);
    auto muteCheckbox = new QCheckBox("Mute");
    formLayout->addRow(muteCheckbox);
    auto soloCheckbox = new QCheckBox("Solo");
    formLayout->addRow(soloCheckbox);
    auto recordCheckbox = new QCheckBox("Record");
    formLayout->addRow(recordCheckbox);
    auto gainSpinBox = new QDoubleSpinBox;
    gainSpinBox->setRange(-96, 6);
    gainSpinBox->setDecimals(1);
    formLayout->addRow("Gain", gainSpinBox);
    auto panSpinBox = new QDoubleSpinBox;
    panSpinBox->setRange(-1, 1);
    panSpinBox->setDecimals(2);
    formLayout->addRow(panSpinBox);
    auto intermediateCheckbox = new QCheckBox("Intermediate");
    intermediateCheckbox->setDisabled(true);
    formLayout->addRow(intermediateCheckbox);
    auto leftLevelSpinBox = new QDoubleSpinBox;
    leftLevelSpinBox->setRange(-96, 6);
    formLayout->addRow("Left level", leftLevelSpinBox);
    auto rightLevelSpinBox = new QDoubleSpinBox;
    rightLevelSpinBox->setRange(-96, 6);
    formLayout->addRow("Right level", rightLevelSpinBox);
    auto selectCheckbox = new QCheckBox("Selected");
    formLayout->addRow(selectCheckbox);
    auto rowHeightSpinBox = new QDoubleSpinBox;
    rowHeightSpinBox->setRange(40, 200);
    formLayout->addRow("Height", rowHeightSpinBox);
    mainLayout->addLayout(formLayout);

    auto trackViewModel = trackListViewModel.trackAt(0);
    nameEdit->setText(trackViewModel->name());
    QObject::connect(trackViewModel, &TrackViewModel::nameChanged, nameEdit, &QLineEdit::setText);
    QObject::connect(nameEdit, &QLineEdit::textChanged, trackViewModel, &TrackViewModel::setName);
    muteCheckbox->setChecked(trackViewModel->mute());
    QObject::connect(trackViewModel, &TrackViewModel::muteChanged, muteCheckbox, &QCheckBox::setChecked);
    QObject::connect(muteCheckbox, &QAbstractButton::clicked, trackViewModel, &TrackViewModel::setMute);
    soloCheckbox->setChecked(trackViewModel->solo());
    QObject::connect(trackViewModel, &TrackViewModel::soloChanged, soloCheckbox, &QCheckBox::setChecked);
    QObject::connect(soloCheckbox, &QAbstractButton::clicked, trackViewModel, &TrackViewModel::setSolo);
    recordCheckbox->setChecked(trackViewModel->record());
    QObject::connect(trackViewModel, &TrackViewModel::recordChanged, recordCheckbox, &QCheckBox::setChecked);
    QObject::connect(recordCheckbox, &QAbstractButton::clicked, trackViewModel, &TrackViewModel::setRecord);
    gainSpinBox->setValue(trackViewModel->gain());
    QObject::connect(trackViewModel, &TrackViewModel::gainChanged, gainSpinBox, &QDoubleSpinBox::setValue);
    QObject::connect(gainSpinBox, &QDoubleSpinBox::valueChanged, trackViewModel, &TrackViewModel::setGain);
    panSpinBox->setValue(trackViewModel->pan());
    QObject::connect(trackViewModel, &TrackViewModel::panChanged, panSpinBox, &QDoubleSpinBox::setValue);
    QObject::connect(panSpinBox, &QDoubleSpinBox::valueChanged, trackViewModel, &TrackViewModel::setPan);
    intermediateCheckbox->setChecked(trackViewModel->intermediate());
    QObject::connect(trackViewModel, &TrackViewModel::intermediateChanged, intermediateCheckbox, &QCheckBox::setChecked);
    leftLevelSpinBox->setValue(trackViewModel->leftLevel());
    QObject::connect(leftLevelSpinBox, &QDoubleSpinBox::valueChanged, trackViewModel, &TrackViewModel::setLeftLevel);
    rightLevelSpinBox->setValue(trackViewModel->rightLevel());
    QObject::connect(rightLevelSpinBox, &QDoubleSpinBox::valueChanged, trackViewModel, &TrackViewModel::setRightLevel);
    selectCheckbox->setChecked(trackViewModel->selected());
    QObject::connect(trackViewModel, &TrackViewModel::selectedChanged, selectCheckbox, &QCheckBox::setChecked);
    QObject::connect(selectCheckbox, &QAbstractButton::clicked, trackViewModel, &TrackViewModel::setSelected);
    rowHeightSpinBox->setValue(trackViewModel->rowHeight());
    QObject::connect(trackViewModel, &TrackViewModel::rowHeightChanged, rowHeightSpinBox, &QDoubleSpinBox::setValue);
    QObject::connect(rowHeightSpinBox, &QDoubleSpinBox::valueChanged, trackViewModel, &TrackViewModel::setRowHeight);


    mainWidget->setLayout(mainLayout);
    QMainWindow win;
    win.setCentralWidget(mainWidget);

    win.show();

    QObject::connect(trackList, &TrackListWidget::trackDoubleClicked, [=, &win](int index) {
        QMessageBox::information(&win, {}, "track double-clicked " + QString::number(index));
    });
    QObject::connect(trackList, &TrackListWidget::contextMenuRequestedForTrack, [=, &win](int index) {
        QMenu menu(&win);
        menu.addAction("Menu on Track " + QString::number(index));
        menu.exec(QCursor::pos());
    });
    QObject::connect(trackList, &TrackListWidget::contextMenuRequestedForTrackDragging, [=, &win](int index, int target) {
        QMenu menu(&win);
        menu.addAction("Move to here");
        menu.addAction("Copy to here");
        menu.exec(QCursor::pos());
    });

    return a.exec();
}