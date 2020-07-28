/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 19:15:57
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-14 01:22:06
 */ 
#include "SmtpConnect.h"
#include "Utils.h"
#include <cstring>


BOOL SmtpConnect::login()
{
	int ret;
	char base64Buffer[512];
	int bufferLen = 512;
	// 1、发送HELO <arg>
	ret = socket->sendCommand("HELO", 4, account.c_str(), account.length());
	if (ret != 250) {
		return FALSE;	//可能是缓冲区的问题
	}
	// 2、发送AUTH LOGIN
	ret = socket->sendCommand("AUTH", 4, "LOGIN", 5);
	if (ret != 334) {
		return FALSE;	//可能是缓冲区的问题
	}
	// 3、发送base64加密后的账号
	Base64Encode((BYTE*)account.c_str(), account.length(), base64Buffer, &bufferLen, ATL_BASE64_FLAG_NOCRLF);
	memcpy(base64Buffer + bufferLen, "\r\n", 2);
	bufferLen += 2;
	ret = socket->sendAndGetStatus(base64Buffer, bufferLen);
	if (ret != 334) {
		;	//账号有误
	}
	// 4、发送base64加密后的密码
	bufferLen = 256;
	Base64Encode((BYTE*)password.c_str(), password.length(), base64Buffer, &bufferLen, ATL_BASE64_FLAG_NOCRLF);
	memcpy(base64Buffer + bufferLen, "\r\n", 2);
	bufferLen += 2;
	ret = socket->sendAndGetStatus(base64Buffer, bufferLen);
	if (ret != 235) {
		return FALSE;	//密码有误
	}
	return TRUE;
}

BOOL SmtpConnect::core()
{
	int ret;
	std::string splice_string("");
	ByteStream _data;
	// 1、发送MAIL FROM:<account>
	splice_string.append("FROM:<").append(account);
	splice_string.append(">");
	ret = socket->sendCommand("MAIL", 4, splice_string.c_str(), splice_string.length());
	if (ret != 250) {
		return FALSE;	//账号有误
	}
	// 2、发送RCPT TO:<target>
	splice_string.clear();
	splice_string.append("TO:<").append(target);
	splice_string.append(">");
	ret = socket->sendCommand("RCPT", 4, splice_string.c_str(), splice_string.length());
	if (ret != 250) {
		return FALSE;	//账号有误
	}
	// 3、发送DATA
	ret = socket->sendCommand("DATA", 4);
	if (ret != 354) {
		return FALSE;	//可能是缓冲区的问题
	}
	// 4、发送数据
	_data = context.toStream(TRUE);
	ret = socket->sendAndGetStatus(_data.getBytes(), _data.getLength());
	if (ret != 250) {
		return FALSE;
	}
	return TRUE;
}

BOOL SmtpConnect::quit()
{
	// 发送QUIT
	socket->sendCommand("QUIT", 4);
	return TRUE;
}

SmtpConnect::SmtpConnect() :
	MailConnect(), context(), target(), account(""), password("")
{
}

SmtpConnect& SmtpConnect::operator=(const SmtpConnect& src)
{
	if (this != &src) {
		MailConnect::operator=(src);
		this->context = src.context;
		this->target = src.target;
		this->account = src.account;
		this->password = src.password;
	}
	return *this;
}

SmtpConnect& SmtpConnect::setContext(MailContext context)
{
	this->context = context;
	this->context.setDate(getLocalTime());
	this->context.setContentEncoding("base64");
	return *this;
}

BOOL SmtpConnect::checkValiable()
{
	if (!MailConnect::checkValiable() || account.empty() || password.empty() || target.empty()) {
		return FALSE;
	}
	return TRUE;
}