/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 19:04:49
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-14 02:16:19
 */ 
#pragma once
#include "MailConnect.h"
#include <string>

using std::shared_ptr;
class SmtpConnect : public MailConnect{
private:
	std::string target;
	MailContext context;
	std::string account;
	std::string password;

	BOOL login() override;
	BOOL core() override;
	BOOL quit() override;

public:
	SmtpConnect();
	SmtpConnect& operator=(const SmtpConnect& src);
	
	SmtpConnect& setContext(MailContext context);
	// 运行发送任务，返回TRUE表示发送成功
	SmtpConnect& setMailTo(std::string val) {
		target = val;
		return *this;
	}
	SmtpConnect& setAccount(std::string val) {
		account = val;
		return *this;
	}
	SmtpConnect& setPassword(std::string val) {
		password = val;
		return *this;
	}

	BOOL checkValiable() override;

};

