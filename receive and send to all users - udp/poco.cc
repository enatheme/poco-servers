//Receive
#include <string>
#include <iostream>
#include <list>
#include <stdlib.h>

#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/MulticastSocket.h>
#include "Poco/Runnable.h"
#include <Poco/Thread.h>
#include "Poco/ThreadPool.h"
#include "Poco/Mutex.h"

#include "creds.cc"


using namespace std;
using namespace Poco::Net;
using namespace Poco;

Mutex writing_mu;
list<SocketAddress>clients_add;
string spread_message;


//sending thread function
class Sender: public Poco::Runnable
{
	public:
		Sender(DatagramSocket *the_sock)
		{
			main_sock = the_sock;
		};
		
		virtual void run()
		{
			while(1)
			{
				//if 2 messages are send in less than 1s, just the last one gonna be send, 2 mutex should be used for that
				sleep(1);
				writing_mu.lock();
				//we check if there is a message to send
				if(!(spread_message.empty()))
				{
					cout << "There is a message to send : " << spread_message << endl;
					for(list<SocketAddress>::iterator it = clients_add.begin(); it!=clients_add.end(); ++it)
					{
						SocketAddress temp = *it;
						cout << "Message send to " << temp.host().toString() << ":" << temp.port() << endl;
						main_sock->sendTo(spread_message.c_str(), spread_message.length(), temp, 0);
					}
					spread_message.clear();
				}
				writing_mu.unlock();
					
			}
		}
		
	private:
		DatagramSocket *main_sock;
};

//Client thread function
class Client: public Poco::Runnable
{
	public:
		Client(){};
		//Client(int* the_id, SocketAddress* the_client_sock_add, SocketAddress* the_client){};
		Client(int the_id, SocketAddress the_client_sock_add, SocketAddress the_client)
		{
			id = the_id;
			client = the_client;
			//initialisation of the socket
			DatagramSocket temp(the_client_sock_add);
			client_sock = temp;
			//welcome message
			cout << "Welcome message sended !" << endl;
			//string welcome = "Welcome there, please listen and speak on the port " + to_string(the_client_sock_add.port());
			string welcome = to_string(the_client_sock_add.port());
			client_sock.sendTo(welcome.c_str(), welcome.length(), the_client, 0);
		};
		
		~Client()
		{
			cout << "Destroyed" << endl;
		};
		
		virtual void run()
		{			
			int bytes_read = 0;

			cout << "New client with the id : " << id << " from " << client.host().toString() << ":" << client.port() << endl;
			while(true)
			{
				char buffer[254] = {};
				//BLOCKING FUNCTION !
				bytes_read = client_sock.receiveFrom(buffer, 254, client);
				if (bytes_read != 0)
				{
					cout << buffer << " from " << client.host().toString() << ":" << client.port() << endl;
					writing_mu.lock();
					spread_message = string(buffer) + " from " + client.host().toString() + to_string(client.port());
					writing_mu.unlock();

				}
			}
		}
		
	
	private:
		int id;
		DatagramSocket client_sock;
		SocketAddress client;
};

int main()
{
	//variables
	
	//client gate
	SocketAddress localhost(SERVER_IP, SERVER_PORT_CONNECTION);
	DatagramSocket main_sock(localhost);
	
	SocketAddress incomming_client;
	
	int next_port = SERVER_PORT_MIN;
	
	//all client pool
	int nb_clients = 0; //the id is defined to a client as the number in is entry in the server. deconnected id's are not used after (it's a poc, not a real optimized server)
	ThreadPool all_clients_pool;
	Client array_clients[SERVER_PORT_MAX - SERVER_PORT_MIN];

	


	//sender thread initialition
	Sender runnable(&main_sock);
	Thread spread_thread;
	spread_thread.start(runnable);


	string err_msg = "Error, you should send \"connection\\n\" to the server to be connected";
	int bytes_read = 0;
	//------------------------//
	
	cout << "server online at " << SERVER_IP << ":" << SERVER_PORT_CONNECTION << " and listening !" << endl;
	
	while(next_port < SERVER_PORT_MAX)
	{
		cout << "waiting message" << endl;
		char buffer[254] = {};
		//BLOCKING FUNCTION !
		bytes_read = main_sock.receiveFrom(buffer, 254, incomming_client);
		if (bytes_read != 0)
		{
			if (string(buffer).compare("connection"))
			{
				//we create the socketaddress
				SocketAddress temp_host(SERVER_IP, next_port);
				cout << "New client from " << incomming_client.host().toString() << ":" << incomming_client.port() << " sended on the port " << next_port << endl;
				
				//client thread initialition
				array_clients[nb_clients] = Client(nb_clients, temp_host, incomming_client);
				all_clients_pool.start(array_clients[nb_clients]);
				nb_clients++;
				next_port++;
				
				//we had it to the list of client
				clients_add.push_back(incomming_client);
			}
			else
			{
				cout << "Bad connection message : \"" << buffer << "\" from " << incomming_client.host().toString() << ":" << incomming_client.port() << endl;
				main_sock.sendTo(err_msg.c_str(), err_msg.length(), incomming_client, 0);
			}

		}
	}
	return(0);
}
