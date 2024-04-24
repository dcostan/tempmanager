#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifdef Q_PROCESSOR_ARM
    QCoreApplication::instance()->installEventFilter(this);

    nextClick->setSingleShot(true);
    connect(nextClick, SIGNAL(timeout()), SLOT(setEventFalse()));
#endif

    QPalette pal = palette();
    pal.setColor(QPalette::Window, "#020017");
    setPalette(pal);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateData()));
    timer->start(2000);
    updateTime();
    updateData();

    connect ( ui->enablerButton, SIGNAL( clicked() ), this, SLOT( enablerClicked() ) );

    connect ( ui->optionsButton, SIGNAL( clicked() ), this, SLOT( optionsClicked() ) );

    connect ( ui->cancelButton, SIGNAL( clicked() ), this, SLOT( cancelClicked() ) );

    connect ( ui->backButton, SIGNAL( clicked() ), this, SLOT( optionsClicked() ) );

    QButtonGroup *bgroup = new QButtonGroup(this);
    bgroup->addButton(ui->zeroButton);
    bgroup->addButton(ui->oneButton);
    bgroup->addButton(ui->twoButton);
    bgroup->addButton(ui->threeButton);
    bgroup->addButton(ui->fourButton);
    bgroup->addButton(ui->fiveButton);
    bgroup->addButton(ui->sixButton);
    bgroup->addButton(ui->sevenButton);
    bgroup->addButton(ui->eightButton);
    bgroup->addButton(ui->nineButton);
    bgroup->addButton(ui->cancButton);
    bgroup->addButton(ui->okButton);

    connect( bgroup, SIGNAL( buttonClicked(QAbstractButton*) ), this, SLOT( keyClicked(QAbstractButton*) ) );

    connect ( ui->nightSwitch, SIGNAL( clicked() ), this, SLOT( nightToggled() ) );

    connect ( ui->dayLimitValue, SIGNAL( clicked() ), this, SLOT( dayLimitClicked() ) );

    connect ( ui->nightLimitValue, SIGNAL( clicked() ), this, SLOT( nightLimitClicked() ) );

    connect ( ui->pressureButton, SIGNAL( clicked() ), this, SLOT( pressureClicked() ) );

    connect ( ui->closeButton, SIGNAL( clicked() ), this, SLOT( optionsClicked() ) );

    connect ( ui->toolsButton, SIGNAL( clicked() ), this, SLOT( toolsClicked() ) );

    connect ( ui->poweroffButton, SIGNAL( clicked() ), this, SLOT( poweroffClicked() ) );

    connect ( ui->guicloserButton, SIGNAL( clicked() ), this, SLOT( guicloserClicked() ) );

    connect ( ui->updaterButton, SIGNAL( clicked() ), this, SLOT( updaterClicked() ) );

    connect ( ui->networkButton, SIGNAL( clicked() ), this, SLOT( connectToNetwork() ) );

    ui->fire->setVisible(false);

    for(int i=0; i<3; i++)
        selectedNumber[i] = '_';

    ui->stackedWidget->setCurrentIndex(0);

    QTimer *runningTime = new QTimer(this);
    connect(runningTime, SIGNAL(timeout()), this, SLOT(restartUI()));
    runningTime->start(86400000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() ==  QEvent::MouseButtonPress && !m_event) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        QPoint origLocation = mouseEvent->pos();

        double newx = 480 - ( origLocation.y() * (1.6) );
        double newy = ( origLocation.x() * (0.7442) ) - 10;

        qDebug() << "x: " << newx << "    y: " << newy;

        QPoint newPoint( (unsigned int)newx, (unsigned int)newy );

        m_event = true;

        QMouseEvent * e1 = new QMouseEvent(QEvent::MouseButtonPress, newPoint, Qt::MouseButton::LeftButton, Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier);
        QCoreApplication::postEvent(obj, e1);
        QMouseEvent * e2 = new QMouseEvent(QEvent::MouseButtonRelease, newPoint, Qt::MouseButton::LeftButton, Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier);
        QCoreApplication::postEvent(obj, e2);

        return true;
    } else if (event->type() ==  QEvent::MouseButtonPress && m_event) {
        nextClick->start(10);
        return QObject::eventFilter(obj, event);
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

void MainWindow::setEventFalse()
{
    m_event = false;
}

void MainWindow::updateTime()
{
    QString text;
    //  Orario
    QTime time = QTime::currentTime();
    text = time.toString("hh:mm");
    if(ui->timeLabel->text().indexOf(text) < 0)
    {
        ui->timeLabel->setText(text);
        ui->timeLabel->setStyleSheet("color: #90ffc6; qproperty-alignment: AlignCenter;");
    }

    //  Data
    QDate date = QDate::currentDate();
    text = QLocale{QLocale::Italian}.toString(date, "d MMMM yyyy");
    if(ui->dateLabel->text().indexOf(text) < 0)
    {
        ui->dateLabel->setText(text);
        ui->dateLabel->setStyleSheet("color: #90ffc6; font: 24pt;");
    }
}

QJsonObject MainWindow::getJsonData()
{
    QFile dataFile("/mnt/ramdisk/data.json");
    dataFile.open(QIODevice::ReadOnly);
    QByteArray dataByte = dataFile.readAll();
    dataFile.close();

    QJsonDocument data = QJsonDocument::fromJson(dataByte);
    return data.object();
}

void MainWindow::saveJsonData(QJsonObject data)
{
    QFile dataFile("/mnt/ramdisk/data.json");
    dataFile.open(QIODevice::WriteOnly);
    dataFile.write(QJsonDocument(data).toJson(QJsonDocument::Compact));
    dataFile.close();
}

void MainWindow::updateData()
{
    //  Temperatura
    QFile tempFile("/mnt/ramdisk/temperature.txt");
    if(!tempFile.exists())
    {
        ui->tempLabel->setText("- - -");
    }
    else
    {
        tempFile.open(QIODevice::ReadOnly);
        QTextStream in(&tempFile);
        QString tempdata = in.readLine();
        int separationIndex = tempdata.lastIndexOf(QChar('*'));
        QString temperature = tempdata.left(separationIndex);

        if(temperature == "nodata")
            ui->tempLabel->setText("- - -");
        else if(temperature.length() >= 3)
            ui->tempLabel->setText(temperature + "°C");

        if(tempdata[separationIndex+1] == '1' && !ui->fire->isVisible())
            ui->fire->setVisible(true);
        else if(tempdata[separationIndex+1] == '0' && ui->fire->isVisible())
            ui->fire->setVisible(false);
    }
    //if(!data["active"].toInt())
    ui->tempLabel->setStyleSheet("color: #90ffc6; font: 60pt; qproperty-alignment: AlignCenter;");
    /*else if(data["night"].toInt())
        ui->tempLabel->setStyleSheet("color: #0000ff; font: 60pt; qproperty-alignment: AlignCenter;");
    else
        ui->tempLabel->setStyleSheet("color: #ff0000; font: 60pt; qproperty-alignment: AlignCenter;");*/

    //  Umidità
    QFile humFile("/mnt/ramdisk/humidity.txt");
    if(!humFile.exists())
    {
        ui->humidityLabel->setText("Umidità: -");
    }
    else
    {
        humFile.open(QIODevice::ReadOnly);
        QTextStream in(&humFile);
        QString humidity = in.readLine();

        if(humidity == "nodata")
            ui->humidityLabel->setText("Umidità: -");
        else if(humidity.length() >= 2)
            ui->humidityLabel->setText("Umidità: " + humidity + "%");
    }
    ui->humidityLabel->setStyleSheet("color: #90ffc6; font: 24pt;");

    //  Pressione
    QFile presFile("/mnt/ramdisk/pressure.npy");
    if(!presFile.exists())
    {
        ui->pressureButton->setText("Pressione barometrica: -");
    }
    else
    {
        double pressure = getPressure(0);

        if(pressure < 0)
            ui->pressureButton->setText("Pressione barometrica: -");
        else
            ui->pressureButton->setText("Pressione barometrica: " + QString::number(pressure) + " hPa");
    }
    ui->pressureButton->setStyleSheet("color: #90ffc6; font: 18pt; font-style: italic;");

    // Rete
    connect ( netProcess, SIGNAL( finished(int) ), this, SLOT( setNetStatus(int) ));
    netProcess->setProgram("checkwlan");
    netProcess->start();

    // Dati JSON
    QJsonObject data = getJsonData();
    if(ui->status->isVisible() != data["active"].toInt())
    {
        ui->enablerButton->setText( data["active"].toInt() ? "Spegni" : "Attiva" );
        ui->status->setVisible(data["active"].toInt());
    }
    if(ui->status->isVisible())
    {
        ui->status->setStyleSheet( data["night"].toInt() ? "background-color: #0000ff;" : "background-color: #ff0000;" );
    }
    if(ui->nightSwitch->isChecked() != data["night"].toInt())
    {
        ui->nightSwitch->setChecked(data["night"].toInt());
        ui->nightLabel->setText( data["night"].toInt() ? "Modalità notte" : "Modalità diurna" );
        ui->nightLabel->setStyleSheet("color: #90ffc6; font: 24pt;");
    }
    if(ui->dayLimitValue->text().indexOf(QString::number(data["max"].toDouble(), 'f', 1)) < 0)
    {
        ui->dayLimitValue->setText(QString::number(data["max"].toDouble(), 'f', 1) + "°C");
        ui->dayLimitValue->setStyleSheet("color: #ff0000; font: 24pt;");
    }
    if(ui->nightLimitValue->text().indexOf(QString::number(data["min"].toDouble(), 'f', 1)) < 0)
    {
        ui->nightLimitValue->setText(QString::number(data["min"].toDouble(), 'f', 1) + "°C");
        ui->nightLimitValue->setStyleSheet("color: #0000ff; font: 24pt;");
    }
    if(ui->stackedWidget->currentIndex() == 4)
    {
        ui->resultLabel->setText(QString(selectedNumber[0]) + QString(selectedNumber[1]) + "." + QString(selectedNumber[2]) + "°C");
        ui->resultLabel->setStyleSheet("color: #90ffc6; font: bold 48pt; qproperty-alignment: AlignCenter;");
        ui->okButton->setEnabled(selectionIndex == 3 ? true : false);
    }
}

void MainWindow::enablerClicked()
{
    QJsonObject data = getJsonData();

    if(data["active"].toInt())
        data["active"] = 0;
    else
        data["active"] = 1;

    saveJsonData(data);

    updateData();
}

void MainWindow::optionsClicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    QLayout* layout = ui->chartView->layout ();
    delete layout;
}

