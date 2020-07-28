/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-02 19:07:45
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-10 17:48:30
 */ 
#pragma once
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "types.h"

typedef int SOCKET;
typedef sockaddr SOCKADDR;
typedef sockaddr_in SOCKADDR_IN;

#define SOCKET_ERROR -1

class CTimeOutSocket {
private:
	CTimeOutSocket(const CTimeOutSocket& rSrc);    // no implementation
	void operator=(const CTimeOutSocket& rSrc);  // no implementation

// Construction
public:
	CTimeOutSocket();
	BOOL Create(UINT nSocketPort = 0, int nSocketType=SOCK_STREAM,
		LPCTSTR lpszSocketAddress = NULL);

// Attributes
private:
	// pthread_t m_pthListen;
public:
	SOCKET m_hSocket;

	// BOOL SetSockOpt(int nOptionName, const void* lpOptionValue,
	// 	int nOptionLen, int nLevel = SOL_SOCKET);
	BOOL GetSockOpt(int nOptionName, void* lpOptionValue,
		int* lpOptionLen, int nLevel = SOL_SOCKET);

	static int GetLastError();

// Operations
public:

	virtual BOOL Accept(CTimeOutSocket& rConnectedSocket,
		SOCKADDR* lpSockAddr = NULL, int* lpSockAddrLen = NULL);

	BOOL Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL);
	BOOL Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen);

	virtual void Close();

	BOOL Connect(LPCTSTR lpszHostAddress, UINT nHostPort);
	BOOL Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);

	int IOCtl(long lCommand, void* lpArgument);

	BOOL Listen(int nConnectionBacklog=5);

	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);

	// int ReceiveFrom(void* lpBuf, int nBufLen,
	// 	SOCKADDR* lpSockAddr, int* lpSockAddrLen);

	// enum { receives = 0, sends = 1, both = 2 };
	// BOOL ShutDown(int nHow = sends);

	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);

	// int SendTo(const void* lpBuf, int nBufLen,
	// 	UINT nHostPort, LPCTSTR lpszHostAddress = NULL);
	// int SendTo(const void* lpBuf, int nBufLen,
	// 	const SOCKADDR* lpSockAddr, int nSockAddrLen);

// Overridable callbacks
protected:
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	// virtual void OnOutOfBandData(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);

// Implementation
public:
	virtual ~CTimeOutSocket();

	BOOL Socket(int nSocketType=SOCK_STREAM,
		int nProtocolType = 0, int nAddressFormat = PF_INET);

protected:
	void SetTimeOut(int uTimeOut);
private:
	int m_TimeOut;
};