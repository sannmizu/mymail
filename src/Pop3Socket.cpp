/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 19:21:07
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-05 19:21:08
 */ 
#include "Pop3Socket.h"
#include "Utils.h"

int Pop3Socket::getStatus(const char* msg, int length)
{
	if (msg == NULL || length < 0)
		return -1;
	int status;
	std::string str = msg;
	str[length] = 0;
	std::vector<std::string> v = ::split(str, " ");
	if (v.size() != 0) {
		v[0].erase(0, v[0].find_first_not_of(" "));
		status = v[0][0] == '+' ? 200 : 400;
	}
	else {
		status = -1;
	}
	return status;
}
