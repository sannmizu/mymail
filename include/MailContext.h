/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 14:16:48
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-15 00:30:47
 */ 
#pragma once
#include <map>
#include <string>
#include "types.h"
#include "ByteStream.h"
#define MAX_PROPERTY_NUM 50
#define PROPRTY_BLOCK (MAX_PROPERTY_NUM/10)
#define MAIL_EMPTY	0
#define MAIL_SEND	1
#define MAIL_RECV	2

class MailContext {
public:
	// 邮件序号
	UINT id;
	// 邮件大小
	UINT size;
private:
	// BOOL status;
	// MIME所有属性
	std::map<std::string, std::string> properties;
	std::string data;
	ByteStream rawStream;

	void AppendMime(const std::string& key, const std::string& value);
	void AppendData(const std::string& data);
public:
	MailContext(const MailContext& context);
	MailContext();
	~MailContext();
	void operator=(const MailContext& context);

	BOOL isEmpty();
	ByteStream getRawStream() {
		return this->rawStream;
	}
	std::string getProperty(const std::string& property_name);
	void addProperty(const std::string& property_name, const std::string& property);
	const std::map<std::string, std::string>& getProperties();
	void addProperties(const std::map<std::string, std::string>& properties);
	// 设置/获得原始正文
	std::string getData();
	void setData(const std::string& data);

	// 设置/获得发件人
	std::string getSendMan();
	void setSendMan(const std::string& address);
	// 设置/获得邮件主题
	std::string getTitle();
	void setTitle(const std::string& title);
	// 设置/获得邮件正文加密方式
	std::string getContentEncoding();
	void setContentEncoding(const std::string& encode);
	// 获得解密正文
	std::string getContentDecoded();
	// 设置/获取时间
	std::string getDate();
	void setDate(const std::string& date);
	// 获取/设置正文格式
	std::string getContentType();
	std::string getContentSubType();
	void setContentType(std::string type);

	ByteStream toStream(BOOL addEnd = TRUE);
	void fromStream(const ByteStream& rawStream);
};