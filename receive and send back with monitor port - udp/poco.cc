//Receive
#include <string>
#include <iostream>

#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/MulticastSocket.h>
#include <Poco/Thread.h>

#include "creds.cc"


using namespace std;
using namespace Poco::Net;
using namespace Poco;


//Monitor thread function
class Monitor_function: public Poco::Runnable
{
	public:
		Monitor_function(int* monitor_connected, DatagramSocket* the_monitor_sock, SocketAddress* the_monitor)
		{
				is_connected = monitor_connected;
				monitor_sock = the_monitor_sock;
				monitor = the_monitor;
		};
	void run()
	{
	
		int bytes_read = 0;

		cout << "MONITORED !" << endl;	
		while(true)
		{
			char buffer[254] = {};
			//BLOCKING FUNCTION !
			bytes_read = monitor_sock->receiveFrom(buffer, 254, *monitor);
			if (bytes_read != 0)
			{
				//to be connected, the monitor should send connection
				if (string(buffer).compare("connection"))
				{
					*is_connected = 1;
					cout << "A monitor is connected from " << monitor->host().toString() << ":" << monitor->port() << endl;
				}
				//if the monitor would leave
				else if(buffer == "quit")
				{
					*is_connected = 0;
				}
			}
		}
	}
	
	private:
		int* is_connected;
		DatagramSocket* monitor_sock;
		 SocketAddress* monitor;
		
	
	
	
};

int main()
{
	//variables
	
	//client gate
	SocketAddress localhost(SERVER_IP, SERVER_PORT);
	DatagramSocket the_sock(localhost);
	
	SocketAddress client;
	
	//monitor gate
	SocketAddress monitorhost(SERVER_IP, MONITOR_PORT);
	DatagramSocket monitor_sock(monitorhost);
	
	SocketAddress monitor;
	
	int monitor_connected = 0;
	
	//monitor thread initialition
	Monitor_function runnable(&monitor_connected, &monitor_sock, &monitor);
	Thread monitor_thread;
	monitor_thread.start(runnable);
	
	
	
	
	int bytes_read = 0;
	
	
	
	
  //_______________________________________________________________________________________________________________________\\
 //_________________________________________________________________________________________________________________________\\	
//___________________________________________________________________________________________________________________________\\	
	
	
	cout << "server online at " << SERVER_IP << ":" << SERVER_PORT << " and listening !" << endl;
	
	while(true)
	{
		char buffer[254] = {};
		//BLOCKING FUNCTION !
		bytes_read = the_sock.receiveFrom(buffer, 254, client);
		if (bytes_read != 0)
		{
			cout << buffer << " from " << client.host().toString() << ":" << client.port() << endl;
			the_sock.sendTo(buffer, bytes_read, client, 0);
			
			//if the connection is monitored
			if(monitor_connected == 1)
			{
				string data = string(buffer) + " from  " + client.host().toString() + ":" + to_string(client.port());
				monitor_sock.sendTo(data.c_str(), data.length(), monitor, 0);
			}
		}
	}
	return(0);
}
