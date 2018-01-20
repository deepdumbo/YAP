#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <direct.h>

#include "Implement/LogImpl.h"
#include "Implement/LogUserImpl.h"
#include "API/Yap/ModuleManager.h"

#include "datamanager.h"
#include "Processors/NiuMriDisplay1D.h"
#include "Processors/NiuMriDisplay2D.h"
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

    test_plot1d();

    on_buttonStart_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::SetImportedProcessors()
{
   // ImportedProcessorManager::RegisterProcessor(L"NiuMriDisplay1D", new NiuMriDisplay1D(*ui->widgetChart));
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

        DataManager::GetHandle().Demo();
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



    uint32_t cmd_id = *(uint32_t*)dataArray.data();
    SampleDataStart start;
    SampleDataData  data;
    SampleDataEnd   end;


    switch(cmd_id)
    {
        case SAMPLE_DATA_START:
        {
            //start.Unpack(dataArray);
        }
            break;
        case SAMPLE_DATA_DATA:
        {
            //data.Unpack(dataArray);

            ui->plot1d_widget->SetData(data.data);
        }
            break;

        case SAMPLE_DATA_END:
        {
            //end.Unpack(dataArray);
        }
            break;
        default:
            break;


    }



    return;

}



void MainWindow::test_plot1d()
{

      const double PI = 3.1415926;
      std::vector<float> data_vector(222);
      std::vector<std::complex<float>> data_vector2(314);


      double dw = 4*PI / data_vector.size();

      for(unsigned int i = 0; i < data_vector.size(); i ++)
      {
          data_vector[i] = 10* sin(i * dw + 30 * PI/ 180);

      }
      for(unsigned int i = 0; i < data_vector2.size(); i ++)
      {

          double temp = data_vector2.size() + 10 - i;
          data_vector2[i].real( temp* 10* sin(i * dw + 30 * PI/ 180) );
          data_vector2[i].imag( temp* 10* cos(i * dw + 30 * PI/ 180) );


      }

     ui->plot1d_widget->SetData(data_vector2);

}
