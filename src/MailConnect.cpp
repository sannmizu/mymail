/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 19:22:00
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-15 02:36:02
 */ 
#include "MailConnect.h"
#include <pthread.h>

using std::string;

void MailConnect::setError(int no, const char* msg)
{
	this->errorNo = no;
	this->errorMsg = string(msg);
}

BOOL MailConnect::checkValiable()
{
	if (socket == nullptr) {
		return FALSE;
	}
}

BOOL MailConnect::quit()
{
	return TRUE;
}

MailConnect::MailConnect() :
	socket(nullptr), fbfunc(NULL), server_addr(""),
	port(0), mInProgress(FALSE), retryTimes(3),
	errorNo(0), errorMsg("")
{

}
MailConnect::MailConnect(shared_ptr<MailSocket> socket) : MailConnect()
{
	this->socket = socket;
}


MailConnect& MailConnect::operator=(const MailConnect& src)
{
	if (this != &src) {
		this->socket = src.socket;
		this->fbfunc = src.fbfunc;

		this->server_addr = src.server_addr;
		this->port = src.port;

		this->mInProgress = src.mInProgress;
		this->retryTimes = src.retryTimes;
		this->errorNo = src.errorNo;
		this->errorMsg = src.errorMsg;
	}
	return *this;
}

MailConnect::~MailConnect()
{

}

BOOL MailConnect::run()
{
	BOOL result = FALSE;
	int times = 0;	// 尝试次数

	LOG("MailConnect", "run");
	if (IsInProgress()) {
		setError(MAILCONN_INRUNNIG, "can't run twice one time");
		return FALSE;
	}

	if (!checkValiable()) {
		setError(MAILCONN_PREPAREERR, "miss message");
		return FALSE;
	}
	//main
	LOG("MailConnect", "connect start");
	do {
		LOG("MailConnect", "one try");
		mInProgress = TRUE;
		times++;
		// 0、创建socket
		socket->Create();
		// 1、建立连接
		result = socket->connect((LPCTSTR)server_addr.c_str(), this->port);
		if (result == FALSE) {
			socket->Close();
			setError(MAILCONN_CONNERR, "can't connect");
			continue;
		}
		LOG("MailConnect", "connect success");
		
		// 2、登陆
		if (!login()) {
			socket->Close();
			result = FALSE;
			setError(MAILCONN_LOGINERR, "error when login");
			continue;	// 可能是缓冲区的问题
		}
		LOG("MailConnect", "login success");
		
		// 3、主操作
		if (!core()) {
			socket->Close();
			result = FALSE;
			setError(MAILCONN_COREERR, "error when do core operate");
			continue;	//可能是缓冲区的问题
		}
		LOG("MailConnect", "core success");
		// 4、退出
		if (!quit()) {
			setError(MAILCONN_QUITERR, "error when quit");
		}
		socket->Close();
		result = TRUE;
		LOG("MailConnect", "quit success");
	} while (result != TRUE && times <= this->retryTimes);
	mInProgress = FALSE;
	return result;
}

void* MailSendThread(void* pParam) {
	int ret = ((MailConnect*)pParam)->run();
	if (((MailConnect*)pParam)->getFallBackFunc() != NULL) {
		((MailConnect*)pParam)->getFallBackFunc()(ret);
	}
}

BOOL MailConnect::runAsyc(fallback_handle handle)
{
	fbfunc = handle;
	pthread_t thread;
	int ret = pthread_create(&thread, NULL, MailSendThread, (void *)this);
	if (ret != 0) {
		return FALSE;
	}
	return TRUE;
}

void MailConnect::setSocket(shared_ptr<MailSocket> socket)
{
	this->socket = socket;
}
