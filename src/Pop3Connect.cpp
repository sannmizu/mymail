/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 19:01:42
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-14 11:54:31
 */ 
#include "Pop3Connect.h"
#include "Utils.h"
#include <list>
#include <vector>
#include <cstring>
#include <cstdlib>

BOOL Pop3Connect::login()
{
	int ret;
	// 1、发送USER <account>
	ret = socket->sendCommand("USER", 4, account.c_str(), account.length());
	if (ret != 200) {
		return FALSE;
	}
	// 2、发送PASS <password>
	ret = socket->sendCommand("PASS", 4, password.c_str(), password.length());
	if (ret != 200) {
		return FALSE;
	}
	return TRUE;
}

BOOL Pop3Connect::core()
{
	using std::vector;
	using std::string;
	ByteStream rawStream;
	int ret;
	LOG("Pop3Connect", "core");
	// 1、发送LIST
	ret = socket->sendCommand("LIST", 4);
	if (ret != 200) {
		return FALSE;
	}
	// 1.1、获取邮件列表
	// <CRLF>为分割，<CRLF>.<CRLF>为结束
	vector<string> split_list;
	vector<string>::reverse_iterator it;
	int list_size = 0;
	rawStream = socket->getRecvBuffer();
	split_list = split(string(rawStream.getBytes(), rawStream.getLength()), "\r\n");
	split_list.erase(split_list.begin());
	split_list.pop_back();
	it = split_list.rbegin();
	for (int i = 0; i < mail_start_pos && it != split_list.rend(); i++) {
		it++;
	}
	for (it; it != split_list.rend(); it++) {
		if (task_recvs == NULL || task_recvlen == NULL) {
			return FALSE;
		}
		if (*task_recvlen > list_size) {
			int _split = (*it).find(' ');
			task_recvs[list_size].id = atoi((*it).substr(0, _split).c_str());
			task_recvs[list_size].size = atoi((*it).substr(_split + 1, (*it).length()).c_str());
			list_size++;
		} else {
			break;
		}
	}
	*task_recvlen = list_size;
	if (task_flag & RECV_SIZEONLY)
		return TRUE;
	// 2、发送RETR n 获取所有邮件信息
	char integerBuffer[10];
	memset(integerBuffer, 0, 10);
	int recieve_num = 0;
	for (int i = 0; i < *task_recvlen; i++) {
		sprintf(integerBuffer, "%d", task_recvs[i].id);
		LOG("Pop3Connect", "start receive one mail");
		ret = socket->sendCommand("RETR", 4, integerBuffer, strlen(integerBuffer), "\r\n.\r\n");
		if (ret != 200) {
			continue;
		}
		LOG("Pop3Connect", "receive one mail");
		// 获取rawStream
		rawStream = socket->getRecvBuffer();
		// 去除第一行返回信息
		string temp = string(rawStream.getBytes(), rawStream.getLength());
		temp = temp.substr(temp.find("\r\n"), temp.length());
		task_recvs[recieve_num].fromStream(ByteStream(temp.c_str(), temp.length()));
		recieve_num++;
	}
	*task_recvlen = recieve_num;
}

BOOL Pop3Connect::quit()
{
	int ret;
	// 发送QUIT
	ret = socket->sendCommand("QUIT", 4);
	// 如果有返回，再发送一次
	if (ret == 200) {
		socket->sendCommand("QUIT", 4);
	}
	return TRUE;
}

Pop3Connect::Pop3Connect() :
	MailConnect(), account(""), password(""), task_recvs(NULL), task_recvlen(0),
	task_flag(0), mail_start_pos(0)
{

}

Pop3Connect& Pop3Connect::operator=(Pop3Connect src)
{
	if (this == &src) {
		this->operator=(src);
		this->account = src.account;
		this->password = src.password;
		this->mail_start_pos = src.mail_start_pos;
	}
	return *this;
}

Pop3Connect& Pop3Connect::setRecvTask(MailContext* recvs, int* recvlen, UINT flag /*= 0*/)
{
	this->task_recvs = recvs;
	this->task_recvlen = recvlen;
	this->task_flag = flag;
	return *this;
}

BOOL Pop3Connect::checkValiable()
{
	if (!MailConnect::checkValiable() || account.empty() || password.empty()) {
		return FALSE;
	}
	return TRUE;
}
