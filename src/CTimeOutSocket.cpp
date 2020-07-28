/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 14:07:09
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-14 03:07:07
 */ 
#include "CTimeOutSocket.h"
#include <strings.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <poll.h>
#include <netdb.h>

#define POLLINALL (POLLIN | POLLERR | POLLHUP)
#define POLLOUTALL (POLLOUT | POLLERR | POLLHUP)


static int errorno = 0;


BOOL getHostbyName(const char* hostName, SOCKADDR *addr, socklen_t *size)
{
    /* 即要解析的域名或主机名 */
    struct addrinfo *info = NULL;
    int ret = getaddrinfo(hostName, NULL, NULL, &info);
    if (ret != 0 || info == NULL) {
        return FALSE;
    }
    
    memcpy(addr, info->ai_addr, info->ai_addrlen);
    *size = info->ai_addrlen;
    freeaddrinfo(info);
	return TRUE;
}

int CTimeOutSocket::GetLastError()
{
    return errorno;
}

BOOL CTimeOutSocket::GetSockOpt(int nOptionName, void* lpOptionValue,
    int* lpOptionLen, int nLevel)
{
    int ret = getsockopt(m_hSocket, nLevel, nOptionName, lpOptionValue, (socklen_t *) lpOptionLen);
    if (ret < 0) {
        errorno = errno;
        return FALSE;
    }
    return TRUE;
}

CTimeOutSocket::CTimeOutSocket()
{
    m_hSocket = -1;
    m_TimeOut = -1;
}

BOOL CTimeOutSocket::Socket(int nSocketType,
    int nProtocolType, int nAddressFormat)
{
    if (m_hSocket > 0) {
        errorno = 100;
        return FALSE;
    }
    m_hSocket = socket(nAddressFormat, nSocketType, nProtocolType);
    if (m_hSocket < 0) {
        m_hSocket = -1;
        errorno = errno;
        return FALSE;
    }
}

BOOL CTimeOutSocket::Create(UINT nSocketPort, int nSocketType,
    LPCTSTR lpszSocketAddress)
{
    int ret = Socket(nSocketType);
    if (ret == FALSE) {
        return FALSE;
    }
    return Bind(nSocketPort, lpszSocketAddress);
}

BOOL CTimeOutSocket::Accept(CTimeOutSocket& rConnectedSocket,
    SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{
    int ret;
    if (m_hSocket < 0) {
        errorno = 100;
        OnConnect(100);
        return FALSE;
    }
    struct pollfd pollsock;
    pollsock.fd = m_hSocket;
    pollsock.events = POLLINALL;
    ret = poll(&pollsock, 1, m_TimeOut);
    if (ret < 0) {
        errorno = errno;
        OnConnect(errno);
        return FALSE;
    }
    if (ret == 0) {
        errorno = 200;
        OnConnect(200);
        return FALSE;
    }
    if (pollsock.revents & POLLHUP) {
        Close();
        errorno = 300;
        OnClose(300);
        return FALSE;
    }
    if (!(pollsock.revents & POLLIN)) {
        errorno = errno;
        OnConnect(errno);
        return FALSE;
    }

    int conncet = accept(m_hSocket, lpSockAddr, (socklen_t *) lpSockAddrLen);
    if (conncet < 0) {
        errorno = errno;
        OnConnect(errno);
        return FALSE;
    }
    rConnectedSocket.m_hSocket = conncet;
    OnConnect(0);
    return TRUE;
}

BOOL CTimeOutSocket::Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress)
{
    SOCKADDR_IN addr;
    bzero(&addr, sizeof(SOCKADDR_IN));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(nSocketPort);
    if (lpszSocketAddress == NULL) {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        addr.sin_addr.s_addr = inet_addr(lpszSocketAddress);
    }
    return Bind((SOCKADDR *) &addr, sizeof(addr));
}

BOOL CTimeOutSocket::Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
    if (m_hSocket < 0) {
        errorno = 100;
        return FALSE;
    }
    int ret = bind(m_hSocket, lpSockAddr, nSockAddrLen);
    if (ret < 0) {
        Close();
        errorno = errno;
        return FALSE;
    }
    return TRUE;
}

void CTimeOutSocket::Close()
{
    if (m_hSocket > 0) {
        close(m_hSocket);
    }
    m_hSocket = -1;
}

