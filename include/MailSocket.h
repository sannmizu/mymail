/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 19:02:35
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-05 22:39:01
 */ 
#pragma once
#include "CTimeOutSocket.h"
#include "ByteStream.h"
#include <string>

#define MAIL_NOTERR			500
#define MAIL_DISCONNECT		100
#define MAIL_SENDERR		101
#define MAIL_RECVERR		102
#define MAIL_SUCCESS		200
#define MAIL_ACCOUNTERR		301
#define MAIL_PASSWORDERR	302
#define MAIL_TARGETERR		303
#define MAIL_NETERR			400



class MailSocket :
	public CTimeOutSocket
{
protected:
	ByteStream m_recvBuffer;

private:
	UINT m_timeout;
	BOOL isConnected;

	UINT m_errorNo;
	std::string m_errorMsg;

public:
	MailSocket();

	void setErrorEx(UINT no, const char* msg);
	BOOL setError(int flag, const char* msg);
	UINT getErrorNo();
	std::string getErrorMsg();
	ByteStream getRecvBuffer();

	void setTimeOutMill(UINT timeout = 5000);
	void cancelTimeOut();

	BOOL checkAlive();

	BOOL connect(LPCTSTR host, UINT port);
	// 发送一条命令并且获取返回状态，失败返回-1或者SMTPRECVERROR，成功返回响应状态码
	int sendCommand(const char* command, int command_len, const char* arg = NULL, int arg_len = 0, const char* endflag = "\r\n");
	// 发送内容并且获取返回状态，失败返回-1或者SMTPRECVERROR，成功返回响应状态码
	int sendAndGetStatus(const char* msg, int msg_len, const char* endflag = "\r\n");

	// 根据输入获取状态码，失败返回-1
	virtual int getStatus(const char* msg, int length) = 0;
public:
	// 操作加上超时
	// 直接发送一段文本，并设置是否获取返回值，不会自动添加CRLF，失败返回-1，成功返回发送字符长度
	int Send(ByteStream& stream);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);

protected:
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};

