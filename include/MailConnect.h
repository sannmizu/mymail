/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 18:44:43
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-05 18:55:39
 */ 
#pragma once
#include <memory>
#include <string>
#include "MailContext.h"
#include "MailSocket.h"

#define MAILCONN_SUCCESS	114514
#define MAILCONN_CONNERR	10000
#define MAILCONN_PREPAREERR	10001
#define MAILCONN_LOGINERR	10002
#define MAILCONN_COREERR	10003
#define MAILCONN_QUITERR	10004

#define MAILCONN_INRUNNIG	20000

typedef void (*fallback_handle)(int);

using std::shared_ptr;

class MailConnect
{
protected:
	shared_ptr<MailSocket> socket;
	fallback_handle fbfunc;
	
	std::string server_addr;
	UINT port;

private:

	BOOL mInProgress;
	int retryTimes;
	int errorNo;
	std::string errorMsg;

	void setError(int no, const char* msg);

	BOOL heart();

	virtual BOOL login() = 0;
	virtual BOOL core() = 0;
	virtual BOOL quit();
public:
	virtual BOOL checkValiable();
	MailConnect();
	MailConnect(shared_ptr<MailSocket> socket);
	MailConnect& operator=(const MailConnect& src);
	~MailConnect();

	BOOL run();
	// 开线程运行任务，完成后调用回调函数
	BOOL runAsyc(fallback_handle handle);

	void setSocket(shared_ptr<MailSocket> socket);
	int getErrorno() const { return errorNo; }
	std::string getErrorMsg() const { return errorMsg; }
	BOOL IsInProgress() const { return mInProgress; }
	fallback_handle getFallBackFunc() { return fbfunc; }
	void setServerAddr(std::string val) { server_addr = val; }
	void setServerPort(UINT val) { port = val; }
};

