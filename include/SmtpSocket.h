#pragma once
#include "MailSocket.h"
class SmtpSocket :
	public MailSocket
{

public:
	int getStatus(const char* msg, int length) override;

};

