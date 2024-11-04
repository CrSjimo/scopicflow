#include <QApplication>
#include <ScopicFlow/private/QuickWrapperHelper_p.h>
#include <ScopicFlow/private/ClavierQuickItem_p.h>
#include <ScopicFlow/ClavierViewModel.h>

#include <QMainWindow>

using namespace sflow;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    qmlRegisterType<sflow::ClavierQuickItem>("ScopicFlowPrivate", 1, 0, "Clavier");
    auto [w, quickItem] = QuickWrapperHelper::wrap("Clavier");

    auto clavier = static_cast<ClavierQuickItem *>(quickItem);
    auto viewModel = new ClavierViewModel;
    clavier->setClavierViewModel(viewModel);

    QMainWindow win;
    win.setCentralWidget(w);

    win.show();

    return a.exec();
}