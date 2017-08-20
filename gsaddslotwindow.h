#ifndef GSADDSLOTWINDOW_H
#define GSADDSLOTWINDOW_H

#include <QMainWindow>
#include <QSplitter>

#include <QProcess>

namespace Ui {
class GSAddSlotWindow;
}

class GSAddSlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GSAddSlotWindow(QWidget *parent = 0);

    void addNewSlot();

    ~GSAddSlotWindow();

private slots:
    void on_actionNew_triggered();

    void on_actionCheck_triggered();

    void printMessage(QString msg);

    void readFromCompiller();
    void readErrorCompiller();
    void finishedCompiller(int exitCode);

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionConsole_triggered(bool checked);

    void on_actionActions_triggered(bool checked);

private:
    Ui::GSAddSlotWindow *ui;
    QSplitter splitter;

    QProcess *processCompiller;
};

#endif // GSADDSLOTWINDOW_H
