/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 19:21:32
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-14 02:54:27
 */ 
#include "Utils.h"
#include <iconv.h>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <cerrno>
#include <ctime>

static const std::string COMPARE_PREFIX = "=?";

int remove_all(std::string& str, const char ch) {
	int num = 0;
	std::string::size_type pos(0);
	while (1) {
		pos = str.find(ch, pos);
		if(pos != std::string::npos) {
			str.erase(pos, 1);
			num++;
		} else {
			break;
		}
    }
	return num;
}

std::vector<std::string> split(const std::string& str, const std::string& pattern)
{
	using std::vector;
	using std::string;
	vector<string> resVec;

	if ("" == str)
	{
		return resVec;
	}
	//方便截取最后一段数据
	string strs = str + pattern;
	
	size_t pos = strs.find(pattern);
	size_t pattern_len = pattern.size();
	size_t size = strs.size();

	while (pos != string::npos)
	{
		string x = strs.substr(0, pos);
		if (!x.empty())
			resVec.push_back(x);
		strs = strs.substr(pos + pattern_len, size);
		pos = strs.find(pattern);
	}

	return resVec;
}

std::string base64EncodeCpp(const std::string& str) {
	std::string result;
	char buffer[800];
	int buffer_len = 800;
	int pos = 0;
	int str_len = str.length();
	while (pos < str_len) {
		buffer_len = 800;
		int src_len = str_len - pos > 570 ? 570 : str_len - pos;
		int ret = Base64Encode((const BYTE *) str.c_str() + pos, src_len, buffer, &buffer_len);
		if (ret == FALSE) {
			return std::string();
		}
		result.append(buffer, buffer_len);
		pos+=src_len;
	}
	return result;
}

std::string base64DecodeCpp(const std::string& str) {
	std::string tmp = str;
	remove_all(tmp, '\n');

	std::string result;
	char buffer[600];
	int buffer_len = 600;
	int pos = 0;
	int str_len = tmp.length();
	while (pos < str_len) {
		buffer_len = 600;
		int src_len = str_len - pos > 760 ? 760 : str_len - pos;
		int ret = Base64Decode((LPCSTR) tmp.c_str() + pos, src_len, (BYTE *) buffer, &buffer_len);
		if (ret == FALSE) {
			return std::string();
		}
		result.append(buffer, buffer_len);
		pos+=src_len;
	}
	return result;
}

std::string mulBase64DecodeCpp(const std::string& str) {
	size_t pos = str.find(COMPARE_PREFIX);
	if (pos == std::string::npos) {
		return str;
	}
	std::string result = str.substr(0, pos);
	std::vector<std::string> parts = split(str.substr(pos), "?");
	int part_len = parts.size() / 4;
	for (int i = 0; i < part_len; i++) {
		if (parts[4 * i + 2][0] == 'B' || parts[4 * i + 2][0] == 'b') {
			std::string decodeStr = base64DecodeCpp(parts[4 * i + 3]);
			result.append(charConvert(decodeStr, parts[4 * i + 1], "UTF-8"));
		} else {
			result.append(parts[4 * i + 3]);
		}
	}
	return result.empty() ? str : result;
}

int convert(const char* from_charset, const char* to_charset,
		char *inbuf, size_t inlen, char *outbuf, size_t& outlen) {
	iconv_t cd = iconv_open(to_charset, from_charset);
	char **pin = &inbuf;
	char **pout = &outbuf;
	
	memset(outbuf, 0, outlen);
	int ret = iconv(cd, pin, &inlen, pout, &outlen);
	iconv_close(cd);
	return ret;
}

std::string gbk2utf8(const std::string& str) {
	return charConvert(str, "GB18030", "UTF-8");
}

std::string utf82gbk(const std::string& str) {
	return charConvert(str, "UTF-8", "GB18030");
}

std::string charConvert(const std::string& str, const std::string& from, const std::string& to) {
	size_t in_len = (size_t) str.length();
	size_t out_len = in_len * 4;
	size_t old_out_len = out_len;
	
	char *in_buffer = (char *) malloc(in_len * sizeof(char));
	if (in_buffer == NULL) {
		return "";
	}
	char *out_buffer = (char *) malloc(out_len * sizeof(char));
	if (out_buffer == NULL) {
		free(in_buffer);
		return "";
	}

	memcpy(in_buffer, str.c_str(), in_len);
	if (from.compare(0, 2, "GB") == 0) {
		convert("GB18030", to.c_str(), in_buffer, in_len, out_buffer, out_len);
	} else {
		convert(from.c_str(), to.c_str(), in_buffer, in_len, out_buffer, out_len);
	}
	std::string result;
	result.append(out_buffer, old_out_len - out_len);
	
	free(in_buffer);
	free(out_buffer);
	return result;
}

std::string getLocalTime() {
	time_t timep;
    time (&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );
    return tmp;
}

std::string tryEncodeHtmldata(const std::string& str) {
	size_t pos = str.find("charset");
	if (pos == std::string::npos) {
		return str;
	}
	size_t pos2 = str.find_first_of("=", pos);
	if (pos2 == std::string::npos || pos2 - pos > 10) {
		return str;
	}
	pos = str.find_first_of("/>\"", pos2);
	if (pos == std::string::npos || pos - pos2 > 20) {
		return str;
	}
	std::string sub = str.substr(pos2 + 1, pos - pos2 - 1);
	remove_all(sub, ' ');
	remove_all(sub, '\"');
	if (sub.length() == 0) {
		return str;
	}
	return charConvert(str, sub, "UTF-8");
}