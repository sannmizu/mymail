/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-02 18:50:39
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-15 02:34:16
 */ 
#include "MailContext.h"
#include "Utils.h"
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;


BOOL IsAsciiStr(const string& str) {
	int len = str.length();
	for (int i = 0; i < len; i++) {
		if (str[i] < 33 || str[i] > 126)
			return FALSE;
	}
	return TRUE;
}

BOOL IsIncludeCRLF(string& str) {
	int len = str.length();
	for (int i = 0; i < len; i++) {
		if (str[i] == '\r' || str[i] == '\n')
			return TRUE;
	}
	return FALSE;
}

void RemoveChar(string& str, char ch) {
	string::iterator it;
	for(it = str.begin(); it != str.end(); it++) {
		if(*it == ch) {
			str.erase(it);
			it--;
		}
	}
}

void MailContext::AppendMime(const string& key, const string& value)
{
	// 判断数据是否合法
	if (key.empty())
		return;
	if (value.empty() && !IsAsciiStr(key))
		return;
	string _value(value);
	RemoveChar(_value, '\r');
	RemoveChar(_value, '\n');

	rawStream.append(key).append(": ", 2);
	// 分割
	int per_row = 70;
	int value_len = _value.length();
	int rows = value_len / per_row + 1;
	for (int row = 0; row < rows - 1; row++) {
		rawStream.append(_value.substr(row * per_row, per_row));
		rawStream.append("\r\n\t", 3);
	}
	rawStream.append(_value.substr(per_row * (rows - 1)));
	rawStream.append("\r\n", 2);
}

void MailContext::AppendData(const string& data)
{
	if (getContentEncoding().compare(0, 6, "base64") == 0) {
		string _data = base64EncodeCpp(data);
		rawStream.append("\r\n", 2).append(_data);
	} else {
		string _data(data);
		if (!_data.empty()) {
			// 插入两个分行，因为结尾有一个分行了，只需要再插入一个
			rawStream.append("\r\n", 2);
			// 处理正文中只有'.'的行
			int i, len;
			len = _data.length();
			for (i = 0; i < len; i++) {
				if (_data[i] == '\r') {
					if (_data[i + 1] == '\n' && _data[i + 2] == '.' && _data[i + 3] == '\r' && _data[i + 4] == '\n') {
						// 插入一个'.'
						_data.insert(i + 2, 1, '.');
					}
				}
			}
			// 放入处理后的正文
			rawStream.append(_data);
		}
	}
}

MailContext::MailContext(const MailContext& context)
{
	*this = context;
}

MailContext::MailContext() :
	data(""), properties(), //, status(MAIL_EMPTY)
	rawStream(), id(0), size(0)
{
	
}

MailContext::~MailContext()
{
}

void MailContext::operator=(const MailContext& context)
{
	if (this != &context) {
		this->id = context.id;
		this->size = context.size;
		this->properties = context.properties;
		this->setData(context.data);
		this->rawStream = context.rawStream;
		//this->status = context.status;
	}
}

BOOL MailContext::isEmpty()
{
	BOOL empty = TRUE;
	if (!this->properties.empty())
		empty = FALSE;
	if (!this->data.empty())
		empty = FALSE;
	return empty;
}

string MailContext::getProperty(const string& property_name)
{
	std::map<string, string>::iterator it = properties.find(property_name);
	if (it != properties.end()) {
		return (*it).second;
	}
	return "";
}

void MailContext::addProperty(const string& property_name, const string& property_value)
{
	properties.insert(std::make_pair(property_name, property_value));
}

const std::map<string, string>& MailContext::getProperties()
{
	return properties;
}

void MailContext::addProperties(const std::map<string, string>& properties)
{
	this->properties.insert(properties.begin(), properties.end());
}

string MailContext::getData()
{
	return this->data;
}

void MailContext::setData(const string& data)
{
	this->data = data;
}

string MailContext::getSendMan()
{
	string raw_string = getProperty("From");
	return mulBase64DecodeCpp(raw_string);
}

void MailContext::setSendMan(const string& address)
{
	string encode_address = string();
	encode_address.append("=?GBK?B?").append(base64EncodeCpp(utf82gbk(address))).append("?=");
	addProperty("From", encode_address);
}

string MailContext::getTitle() {
	string raw_title = getProperty("Subject");
	return mulBase64DecodeCpp(raw_title);
}

void MailContext::setTitle(const string& title) {
	string encode_title = string();
	encode_title.append("=?GBK?B?").append(base64EncodeCpp(utf82gbk(title))).append("?=");
	addProperty("Subject", encode_title);
}

string MailContext::getContentEncoding() {
	return getProperty("Content-Transfer-Encoding");
}

void MailContext::setContentEncoding(const string& encode) {
	addProperty("Content-Transfer-Encoding", encode);
}

string MailContext::getContentDecoded() {
	if (getProperty("Content-Transfer-Encoding").compare(0, 6, "base64") == 0) {
		return base64DecodeCpp(this->data);
	}
	return this->data;
}

string MailContext::getDate() {
	return getProperty("Date");
}

void MailContext::setDate(const string& date) {
	addProperty("Date", date);
}

string MailContext::getContentType() {
	return getProperty("Content-Type");
}

string MailContext::getContentSubType() {
	string type = getContentType();
	vector<string> parts = split(type, "/");
	if (parts.size() < 2) {
		return type;
	}
	return parts[1];
}

void MailContext::setContentType(string type) {
	addProperty("Content-Type", type);
}

ByteStream MailContext::toStream(BOOL addEnd)
{
	if (isEmpty())
		return nullptr;
	// 重置rawStream
	rawStream.clear();
	// 插入所有MIME属性
	string key;
	string value;
	std::map<string, string>::iterator it = properties.begin();
	for (it; it != properties.end(); it++) {
		AppendMime((*it).first, (*it).second);
	}
	// 插入正文
	AppendData(data);
	if (addEnd)
		rawStream.append("\r\n.\r\n", 5);
	return ByteStream(rawStream);
}

void MailContext::fromStream(const ByteStream& raw)
{
	using std::string;
	this->rawStream = raw;
	// 先拆成MIME信息头和正文两部分
	int pos;
	string mimeStream;
	string dataStream;
	string temp;
	pos = raw.find("\r\n\r\n");
	if (pos != string::npos) {
		// 处理正文部分
		dataStream = string(raw.getBytes() + pos + 4, raw.getLength() - pos - 4);
		setData(dataStream);
	}
	mimeStream = string(raw.getBytes(), pos >= 0 ? pos : raw.getLength());
	// 处理MIME部分
	temp = "";
	for (;;) {
		pos = mimeStream.find("\r\n");
		if (pos == string::npos) {
			break;
		}
		if (mimeStream[pos + 2] == '\t' || mimeStream[pos + 2] == ' ') {
			temp.append(mimeStream.substr(0, pos));
			mimeStream = mimeStream.substr(pos + 2);
			mimeStream.erase(0, mimeStream.find_first_not_of(" \t"));
			continue;
		}
		else {
			temp.append(mimeStream.substr(0, pos));
			mimeStream = mimeStream.substr(pos + 2);
			pos = temp.find(':');
			string key = temp.substr(0, pos);
			string value = temp.substr(pos + 1);
			addProperty(key, value.erase(0, value.find_first_not_of(" ")));
			temp.clear();
		}
	}
}
