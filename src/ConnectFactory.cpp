/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 14:16:16
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-14 12:51:26
 */ 
#include "ConnectFactory.h"
using std::string;

void ConnectFactory::setError(UINT no, const char* msg)
{
	this->errorNo = no;
	this->errorMsg = string(msg);
}

ConnectFactory::ConnectFactory() :
    server_addr(""), port(25), errorNo(0), errorMsg("")
{
}

ConnectFactory::ConnectFactory(const string& addr) : port(25), errorNo(0)
{
    server_addr = addr;
}


ConnectFactory::ConnectFactory(const char* addr) : port(25), errorNo(0)
{
    server_addr = string(addr);
}

ConnectFactory& ConnectFactory::setServerAddr(const string& server_addr)
{
	this->server_addr = server_addr;
	return *this;
}

ConnectFactory& ConnectFactory::setServerPost(UINT port)
{
	this->port = port;
	return *this;
}

MailConnect* ConnectFactory::build(int classname)
{
    char buffer[128];
    int ret; 
	MailConnect *retConn;
	if (classname == SMTP) {
		LOG("ConnectFactory", "create SMTP");
		shared_ptr<SmtpSocket> socket(new SmtpSocket());
		retConn = new SmtpConnect();
		if (socket == nullptr) {
			this->setError(1, "malloc error");
		}
		else {
			LOG("ConnectFactory", "create socket");
			if (socket->Create() == FALSE) {
				LOG("ConnectFactory", "create socket fail");
				this->setError(socket->GetLastError(), "error when create a socket");
				socket.reset();
				socket = nullptr;
			} else {
				LOG("ConnectFactory", "create socket success");
			}
		}
		retConn->setSocket(socket);
		retConn->setServerAddr(server_addr);
		retConn->setServerPort(25);
		LOG("ConnectFactory", "create SmtpConnect success");
		return retConn;
	}
	else if (classname == POP3) {
		LOG("ConnectFactory", "create POP3");
		shared_ptr<Pop3Socket> socket(new Pop3Socket());
		retConn = new Pop3Connect();
		if (socket == nullptr) {
			this->setError(1, "malloc error");
		}
		else {
			if (socket->Create() == FALSE) {
				this->setError(socket->GetLastError(), "error when create a socket");
				socket.reset();
				socket = nullptr;
			}
		}
		retConn->setSocket(socket);
		retConn->setServerAddr(server_addr);
		retConn->setServerPort(110);
		return retConn;
	}
	return nullptr;
}