BOOL CTimeOutSocket::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
    if (lpszHostAddress == NULL) {
        return FALSE;
    }
    LOG("CTimeOutSocket", "connect");
    SOCKADDR_IN addr;
    socklen_t addr_len = sizeof(SOCKADDR_IN);
    bzero(&addr, sizeof(SOCKADDR_IN));
    getHostbyName(lpszHostAddress, (SOCKADDR *) &addr, &addr_len);
    addr.sin_port = htons(nHostPort);
    
    return Connect((SOCKADDR *) &addr, addr_len);
}

BOOL CTimeOutSocket::Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
    LOG("CTimeOutSocket", "start connect");
    int ret = connect(m_hSocket, lpSockAddr, nSockAddrLen);
    if (ret < 0) {
        LOG("CTimeOutSocket", "connect error");
        errorno = errno;
        this->OnConnect(errno);
        return FALSE;
    }
    LOG("CTimeOutSocket", "connect success");
    this->OnConnect(0);
    return TRUE;
}

BOOL CTimeOutSocket::IOCtl(long lCommand, void* lpArgument)
{
    int ret = ioctl(m_hSocket, lCommand, lpArgument);
    if (ret < 0) {
        errorno = errno;
        return ret;
    }
    return ret;
}

BOOL CTimeOutSocket::Listen(int nConnectionBacklog)
{
    if (m_hSocket < 0) {
        errorno = 100;
        return FALSE;
    }
    int ret = listen(m_hSocket, nConnectionBacklog);
    if (ret < 0) {
        errorno = errno;
        return FALSE;
    }
    return TRUE;
}

int CTimeOutSocket::Receive(void* lpBuf, int nBufLen, int nFlags)
{
    if (m_hSocket < 0) {
        return -1;
    }
    int ret;
    struct pollfd pollsock;
    pollsock.fd = m_hSocket;
    pollsock.events = POLLINALL;
    ret = poll(&pollsock, 1, m_TimeOut);
    if (ret < 0) {
        errorno = errno;
        OnConnect(errno);
        return FALSE;
    }
    if (ret == 0) {
        errorno = 200;
        OnConnect(200);
        return FALSE;
    }
    if (pollsock.revents & POLLHUP) {
        Close();
        errorno = 300;
        OnClose(300);
        return FALSE;
    }
    if (!(pollsock.revents & POLLIN)) {
        errorno = errno;
        OnConnect(errno);
        return FALSE;
    }

    ret = read(m_hSocket, lpBuf, (size_t) nBufLen);
    if (ret < 0) {
        errorno = errno;
    }
    return ret;
}

int CTimeOutSocket::Send(const void* lpBuf, int nBufLen, int nFlags)
{
    if (m_hSocket < 0) {
        return -1;
    }
    int ret;
    struct pollfd pollsock;
    pollsock.fd = m_hSocket;
    pollsock.events = POLLOUTALL;
    ret = poll(&pollsock, 1, m_TimeOut);
    if (ret < 0) {
        errorno = errno;
        OnConnect(errno);
        return FALSE;
    }
    if (ret == 0) {
        errorno = 200;
        OnConnect(200);
        return FALSE;
    }
    if (pollsock.revents & POLLHUP) {
        Close();
        errorno = 300;
        OnClose(300);
        return FALSE;
    }
    if (!(pollsock.revents & POLLOUT)) {
        errorno = errno;
        OnConnect(errno);
        return FALSE;
    }

    ret = write(m_hSocket, lpBuf, (size_t) nBufLen);
    if (ret < 0) {
        errorno = errno;
    }
    return ret;
}

void CTimeOutSocket::OnReceive(int nErrorCode) {}
void CTimeOutSocket::OnSend(int nErrorCode) {}
void CTimeOutSocket::OnAccept(int nErrorCode) {}
void CTimeOutSocket::OnConnect(int nErrorCode) {}
void CTimeOutSocket::OnClose(int nErrorCode) {}

CTimeOutSocket::~CTimeOutSocket()
{
    if (m_hSocket > 0) {
        close(m_hSocket);
    }
}

void CTimeOutSocket::SetTimeOut(int uTimeOut)
{
    m_TimeOut = uTimeOut;
}