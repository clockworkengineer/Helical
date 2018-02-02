#include "helicalserverconnectionsdialog.h"
#include "ui_helicalserverconnectionsdialog.h"
#include "helicalmainwindow.h"
#include "helicalserverdetailsdialog.h"

HelicalServerConnectionsDialog::HelicalServerConnectionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalServerConnectionsDialog)
{
    ui->setupUi(this);

    loadConnectionsList();
    populateConnectionList();

    connect(this, &HelicalServerConnectionsDialog::connectToServer, static_cast<HelicalMainWindow*>(parent), &HelicalMainWindow::connectToServer);

}

HelicalServerConnectionsDialog::~HelicalServerConnectionsDialog()
{
    delete ui;
}

void HelicalServerConnectionsDialog::loadConnectionsList()
{

    QSettings helicalSettings;
    helicalSettings.beginGroup("ConnectionList");
    m_connectionList = helicalSettings.value("connections").toStringList();
    helicalSettings.endGroup();

}

void HelicalServerConnectionsDialog::populateConnectionList()
{

    ui->connectionList->clear();
    ui->connectionList->addItems(m_connectionList);
    saveConnectionList();

}

void HelicalServerConnectionsDialog::saveConnectionList()
{

    QSettings helicalSettings;
    helicalSettings.beginGroup("ConnectionList");
    helicalSettings.setValue("connections",m_connectionList);
    helicalSettings.endGroup();
}

void HelicalServerConnectionsDialog::on_newServerButton_clicked()
{

    HelicalServerDetailsDialog serverDetails("");
    serverDetails.exec();

    QString connectionName = serverDetails.connectionName();
    if (!connectionName.isEmpty()) {
        m_connectionList.append(connectionName);
        populateConnectionList();
    }

}

void HelicalServerConnectionsDialog::on_editServerButton_clicked()
{

    QListWidgetItem *connectionToEdit= ui->connectionList->currentItem();

    if(connectionToEdit != nullptr) {
        HelicalServerDetailsDialog serverDetails(connectionToEdit->text());
        serverDetails.exec();
    }
}

void HelicalServerConnectionsDialog::on_removeServerButton_clicked()
{
    QListWidgetItem *connectionToDelete = ui->connectionList->currentItem();

    if(connectionToDelete != nullptr) {
        QString connectionName = connectionToDelete->text();
        int connectionIndex = m_connectionList.indexOf(connectionName);
        if (connectionIndex != -1) {
            m_connectionList.removeAt(connectionIndex);
            populateConnectionList();
        }
        QSettings hedlicalSettings;
        hedlicalSettings.beginGroup(connectionName);
        hedlicalSettings.remove("");
        hedlicalSettings.endGroup();

    }

}

void HelicalServerConnectionsDialog::on_connectServerButton_clicked()
{

    QListWidgetItem *connectionToRun= ui->connectionList->currentItem();

    if(connectionToRun != nullptr) {
        emit connectToServer(connectionToRun->text());
    }
}
