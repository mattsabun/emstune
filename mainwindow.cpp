/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of FreeTune                                        *
*                                                                          *
*   FreeTune is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   FreeTune is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/

#include "mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include "datafield.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	//populateDataFields();
	ui.setupUi(this);
	connect(ui.connectPushButton,SIGNAL(clicked()),this,SLOT(connectButtonClicked()));
	connect(ui.loadLogPushButton,SIGNAL(clicked()),this,SLOT(loadLogButtonClicked()));
	connect(ui.playLogPushButton,SIGNAL(clicked()),this,SLOT(playLogButtonClicked()));
	connect(ui.pauseLogPushButton,SIGNAL(clicked()),this,SLOT(pauseLogButtonClicked()));
	connect(ui.stopLogPushButton,SIGNAL(clicked()),this,SLOT(stopLogButtonClicked()));
	ui.sendCommandTableWidget->setColumnCount(4);
	ui.sendCommandTableWidget->setColumnWidth(0,50);
	ui.sendCommandTableWidget->setColumnWidth(1,100);
	ui.sendCommandTableWidget->setColumnWidth(2,100);
	ui.sendCommandTableWidget->setColumnWidth(3,500);

	ui.locationIdInfoTableWidget->setColumnCount(16);
	ui.locationIdInfoTableWidget->setColumnWidth(0,80);
	ui.locationIdInfoTableWidget->setColumnWidth(1,80);
	ui.locationIdInfoTableWidget->setColumnWidth(2,80);
	ui.locationIdInfoTableWidget->setColumnWidth(3,80);
	ui.locationIdInfoTableWidget->setColumnWidth(4,80);
	ui.locationIdInfoTableWidget->setColumnWidth(5,100);
	ui.locationIdInfoTableWidget->setColumnWidth(6,100);
	ui.locationIdInfoTableWidget->setColumnWidth(7,80);
	ui.locationIdInfoTableWidget->setColumnWidth(8,80);
	ui.locationIdInfoTableWidget->setColumnWidth(9,80);
	ui.locationIdInfoTableWidget->setColumnWidth(10,80);
	ui.locationIdInfoTableWidget->setColumnWidth(11,80);
	ui.locationIdInfoTableWidget->setColumnWidth(12,120);
	ui.locationIdInfoTableWidget->setColumnWidth(13,120);
	ui.locationIdInfoTableWidget->setColumnWidth(14,120);
	ui.locationIdInfoTableWidget->setColumnWidth(15,120);

	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("LocID"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Flags"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Parent"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("RamPage"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("FlashPage"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("RamAddress"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("FlashAddress"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("Size"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(8,new QTableWidgetItem("Has Parent"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(9,new QTableWidgetItem("Is Ram"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(10,new QTableWidgetItem("Is Flash"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(11,new QTableWidgetItem("Is Index"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(12,new QTableWidgetItem("Is Read Only"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(13,new QTableWidgetItem("Is Verified"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(14,new QTableWidgetItem("For Backup"));
	ui.locationIdInfoTableWidget->setHorizontalHeaderItem(15,new QTableWidgetItem("Table Type"));

	connect(ui.interByteDelaySpinBox,SIGNAL(valueChanged(int)),this,SLOT(interByteDelayChanged(int)));
	dataPacketDecoder = new DataPacketDecoder(this);
	connect(dataPacketDecoder,SIGNAL(payloadDecoded(QMap<QString,double>)),this,SLOT(dataLogDecoded(QMap<QString,double>)));
	ui.tableWidget->setColumnCount(2);
	ui.tableWidget->setColumnWidth(0,150);
	ui.tableWidget->setColumnWidth(1,50);

	ui.tableWidget->setRowCount(dataPacketDecoder->m_dataFieldList.size());
	for (int i=0;i<dataPacketDecoder->m_dataFieldList.size();i++)
	{
		m_nameToIndexMap[dataPacketDecoder->m_dataFieldList[i].name()] = i;
		ui.tableWidget->setItem(i,0,new QTableWidgetItem(dataPacketDecoder->m_dataFieldList[i].description()));
		ui.tableWidget->setItem(i,1,new QTableWidgetItem("0"));
	}

	/*logLoader = new LogLoader(this);
	connect(logLoader,SIGNAL(endOfLog()),this,SLOT(logFinished()));
	connect(logLoader,SIGNAL(payloadReceived(QByteArray,QByteArray)),this,SLOT(logPayloadReceived(QByteArray,QByteArray)));
	connect(logLoader,SIGNAL(logProgress(qlonglong,qlonglong)),this,SLOT(logProgress(qlonglong,qlonglong)));
	*/
	emsComms = new FreeEmsComms(this);
	connect(emsComms,SIGNAL(connected()),this,SLOT(emsCommsConnected()));
	connect(emsComms,SIGNAL(dataLogPayloadReceived(QByteArray,QByteArray)),this,SLOT(logPayloadReceived(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(firmwareVersion(QString)),this,SLOT(firmwareVersion(QString)));
	connect(emsComms,SIGNAL(interfaceVersion(QString)),this,SLOT(interfaceVersion(QString)));
	connect(emsComms,SIGNAL(locationIdList(QList<unsigned short>)),this,SLOT(locationIdList(QList<unsigned short>)));
	connect(emsComms,SIGNAL(unknownPacket(QByteArray,QByteArray)),this,SLOT(unknownPacket(QByteArray,QByteArray)));
	connect(emsComms,SIGNAL(commandSuccessful(int)),this,SLOT(commandSuccessful(int)));
	connect(emsComms,SIGNAL(commandFailed(int,unsigned short)),this,SLOT(commandFailed(int,unsigned short)));
	connect(emsComms,SIGNAL(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)),this,SLOT(locationIdInfo(unsigned short,unsigned short,QList<FreeEmsComms::LocationIdFlags>,unsigned short,unsigned char,unsigned char,unsigned short,unsigned short,unsigned short)));


	widget = new GaugeWidget(ui.tab_2);
	widget->setGeometry(0,0,1200,600);
	widget->show();

	pidcount = 0;

	timer = new QTimer(this);
	connect(timer,SIGNAL(timeout()),this,SLOT(timerTick()));
	timer->start(1000);

	guiUpdateTimer = new QTimer(this);
	connect(guiUpdateTimer,SIGNAL(timeout()),this,SLOT(guiUpdateTimerTick()));
	guiUpdateTimer->start(250);

	statusBar()->addWidget(ui.ppsLabel);
	statusBar()->addWidget(ui.statusLabel);
	emsComms->start();
}

void MainWindow::timerTick()
{
	ui.ppsLabel->setText("PPS: " + QString::number(pidcount));
	pidcount = 0;
}
void MainWindow::unknownPacket(QByteArray header,QByteArray payload)
{
	QString result = "";
	for (int i=0;i<header.size();i++)
	{
		result += (((unsigned char)header[i] < (char)0xF) ? "0" : "") + QString::number((unsigned char)header[i],16).toUpper();
	}
	for (int i=0;i<payload.size();i++)
	{
		result += (((unsigned char)payload[i] < (char)0xF) ? "0" : "") + QString::number((unsigned char)payload[i],16).toUpper();
	}
}

void MainWindow::loadLogButtonClicked()
{
	QFileDialog file;
	if (file.exec())
	{
		if (file.selectedFiles().size() > 0)
		{
			QString filename = file.selectedFiles()[0];
			ui.statusLabel->setText("Status: File loaded and not playing");
			//logLoader->loadFile(filename);
			emsComms->loadLog(filename);

		}
	}
}
void MainWindow::interByteDelayChanged(int num)
{
	emsComms->setInterByteSendDelay(num);
}

void MainWindow::logFinished()
{
	ui.statusLabel->setText("Status: File loaded and log finished");
}

void MainWindow::playLogButtonClicked()
{
	//logLoader->start();
	emsComms->playLog();
	ui.statusLabel->setText("Status: File loaded and playing");
}
void MainWindow::locationIdList(QList<unsigned short> idlist)
{
	for (int i=0;i<idlist.size();i++)
	{
		//ui/listWidget->addItem(QString::number(idlist[i]));
		emsComms->getLocationIdInfo(idlist[i]);
	}
}
void MainWindow::locationIdInfo(unsigned short locationid,unsigned short rawFlags,QList<FreeEmsComms::LocationIdFlags> flags,unsigned short parent, unsigned char rampage,unsigned char flashpage,unsigned short ramaddress,unsigned short flashaddress,unsigned short size)
{
	/*ui.locationIdInfoTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("LocID"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Parent"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("RamPage"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("FlashPage"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("RamAddress"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("FlashAddress"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("Size"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("Has Parent"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(8,new QTableWidgetItem("Is Ram"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(9,new QTableWidgetItem("Is Flash"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(10,new QTableWidgetItem("Is Index"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(11,new QTableWidgetItem("Is Read Only"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(12,new QTableWidgetItem("Is Verified"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(13,new QTableWidgetItem("For Backup"));
 ui.locationIdInfoTableWidget->setHorizontalHeaderItem(14,new QTableWidgetItem("Table Type"));*/
	qDebug() << "Location ID Info for:" << QString::number(locationid,16);
	bool found = false;
	int foundi = -1;

	/*for (int i=0;i<ui.locationIdInfoTableWidget->rowCount();i++)
	{
		if (ui.locationIdInfoTableWidget->item(i,0)->text().toInt() == locationid)
		{
			foundi = i;
			found = true;
		}
	}
	if (!found)
	{*/
	ui.locationIdInfoTableWidget->setRowCount(ui.locationIdInfoTableWidget->rowCount()+1);
	foundi = ui.locationIdInfoTableWidget->rowCount()-1;
	ui.locationIdInfoTableWidget->setItem(foundi,0,new QTableWidgetItem(QString::number(locationid,16)));
	for (int i=1;i<16;i++)
	{
		ui.locationIdInfoTableWidget->setItem(foundi,i,new QTableWidgetItem(""));
	}
	//}
	ui.locationIdInfoTableWidget->item(foundi,1)->setText(QString::number(rawFlags));

	if (flags.contains(FreeEmsComms::BLOCK_IS_2D_TABLE))
	{
		ui.locationIdInfoTableWidget->item(foundi,15)->setText("2d Table");
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_LOOKUP_DATA))
	{
		ui.locationIdInfoTableWidget->item(foundi,15)->setText("Lookup ");
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_MAIN_TABLE))
	{
		ui.locationIdInfoTableWidget->item(foundi,15)->setText("Main Table");
	}
	else if (flags.contains(FreeEmsComms::BLOCK_IS_CONFIGURATION))
	{
		ui.locationIdInfoTableWidget->item(foundi,15)->setText("Config");
	}

	if (flags.contains(FreeEmsComms::BLOCK_IS_INDEXABLE))
	{
		ui.locationIdInfoTableWidget->item(foundi,11)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,13)->setText("False");
	}

	if (flags.contains(FreeEmsComms::BLOCK_IS_READ_ONLY))
	{
		ui.locationIdInfoTableWidget->item(foundi,12)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,12)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_GETS_VERIFIED))
	{
		ui.locationIdInfoTableWidget->item(foundi,13)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,13)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_FOR_BACKUP_RESTORE))
	{
		ui.locationIdInfoTableWidget->item(foundi,14)->setText("True");
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,14)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_IS_RAM))
	{
		ui.locationIdInfoTableWidget->item(foundi,9)->setText("True");
		ui.locationIdInfoTableWidget->item(foundi,3)->setText(QString::number(rampage));
		ui.locationIdInfoTableWidget->item(foundi,5)->setText(QString::number(ramaddress));
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,9)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_IS_FLASH))
	{
		ui.locationIdInfoTableWidget->item(foundi,10)->setText("True");
		ui.locationIdInfoTableWidget->item(foundi,4)->setText(QString::number(flashpage));
		ui.locationIdInfoTableWidget->item(foundi,6)->setText(QString::number(flashaddress));
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,10)->setText("False");
	}
	if (flags.contains(FreeEmsComms::BLOCK_HAS_PARENT))
	{
		ui.locationIdInfoTableWidget->item(foundi,8)->setText("True");
		ui.locationIdInfoTableWidget->item(foundi,2)->setText(QString::number(parent));
	}
	else
	{
		ui.locationIdInfoTableWidget->item(foundi,8)->setText("False");
	}
	ui.locationIdInfoTableWidget->item(foundi,7)->setText(QString::number(size));
	//Q_UNUSED(locationid)
	//Q_UNUSED(flags)
	//Q_UNUSED(parent)
	//Q_UNUSED(rampage)
	//Q_UNUSED(flashpage)
	//Q_UNUSED(ramaddress)
	//Q_UNUSED(flashaddress)
	//Q_UNUSED(size)
}
void MainWindow::blockRetrieved(int sequencenumber,QByteArray header,QByteArray payload)
{
	Q_UNUSED(sequencenumber)
	Q_UNUSED(header)
	Q_UNUSED(payload)
}
void MainWindow::dataLogPayloadReceived(QByteArray header,QByteArray payload)
{
	Q_UNUSED(header)
	Q_UNUSED(payload)
}
void MainWindow::interfaceVersion(QString version)
{
	ui.interfaceVersionLineEdit->setText(version);
}
void MainWindow::firmwareVersion(QString version)
{
	ui.firmwareVersionLineEdit->setText(version);
}
void MainWindow::error(QString msg)
{
	Q_UNUSED(msg)
}
void MainWindow::emsCommsConnected()
{
	int firmwareseq = emsComms->getFirmwareVersion();
	int ifaceseq = emsComms->getInterfaceVersion();
	int locidseq = emsComms->getLocationIdList(0x00,0x00);
	ui.sendCommandTableWidget->setRowCount(ui.sendCommandTableWidget->rowCount()+1);
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(firmwareseq)));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,1,new QTableWidgetItem("0"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,2,new QTableWidgetItem("Pending"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,3,new QTableWidgetItem("getFirmwareVersion"));

	ui.sendCommandTableWidget->setRowCount(ui.sendCommandTableWidget->rowCount()+1);
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(ifaceseq)));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,1,new QTableWidgetItem("0"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,2,new QTableWidgetItem("Pending"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,3,new QTableWidgetItem("getInterfaceVersion"));

	ui.sendCommandTableWidget->setRowCount(ui.sendCommandTableWidget->rowCount()+1);
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(locidseq)));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,1,new QTableWidgetItem("0"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,2,new QTableWidgetItem("Pending"));
	ui.sendCommandTableWidget->setItem(ui.sendCommandTableWidget->rowCount()-1,3,new QTableWidgetItem("getLocationIdList"));
}

void MainWindow::commandSuccessful(int sequencenumber)
{
	qDebug() << "command succesful:" << QString::number(sequencenumber);
	for (int i=0;i<ui.sendCommandTableWidget->rowCount();i++)
	{
		if (ui.sendCommandTableWidget->item(i,0)->text().toInt() == sequencenumber)
		{
			ui.sendCommandTableWidget->item(i,1)->setText("Success");
		}
	}
}
void MainWindow::commandFailed(int sequencenumber,unsigned short errornum)
{
	qDebug() << "command failed:" << QString::number(sequencenumber);
	for (int i=0;i<ui.sendCommandTableWidget->rowCount();i++)
	{
		if (ui.sendCommandTableWidget->item(i,0)->text().toInt() == sequencenumber)
		{
			ui.sendCommandTableWidget->item(i,1)->setText("Failed");
			ui.sendCommandTableWidget->item(i,2)->setText(QString::number(errornum));
		}
	}
}
void MainWindow::pauseLogButtonClicked()
{

}

void MainWindow::stopLogButtonClicked()
{

}
void MainWindow::connectButtonClicked()
{
	emsComms->connectSerial(ui.portNameLineEdit->text(),ui.baudRateLineEdit->text().toInt());
}

void MainWindow::logProgress(qlonglong current,qlonglong total)
{
	Q_UNUSED(current)
	Q_UNUSED(total)
	//setWindowTitle(QString::number(current) + "/" + QString::number(total) + " - " + QString::number((float)current/(float)total));
}
void MainWindow::guiUpdateTimerTick()
{
	QMap<QString,double>::const_iterator i = m_valueMap.constBegin();
	while (i != m_valueMap.constEnd())
	{
		widget->propertyMap.setProperty(i.key().toAscii(),QVariant::fromValue(i.value()));
		ui.tableWidget->item(m_nameToIndexMap[i.key()],1)->setText(QString::number(i.value()));
		//qDebug() << i.key() << m_nameToIndexMap[i.key()] << i.value();
		i++;
	}
}
void MainWindow::dataLogDecoded(QMap<QString,double> data)
{
	m_valueMap = data;
}

void MainWindow::logPayloadReceived(QByteArray header,QByteArray payload)
{
	Q_UNUSED(header)
	pidcount++;
	if (payload.length() != 96)
	{
		//Wrong sized payload!
		//We should do something here or something...
		//return;
	}
	dataPacketDecoder->decodePayload(payload);
	//guiUpdateTimerTick();

}

MainWindow::~MainWindow()
{
	emsComms->terminate();
	emsComms->wait(1000);
	delete emsComms;
}
