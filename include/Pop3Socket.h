#pragma once
#include "MailSocket.h"
class Pop3Socket :
	public MailSocket
{
	
public:
	int getStatus(const char* msg, int length) override;

};

