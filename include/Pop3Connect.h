/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 18:38:39
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-14 00:37:27
 */ 
#pragma once
#include "MailConnect.h"
#include <string>

// 获取邮件
#define RECV_RETURN		0x0000
// 删除邮件
//#define RECV_DELETE		0X0001
// 只获取文件大小
#define RECV_SIZEONLY	0x0002

using std::shared_ptr;

class Pop3Connect : public MailConnect {
private:
	std::string account;
	std::string password;

	MailContext* task_recvs;
	int mail_start_pos;
	int* task_recvlen;
	UINT task_flag;

	BOOL login() override;
	BOOL core() override;
	BOOL quit() override;

public:
	Pop3Connect();
	Pop3Connect& operator=(Pop3Connect src);

	Pop3Connect& setAccount(std::string val) {
		account = val;
		return *this;
	}
	Pop3Connect& setPassword(std::string val) {
		password = val;
		return *this;
	}
	Pop3Connect& setStartPos(int pos) {
		mail_start_pos = pos;
		return *this;
	}
	Pop3Connect& setRecvTask(MailContext* recvs, int* recvlen, UINT flag = 0);

	BOOL checkValiable() override;

};