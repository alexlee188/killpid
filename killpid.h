#ifndef KILLPID_H
#define KILLPID_H

#include <QDialog>
#include <QProcess>
#include <QSettings>

namespace Ui {
    class killPID;
}

class killPID : public QDialog
{
    Q_OBJECT

public:
    explicit killPID(QWidget *parent = 0);
    ~killPID();

private slots:
    void on_pBtnAdd_clicked();

    void on_pBtnFind_clicked();

    void on_pBtnClose_clicked();

    void on_pBtnRemove_clicked();

    void on_pBtnKillPID_clicked();

    void on_pBtnKillList_clicked();

    void on_pBtnSave_clicked();

private:
    Ui::killPID *ui;
    void getProcesses();
    void loadCombo();
    void readList();
    QList<QString> processes;

};

#endif // KILLPID_H
