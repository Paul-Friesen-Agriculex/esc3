#include <iostream>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QString> 
#include <QStringList>
#include <QByteArray>   
#include <QMessageBox>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QAbstractSocket>
#include <QHostInfo>
#include <QTcpSocket>
#include <QDataStream>
#include <QNetworkProxyQuery>	//2020_12_17
//#include <QTimer>	//Need for ping test

#include "centre.hpp"
#include "windows_tcp_printer.hpp"
#include "button.hpp"
//#include "help_screen.hpp"


using namespace std;

windows_tcp_printer::windows_tcp_printer(centre* set_centre_p)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  //help_screen_p = 0;  // Currently defined a modal QMessageBox
  
  connection_header_label_p = new QLabel("Connection Status:");
  connection_status_label_p = new QLabel("Not Connected.");
  connection_status_label_p->setStyleSheet("QLabel { color: red }");

  back_button_p = new button("Back");
  help_button_p = new button("Help");
  connect_button_p = new button("Connect");
  disconnect_button_p = new button("Disconnect");

  ipv4_groupbox_p = new QGroupBox;
  port_groupbox_p = new QGroupBox;

  main_layout_p= new QGridLayout;
  ipv4_layout_p = new QGridLayout;
  port_layout_p = new QGridLayout;

  addr1_p = new QSpinBox;
  addr2_p = new QSpinBox;
  addr3_p = new QSpinBox;
  addr4_p = new QSpinBox;
  port1_p = new QSpinBox;
  port2_p = new QSpinBox;
  port3_p = new QSpinBox;
  port4_p = new QSpinBox;
  port5_p = new QSpinBox;
  
  create_ipv4_groupbox();
  create_port_groupbox(); 
  
  main_layout_p->addWidget(help_button_p, 0, 0);
  main_layout_p->addWidget(back_button_p, 0, 3);
  main_layout_p->addWidget(ipv4_groupbox_p, 1, 0, 1, 4);
  main_layout_p->addWidget(port_groupbox_p, 2, 0, 1, 4);
  main_layout_p->addWidget(connection_header_label_p, 3, 0);
  main_layout_p->addWidget(connection_status_label_p, 3, 1, 1, 3);
  main_layout_p->addWidget(connect_button_p, 4, 2);
  main_layout_p->addWidget(disconnect_button_p, 4, 3);
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(connect_button_p, SIGNAL(clicked()), this, SLOT(connect_windows_tcp()));
  connect(disconnect_button_p, SIGNAL(clicked()), this, SLOT(disconnect_windows_tcp()));
  connect(centre_p, SIGNAL(tcp_connection_detected_signal()), this, SLOT(connection_detected())); //
  
  default_windows_printer_ipv4 = "192.168.0.1";
  default_windows_printer_port = 13000;
  
  check_socket_status();
  initial_ipv4();
  initial_port();
  set_ipv4();
  set_port();  
  
  change_network();	//TEST~~~ 2020_12_17
}

void windows_tcp_printer::change_network()	//TEST~~~ 2020_12_17
{
	//Programmatically change between multiple networks or configuring static Ipv4 addresses
	
	QNetworkProxyQuery* test_network_proxy = new QNetworkProxyQuery();
	
	centre_p->tcp_socket_p->setProxy(QNetworkProxy::NoProxy);
}

windows_tcp_printer::~windows_tcp_printer()
{
	//
}

void windows_tcp_printer::create_ipv4_groupbox()
{
  addr1_p->setRange(0,255);
  addr2_p->setRange(0,255);
  addr3_p->setRange(0,255);
  addr4_p->setRange(0,255);
  
  addr1_p -> setStyleSheet("QSpinBox { width: 50px }" );
  addr2_p -> setStyleSheet("QSpinBox { width: 50px }" );
  addr3_p -> setStyleSheet("QSpinBox { width: 50px }" );
  addr4_p -> setStyleSheet("QSpinBox { width: 50px }" );
  
  addr2_p->setPrefix(".");
  addr3_p->setPrefix(".");
  addr4_p->setPrefix(".");
  
  ipv4_layout_p->addWidget(addr1_p, 0, 0);
  ipv4_layout_p->addWidget(addr2_p, 0, 1);
  ipv4_layout_p->addWidget(addr3_p, 0, 2);
  ipv4_layout_p->addWidget(addr4_p, 0, 3);
  ipv4_groupbox_p->setLayout(ipv4_layout_p);
  
  ipv4_groupbox_p->setStyleSheet("QGroupBox { border: 0px }");
}

