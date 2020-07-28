/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 18:36:24
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-15 00:11:47
 */ 
#pragma once
#include <string>
#include "MailConnect.h"
#include "SmtpConnect.h"
#include "SmtpSocket.h"
#include "Pop3Socket.h"
#include "Pop3Connect.h"
class ConnectFactory {
private:
	std::string server_addr;
	UINT port;

	UINT errorNo;
	std::string errorMsg;

	/* functions */
	void setError(UINT no, const char* msg);
public:
	const static int SMTP = 1;
	const static int POP3 = 2;

	ConnectFactory();
	ConnectFactory(const std::string& server_addr);
	ConnectFactory(const char* server_addr);
	ConnectFactory& setServerAddr(const std::string& addr);
	ConnectFactory& setServerPost(UINT port);
	MailConnect* build(int protocol);
};