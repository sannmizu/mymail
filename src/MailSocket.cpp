/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 19:04:06
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-14 11:58:37
 */ 
#include "MailSocket.h"
#include <cstdlib>
#include <cstring>

MailSocket::MailSocket() :
	m_recvBuffer(), m_timeout(2000), m_errorNo(MAIL_DISCONNECT), m_errorMsg(""), isConnected(FALSE)
{
}

void MailSocket::setErrorEx(UINT no, const char* msg)
{
	this->m_errorNo = no;
	this->m_errorMsg = std::string(msg);
}

BOOL MailSocket::setError(int flag, const char* msg)
{
	if (flag == 0) {
		MailSocket::setErrorEx(MAIL_DISCONNECT, "disconnect");
		return FALSE;
	}
	else if (flag == SOCKET_ERROR) {
		MailSocket::setErrorEx(MAIL_NETERR, msg);
		return FALSE;
	}
	MailSocket::setErrorEx(MAIL_SUCCESS, "ok");
	return TRUE;
}

void MailSocket::setTimeOutMill(UINT timeout)
{
	this->m_timeout = timeout;
}

void MailSocket::cancelTimeOut()
{
	this->m_timeout = 0;
}

BOOL MailSocket::checkAlive()
{
	return MailSocket::sendCommand("NOOP", 4) == 250 ? TRUE : FALSE;
}

BOOL MailSocket::connect(LPCTSTR host, UINT port)
{
	m_recvBuffer.clear();
	char recvBuffer[1024];
	int ret;
	int readlen = 0;
	if (!isConnected && CTimeOutSocket::Connect(host, port) == 0) {
		setErrorEx(CTimeOutSocket::GetLastError(), "can't connect");
		return FALSE;
	}
	for (;;) {
		ret = MailSocket::Receive(recvBuffer, 1024);
		if (ret > 0) {
			m_recvBuffer.append(recvBuffer, ret);
			int err1 = CTimeOutSocket::IOCtl(FIONREAD, &readlen);
			if (err1 == 0 && readlen > 0) {
				continue;
			}
		}
		if (!MailSocket::setError(ret, "not accept response")) {
			return FALSE;
		}
		break;
	}
	return TRUE;
}

int MailSocket::sendCommand(const char* command, int command_len, const char* arg, int arg_len, const char* endflag)
{
	int status = -1;
	int sendMsgLength = command_len;
	char* sendMsgAppend;

	m_recvBuffer.clear();

	/* 设置文本 */
	sendMsgAppend = (char*)malloc(command_len + arg_len + 3);
	memcpy(sendMsgAppend, command, command_len);
	if (arg) {
		sendMsgAppend[command_len] = ' ';
		memcpy(sendMsgAppend + command_len + 1, arg, arg_len);
		sendMsgLength += (1 + arg_len);
	}
	sendMsgLength += 2;
	sendMsgAppend[sendMsgLength - 2] = '\r';
	sendMsgAppend[sendMsgLength - 1] = '\n';

	/* 发送文本并等待返回 */
	status = MailSocket::sendAndGetStatus(sendMsgAppend, sendMsgLength, endflag);

	/* 释放资源 */
	free(sendMsgAppend);
	return status;
}

int MailSocket::sendAndGetStatus(const char* msg, int msg_len, const char* endflag)
{
	int status = -1;
	int size = -1;
	char recv[128];
	int readlen;
	BOOL first = TRUE;
	CTimeOutSocket::SetTimeOut(m_timeout);
	size = CTimeOutSocket::Send(msg, msg_len);
	if (MailSocket::setError(size, "error when send")) {
		for (;;) {
			size = CTimeOutSocket::Receive(recv, 128);
			MailSocket::setError(size, "error when receive after send");
			if (first)
				status = getStatus(recv, size);
			if (size > 0) {
				m_recvBuffer.append(recv, size);
				CTimeOutSocket::IOCtl(FIONREAD, &readlen);
				if (readlen > 0) {
					first = FALSE;
					continue;
				}
				// 没见到结束标志
				if (m_recvBuffer.find(endflag) == string::npos) {
					first = FALSE;
					continue;
				}
			}
			break;
		}
	}
	return status;
}

int MailSocket::Send(ByteStream& stream)
{
	return MailSocket::Send(stream.getBytes(), stream.getLength());
}

int MailSocket::Send(const void* lpBuf, int nBufLen, int nFlags /*= 0*/)
{
	int size;
	CTimeOutSocket::SetTimeOut(m_timeout);
	size = CTimeOutSocket::Send(lpBuf, nBufLen, nFlags);
	MailSocket::setError(size, "error when send");
	return size;
}

int MailSocket::Receive(void* lpBuf, int nBufLen, int nFlags /*= 0*/)
{
	int size;
	CTimeOutSocket::SetTimeOut(m_timeout);
	size = CTimeOutSocket::Receive(lpBuf, nBufLen, nFlags);
	MailSocket::setError(size, "error when receive");
	return size;
}

UINT MailSocket::getErrorNo()
{
	return this->m_errorNo;
}

std::string MailSocket::getErrorMsg()
{
	return this->m_errorMsg;
}

ByteStream MailSocket::getRecvBuffer()
{
	return m_recvBuffer;
}

void MailSocket::OnConnect(int nErrorCode)
{
	if (nErrorCode == 0) {
		isConnected = TRUE;
	}
}

void MailSocket::OnClose(int nErrorCode)
{
	if (nErrorCode == 0) {
		isConnected = FALSE;
	}
}