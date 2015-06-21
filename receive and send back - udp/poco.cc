//Receive
#include <string>
#include <iostream>
#include <list>
#include <tuple>
#include <sstream>

#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/MulticastSocket.h>

#include "creds.cc"


using namespace std;
using namespace Poco::Net;

int main()
{

	SocketAddress localhost(SERVER_IP, SERVER_PORT);
	DatagramSocket the_sock(localhost);
	
	SocketAddress client;
	
	int bytes_read = 0;
	
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
		}
	}
	return(0);
}
