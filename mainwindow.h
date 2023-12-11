#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cnpy.h"

#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QAbstractButton>
#include <QtGlobal>
#include <QtNetwork>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QLineSeries>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setEventFalse();
    void updateTime();
    QJsonObject getJsonData();
    void saveJsonData(QJsonObject data);
    void updateData();
    void enablerClicked();
    void optionsClicked();
    void cancelClicked();
    void keyClicked(QAbstractButton *button);
    void nightToggled();
    void pressureClicked();
    void toolsClicked();
    void guicloserClicked();
    void poweroffClicked();
    void rebootClicked();
    void updaterClicked();
    void updateFinished(int exitCode);
    void dayLimitClicked();
    void nightLimitClicked();
    double getPressure(unsigned int i);
    void connectToNetwork();
    void setNetStatus(int exitCode);
    void restartUI();

private:
    Ui::MainWindow *ui;
    bool eventFilter(QObject *, QEvent*);
    QTimer *nextClick = new QTimer(this);
    bool m_event = false;
    bool enabled;
    char selectedNumber[3];
    int selectionIndex = 0;
    bool settingDay;
    QProcess * const netProcess = new QProcess();
    int netStatus = 0;
};

#endif // MAINWINDOW_H
