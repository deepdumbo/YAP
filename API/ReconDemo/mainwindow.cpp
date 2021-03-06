#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <direct.h>

#include "Implement/LogImpl.h"
#include "Implement/LogUserImpl.h"
#include "API/Yap/ModuleManager.h"

#include "datamanager.h"
#include "Processors/NiuMriDisplay1D.h"
#include "Processors/NiuMriDisplay2D.h"
#include "Processors/RecieveData.h"
#include "Processors/NiumagSliceIterator.h"
#include "DataSample/reconserver.h"
#include "imagelayoutwidget.h"
#include "Interface/Interfaces.h"
#include "Implement/ProcessorImpl.h"
#include "Client/DataHelper.h"
#include "datamanager.h"
#include <QFileDialog>

#include "DataSample/SampleDataProtocol.h"

#include <complex>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    SetImportedProcessors();

    DataManager::GetHandle().Demo1D();

    on_buttonStart_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::SetImportedProcessors()
{
    Yap::ImportedProcessorManager::RegisterProcessor(L"RecieveData", new RecieveData());
    Yap::ImportedProcessorManager::RegisterProcessor(L"NiuMriDisplay1D", new NiuMriDisplay1D(*ui->plot1d_widget));
    Yap::ImportedProcessorManager::RegisterProcessor(L"NiuMriDisplay2D", new NiuMriDisplay2D(*ui->imagelayout_widget));
    Yap::ImportedProcessorManager::RegisterProcessor(L"NiumagSliceIterator", new NiumagSliceIterator());
}




void MainWindow::on_actionOpen_triggered()
{

    bool demo = false;

    if (!demo){
        QString start_folder2 = "D:\\projects\\ReconDemo\\Data";
        char   buffer[MAX_PATH];
        getcwd(buffer, MAX_PATH);

        QString start_folder(buffer);
        start_folder +="\\Data";



        auto file_path = QFileDialog::getOpenFileName(this, tr("Load File"), start_folder,
                  "Raw Data (*.fid);;NiuMriImage Files (*.niuimg);;Image Files (*.img);;All Files (*.*)", nullptr);
        if (!file_path.isEmpty())
        {
            DataManager::GetHandle().Load(file_path);

        }
    }
    else
    {

        //流水线的特点：数据驱动，数据不会停止，如果中间有停止机制（停在内存里），将造成拥堵。
        //流水线末端：显示，或者存储等数据堆积则是可以理解的。
        //auto output_data = CreateDemoIData();
        //ui->imagelayout_widget->AddImage(output_data.get(), nullptr);

        DataManager::GetHandle().Demo2D();
    }


}

void MainWindow::on_buttonStart_clicked()
{
    if (!reconServer)
    {
        QString port = ui->editListeningPort->text();
        reconServer = std::shared_ptr<ReconServer>(new ReconServer(this, port.toInt()));
        connect(reconServer.get(), &ReconServer::signalDataReceived, this, &MainWindow::slotDataReceived);
        ui->buttonStart->setText("Waiting");
    }
}



void MainWindow::slotDataReceived(QByteArray dataArray)
{
    //ui->editInfo->appendPlainText(message);
    //

    assert(0);
    return;

}