void MainWindow::cancelClicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::keyClicked(QAbstractButton *button)
{
    // get widget id in grid
    int idx = ui->gridLayout->indexOf(button);
    // variables to get grid value
    int row = -1, col = -1, rowspan = -1, colspan = -1;
    // gimme the values! NOW!!
    ui->gridLayout->getItemPosition(idx, &row, &col, &rowspan, &colspan);

    if(row == 0 && col == 3 && selectionIndex < 3)
    {
        selectedNumber[selectionIndex] = '0';
        selectionIndex++;
    }
    else if(row == 1 && col == 3)
    {
        selectionIndex--;
        selectedNumber[selectionIndex] = '_';
    }
    else if(row == 2 && col == 3)
    {
        QJsonObject data = getJsonData();
        ui->stackedWidget->setCurrentIndex(1);
        double result = ((int(selectedNumber[0]) - 48) * 10) + (int(selectedNumber[1]) - 48) + ((int(selectedNumber[2]) - 48) * 0.1);
        if(settingDay)
            data["max"] = result;
        else
            data["min"] = result;
        saveJsonData(data);
        for(int i=0; i<3; i++)
            selectedNumber[i] = '_';
        selectionIndex = 0;
    }
    else if(selectionIndex < 3)
    {
        selectedNumber[selectionIndex] = char(48+col+1+(3*row));
        selectionIndex++;
    }

    updateData();
}

