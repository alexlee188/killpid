#include "killpid.h"
#include "ui_killpid.h"
#include <QMessageBox>

killPID::killPID(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::killPID)
{
    ui->setupUi(this);
    getProcesses();
    loadCombo();
    readList();
}

killPID::~killPID()
{
    delete ui;
}

void killPID::loadCombo()
{
    QString myStr;

    if (!processes.isEmpty()) {
        ui->comboBox->clear();
        for (int i = 0; i < processes.size(); ++i) {
            ui->comboBox->addItem(processes.at(i));
        }
    }
}

void killPID::getProcesses()
{
    // Populate the 'processes' list with every current PID
    QProcess process;
    QString myStr;

    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("ps",  QStringList() << "-A");
    if (!process.waitForStarted())
        return;
    if (!process.waitForFinished())
        return;
    processes.clear();
    myStr = process.readLine();
    while (!myStr.isEmpty())
    {
        myStr = myStr.left(6) + " ... " +
                myStr.mid(24,16).split(" ").first().trimmed();
        processes.append(myStr);
        myStr = process.readLine();
    }
    process.closeWriteChannel();
    process.closeReadChannel(process.readChannel());
}

void killPID::on_pBtnAdd_clicked()
{
    QString myStr;

//  We need to check if we are picking a value from the comboBox list or if we have entered
//  a command name in the comboBox's editLine. If from the list we extract the command name.
    myStr = ui->comboBox->currentText().mid(6, 5);
    if (myStr == " ... ") {  //Sureley no one would have a process called that!
        myStr = ui->comboBox->currentText().mid(11, 15).simplified();
        if (ui->comboBox->currentText() == "  PID  ... CMD") {
            QMessageBox::information(this, "Invalid PID",
                                     "  PID  ... CMD is the header.\n"
                                     "Please choose from the\n"
                                     "comboBox pick List, or\n"
                                     "enter a valid process.");
        } else {
            ui->listWidget->addItem(myStr.split(' ').first());
        }
    } else {
        ui->listWidget->addItem(ui->comboBox->currentText());
    }
}

void killPID::on_pBtnFind_clicked()
{
    QList<QString> pids;
    QString myStr;

    getProcesses();
    myStr = ui->comboBox->currentText();
    if (!processes.isEmpty()) {
        ui->comboBox->clear();
        for (int i = 0; i < processes.size(); ++i) {
            if (processes.at(i).mid(11, 15).simplified().contains(myStr)) {
                ui->comboBox->addItem(processes.at(i));
            }
        }
    }
}

void killPID::on_pBtnClose_clicked()
{
    close();
}

void killPID::on_pBtnRemove_clicked()
{
    ui->listWidget->takeItem(ui->listWidget->currentRow());
}

void killPID::on_pBtnKillPID_clicked()
{
    QString myStr;
    QProcess killSelection;

    myStr = ui->comboBox->currentText().mid(6, 5);
    if (myStr != " ... ") {
        QMessageBox::information(this, "Invalid PID",
                                 "You must choose from the\ncomboBox pick List");
    } else {
        myStr = ui->comboBox->currentText().left(6).trimmed();
        killSelection.start("kill " + myStr);
            if (!killSelection.waitForStarted())
               return;
            if (!killSelection.waitForFinished())
                return;
        ui->comboBox->removeItem(ui->comboBox->currentIndex());
    }
}

void killPID::on_pBtnKillList_clicked()
{
    QString myStr, myList;
    QList<QString> pids;
    QMessageBox msgBox;
    QProcess killSelection;

    myList = "";
    for (int cnt = 0; cnt < ui->listWidget->count(); ++cnt) {
        ui->listWidget->setCurrentRow(cnt);
        myStr = ui->listWidget->currentItem()->text();
        foreach (const QString &str, processes) {
            if (str.isEmpty())
                break;
            if (str.mid(11, 15).simplified() == (myStr)) {
                myList += "\n";
                myList += str.left(6);
                myList += " ";
                myList += str.mid(11, 15).simplified();
                pids.append(str.left(6).trimmed());
            }
        }
    }
    if (pids.count() == 0) {
        QMessageBox::information(this, "No Matching processes!",
                                 "As there were no matching\n"
                                 "processes, nothing was killed");
    } else {
        msgBox.setText("The following pid's will be deleted\n    (Escape key = Cancel)");
        msgBox.setInformativeText(myList);
        msgBox.setStandardButtons(QMessageBox::Apply | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Apply);
        if (msgBox.exec() == 0x02000000) { // msgBox.clickedButton() == msgBox.defaultButton()
            foreach (myStr, pids) {
                if (myStr.isEmpty())
                    break;
                killSelection.start("kill " + myStr);
                if (!killSelection.waitForStarted())
                    return;
                if (!killSelection.waitForFinished())
                    return;
                getProcesses();
                loadCombo();
            }
        }
    }
}

void killPID::readList()
{
    QSettings settings("freesoftware", "killPID");
    int size = settings.beginReadArray("listItems");

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        ui->listWidget->addItem(settings.value("value").toString());
    }
    settings.endArray();
}

void killPID::on_pBtnSave_clicked()
{
     QSettings settings("freesoftware", "killPID");

     settings.clear();
     settings.beginWriteArray("listItems");
     for (int cnt = 0; cnt < ui->listWidget->count(); ++cnt) {
         ui->listWidget->setCurrentRow(cnt);
         settings.setArrayIndex(cnt);
         settings.setValue("value", ui->listWidget->currentItem()->text());
     }
     settings.endArray();
}
