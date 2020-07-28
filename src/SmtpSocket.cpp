/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 19:21:14
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-05 19:21:15
 */ 
#include "SmtpSocket.h"
#include "Utils.h"

int SmtpSocket::getStatus(const char* msg, int length)
{
	if (msg == NULL || length < 0)
		return -1;
	int status;
	std::string str = msg;
	str[length] = 0;
	std::vector<std::string> v = ::split(str, " ");
	if (v.size() != 0) {
		status = ::atoi(v[0].c_str());
		status = status == 0 ? -1 : status;
	}
	else {
		status = -1;
	}
	return status;
}