void MainWindow::nightToggled()
{
    QJsonObject data = getJsonData();

    if(data["night"].toInt())
        data["night"] = 0;
    else
        data["night"] = 1;

    saveJsonData(data);

    data = getJsonData();

    ui->nightSwitch->setChecked(data["night"].toInt());
    ui->nightLabel->setText( data["night"].toInt() ? "Modalità notte" : "Modalità diurna" );
    ui->nightLabel->setStyleSheet("color: #90ffc6; font: 24pt;");
}

void MainWindow::pressureClicked()
{
    ui->stackedWidget->setCurrentIndex(5);

    QLineSeries *series = new QLineSeries();

    QFile presFile("/mnt/ramdisk/pressure.npy");
    if(presFile.exists())
    {
        cnpy::NpyArray arr = cnpy::npy_load("/mnt/ramdisk/pressure.npy");

        double* pressure_data = arr.data<double>();
        unsigned int timeLapse = arr.shape[0]/12;
        if(timeLapse < 1)
            timeLapse = 1;

        for(unsigned int i=0; i < arr.shape[0]; i+=timeLapse)
            series->append((arr.shape[0] - i) / 15, pressure_data[i]);
    }

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Andamento della pressione");

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QHBoxLayout *chartLayout = new QHBoxLayout;

    chartLayout->addWidget(chartView);

    ui->chartView->setLayout(chartLayout);
}

void MainWindow::dayLimitClicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    settingDay = true;
    updateData();
}

void MainWindow::nightLimitClicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    settingDay = false;
    updateData();
}

void MainWindow::toolsClicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->afterUpdateButton->setVisible(false);
    ui->progressBar->setRange(0, 0);
    ui->updateLabel->setText("Aggiornamento in corso...");
    ui->updateLabel->setStyleSheet("color: #90ffc6; font: 22pt; qproperty-alignment: AlignCenter;");
}