void windows_tcp_printer::create_port_groupbox()
{
  port1_p -> setRange(0,6);
  port2_p -> setRange(0,9);
  port3_p -> setRange(0,9);
  port4_p -> setRange(0,9);
  port5_p -> setRange(0,9);
  
  port1_p -> setStyleSheet("QSpinBox { width: 50px }" );
  port2_p -> setStyleSheet("QSpinBox { width: 50px }" );
  port3_p -> setStyleSheet("QSpinBox { width: 50px }" );
  port4_p -> setStyleSheet("QSpinBox { width: 50px }" );
  port5_p -> setStyleSheet("QSpinBox { width: 50px }" );
  
  port_layout_p->addWidget(port1_p, 0, 0);
  port_layout_p->addWidget(port2_p, 0, 1);
  port_layout_p->addWidget(port3_p, 0, 2);
  port_layout_p->addWidget(port4_p, 0, 3);
  port_layout_p->addWidget(port5_p, 0, 4);
  port_groupbox_p->setLayout(port_layout_p);
  
  port_groupbox_p->setStyleSheet("QGroupBox { border: 0px }");
}

void windows_tcp_printer::initial_ipv4()
{
	QString found_ipv4;
	QList<QHostAddress> list = QNetworkInterface::allAddresses();	
  
  for(int nIter=0; nIter<list.count(); nIter++)
  {
      if(!list[nIter].isLoopback())
      {
          if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
          {
              found_ipv4 = list[nIter].toString();
          }
      }
  }
  
  if(centre_p->tcp_socket_p->state() == QAbstractSocket::ConnectedState)
  {
		windows_printer_ipv4 = centre_p->tcp_socket_p->peerAddress().toString();
	}
  else if(!found_ipv4.isEmpty())
  {
    windows_printer_ipv4 = found_ipv4;
  }
	else
	{
		windows_printer_ipv4 = default_windows_printer_ipv4;
	}
}

void windows_tcp_printer::initial_port()
{
	if(centre_p->tcp_socket_p->state() == QAbstractSocket::ConnectedState)
	{
		windows_printer_port = centre_p->tcp_socket_p->peerPort();
	}
	else
	{
		windows_printer_port = default_windows_printer_port;
	}
}

void windows_tcp_printer::set_ipv4()
{
  QString ipv4_value = windows_printer_ipv4;
  QStringList ipv4_segments;
  int addr1, addr2, addr3, addr4;
  
	ipv4_segments = ipv4_value.split(".");
  addr1 = ipv4_segments[0].toInt();
  addr2 = ipv4_segments[1].toInt();
  addr3 = ipv4_segments[2].toInt();
  addr4 = ipv4_segments[3].toInt();
  
  addr1_p->setValue(addr1);
  addr2_p->setValue(addr2);
  addr3_p->setValue(addr3);
  addr4_p->setValue(addr4);
}

void windows_tcp_printer::set_port()
{
  QString port_value = QString::number(windows_printer_port); 
  
  port1_p->setValue(port_value.at(0).digitValue());
  port2_p->setValue(port_value.at(1).digitValue());
  port3_p->setValue(port_value.at(2).digitValue());
  port4_p->setValue(port_value.at(3).digitValue());
  port5_p->setValue(port_value.at(4).digitValue());
}

void windows_tcp_printer::get_ipv4()
{ 
  QString ipv4_value;
  
  ipv4_value = QString::number(addr1_p->value()) + "." +
               QString::number(addr2_p->value()) + "." +
               QString::number(addr3_p->value()) + "." +
               QString::number(addr4_p->value());
               
  windows_printer_ipv4 = ipv4_value;
}

void windows_tcp_printer::get_port()
{
  QString port_value;
  
  port_value = QString::number(port1_p->value()) + 
							 QString::number(port2_p->value()) + 
							 QString::number(port3_p->value()) + 
							 QString::number(port4_p->value()) + 
							 QString::number(port5_p->value());
			   
  windows_printer_port = port_value.toInt();
}