void MainWindow::guicloserClicked()
{
    QApplication::quit();
}

void MainWindow::poweroffClicked()
{
    ui->stackedWidget->setVisible(false);
    system("poweroff &");
}

void MainWindow::rebootClicked()
{
    ui->stackedWidget->setVisible(false);
    system("reboot &");
}

void MainWindow::updaterClicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    QProcess * const updateProcess = new QProcess();
    connect ( updateProcess, SIGNAL( finished(int) ), this, SLOT( updateFinished(int) ));
    updateProcess->setProgram("system-updater");
    updateProcess->start();
}

void MainWindow::updateFinished(int exitCode)
{
    if(exitCode == 2)
    {
        ui->updateLabel->setText("Compilazione fallita");
        ui->updateLabel->setStyleSheet("color: #90ffc6; font: 22pt; qproperty-alignment: AlignCenter;");
        ui->afterUpdateButton->setVisible(true);
        ui->progressBar->setRange(0, 100);
        ui->progressBar->setValue(0);
        connect ( ui->afterUpdateButton, SIGNAL( clicked() ), this, SLOT( toolsClicked() ) );
    }else if(exitCode == 1)
    {
        ui->updateLabel->setText("Sistema aggiornato");
        ui->updateLabel->setStyleSheet("color: #90ffc6; font: 22pt; qproperty-alignment: AlignCenter;");
        ui->afterUpdateButton->setVisible(true);
        ui->progressBar->setRange(0, 100);
        ui->progressBar->setValue(0);
        connect ( ui->afterUpdateButton, SIGNAL( clicked() ), this, SLOT( toolsClicked() ) );
    }
    else if(exitCode == 0)
    {
        ui->updateLabel->setText("Aggiornamento completato");
        ui->updateLabel->setStyleSheet("color: #90ffc6; font: 22pt; qproperty-alignment: AlignCenter;");
        ui->afterUpdateButton->setVisible(true);
        ui->progressBar->setRange(0, 100);
        ui->progressBar->setValue(100);
        ui->afterUpdateButton->setText("Riavvia");
        connect ( ui->afterUpdateButton, SIGNAL( clicked() ), this, SLOT( rebootClicked() ) );
    }
    else
    {
        ui->updateLabel->setText("Errore sconosciuto");
        ui->updateLabel->setStyleSheet("color: #90ffc6; font: 22pt; qproperty-alignment: AlignCenter;");
        ui->afterUpdateButton->setVisible(true);
        ui->progressBar->setRange(0, 100);
        ui->progressBar->setValue(0);
        connect ( ui->afterUpdateButton, SIGNAL( clicked() ), this, SLOT( toolsClicked() ) );
    }
}

double MainWindow::getPressure(unsigned int i)
{
    try
    {
        cnpy::NpyArray arr = cnpy::npy_load("/mnt/ramdisk/pressure.npy");

        double* pressure_data = arr.data<double>();

        if(arr.shape[0] <= i)
            return -1;

        return pressure_data[i];
    }
    catch (std::runtime_error &e)
    {
        qWarning() << "Caught a runtime_error exception: " << e.what () << '\n';
        return -1;
    }
}

void MainWindow::connectToNetwork()
{
    if(netStatus == 2)
        system("rfkill unblock wifi &");
    else
        system("rfkill block wifi &");
}

void MainWindow::setNetStatus(int exitCode)
{
    if(netStatus != exitCode)
    {
        netStatus = exitCode;

        if(exitCode == 2)
            ui->networkButton->setStyleSheet("background-image: url(:/lock.png); \
                                              background-repeat: repeat-n;       \
                                              background-position: center;       \
                                              border-width:1px;                  \
                                              border-radius:20px;                \
                                              max-width:80px;                    \
                                              max-height:80px;                   \
                                              min-width:80px;                    \
                                              min-height:80px;");
        else if(exitCode == 1)
            ui->networkButton->setStyleSheet("background-image: url(:/net.png);  \
                                              background-repeat: repeat-n;       \
                                              background-position: center;       \
                                              border-width:1px;                  \
                                              border-radius:20px;                \
                                              max-width:80px;                    \
                                              max-height:80px;                   \
                                              min-width:80px;                    \
                                              min-height:80px;");
        else
            ui->networkButton->setStyleSheet("background-image: url(:/nonet.png);\
                                              background-repeat: repeat-n;       \
                                              background-position: center;       \
                                              border-width:1px;                  \
                                              border-radius:20px;                \
                                              max-width:80px;                    \
                                              max-height:80px;                   \
                                              min-width:80px;                    \
                                              min-height:80px;");
    }

}

void MainWindow::restartUI()
{
    system("systemctl restart gui");
}