void windows_tcp_printer::check_socket_status()
{
  QTcpSocket* tcp_server_reference = new QTcpSocket;
	tcp_server_reference = centre_p->tcp_socket_p;
	
	qDebug() << tcp_server_reference->state();
  
  if((tcp_server_reference->state() == QAbstractSocket::ConnectedState) || (tcp_server_reference->state() == QAbstractSocket::ConnectingState))
	{
		connection_status_label_p->setText("Connected");
		connection_status_label_p->setStyleSheet("QLabel { color: green }");
		
		qDebug() << "---Socket Address---";
		qDebug() << "Local:  " << tcp_server_reference->localAddress() << ":" << tcp_server_reference->localPort();
		qDebug() << "Target: " << tcp_server_reference->peerAddress() << ":" <<tcp_server_reference->peerPort();		
	}
	else if((tcp_server_reference->state() == QAbstractSocket::UnconnectedState) || (tcp_server_reference->state() == QAbstractSocket::ClosingState))
  {
		centre_p->windows_printer_tcp_connection = false;
		centre_p->communicate_by_tcp = false;
		
	  connection_status_label_p->setText("Not Connected");
		connection_status_label_p->setStyleSheet("QLabel { color: red }");
	}
	
  if(tcp_server_reference->state() == QAbstractSocket::ConnectedState)
  {
		centre_p->windows_printer_tcp_connection = true;
		centre_p->communicate_by_tcp = true;
		
		windows_printer_ipv4 = tcp_server_reference->localAddress().toString();
		windows_printer_port = tcp_server_reference->localPort();
	}
	
	qDebug() << "centre_p->windows_printer_tcp_connection: " << centre_p->windows_printer_tcp_connection;	//OMIT~~~//
	qDebug() << "centre_p->communicate_by_tcp: " << centre_p->communicate_by_tcp;													//OMIT~~~//
}

void windows_tcp_printer::connect_windows_tcp()
{
	get_ipv4();
	get_port();
	
	QString addr_string = QString::number(addr1_p->value()) + "." +
						  QString::number(addr2_p->value()) + "." +
						  QString::number(addr3_p->value()) + "." +
						  QString::number(addr4_p->value());
	QString port_string = QString::number(port1_p->value()) + 
						  QString::number(port2_p->value()) +
						  QString::number(port3_p->value()) +
						  QString::number(port4_p->value()) +
						  QString::number(port5_p->value());
						  	
	QTcpSocket* tcp_server_reference = new QTcpSocket;
	tcp_server_reference = centre_p->tcp_socket_p;
	tcp_server_reference->connectToHost(addr_string, port_string.toInt());

	if(tcp_server_reference->waitForConnected(5000))
	{
		check_socket_status();
	}
}

void windows_tcp_printer::disconnect_windows_tcp()
{  
  QTcpSocket* tcp_server_reference = new QTcpSocket;
  tcp_server_reference = centre_p->tcp_socket_p;  
  
  tcp_server_reference->disconnectFromHost();
  
  if((tcp_server_reference->waitForDisconnected(5000)) || (tcp_server_reference->state() == QAbstractSocket::UnconnectedState))
  {
		check_socket_status();
  }
}

void windows_tcp_printer::help_button_clicked() 
{
  // Thoughts on using QMessageBox compared to help_screen?
  QMessageBox::information( 
    this, 
    tr("Help - Windows Printer"), 
    tr("Similar to the other TCP connectivity options, this connectivity method will set the ESC3 seed counter as a server that will connect to a client over TCP. "
       "This utility is intended to be paired with our Windows based label printing software.\n\n"
       "The ESC3 will act as a server which will send count results to the receiving Windows computer. The label printing software will accept "
       "the sent data as parameters that can be used to populate barcode elements on the printable labels. Data is automatically sent from the ESC3 after "
       "the counted seed are collected from the spout.\n\n"
       "The specified IPv4 and port address should reflect the target port on the Windows label printing software. To see the socket address, click on the \"Settings\" "
       "button on the bottom right of Windows printing software.") ); // Proofread
}

void windows_tcp_printer::back_button_clicked()
{  
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

//===========================================================================================================================================================//
// Function for checking that the socket is still alive //
void windows_tcp_printer::socket_status_ping()
{
	qDebug() << "socket_status_ping";
	
}

void windows_tcp_printer::connection_detected()	//WIP~~~//
{
	qDebug() << "tcp_connection_detected_signal()";
}






