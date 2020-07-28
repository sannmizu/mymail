/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 14:17:05
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-13 22:03:02
 */ 
#pragma once
#include <string>
#define BUFFER_BLOCK 1024

using std::string;

class ByteStream {
private:
	void* buffer;
	int buffer_length;
	int alloc_length;

	void copy(const void* str, int lenth);
public:
	~ByteStream();
	ByteStream();
	ByteStream(const ByteStream& src);
	ByteStream(const string& str);
	ByteStream(const char* str);
	ByteStream(const char* str, int lenth);
	ByteStream(const void* str, int lenth);

	ByteStream& operator+=(const string& src);
	ByteStream& operator=(const ByteStream& src);
	ByteStream& operator+(const ByteStream& src);
	ByteStream& operator+(const string& src);
	ByteStream& operator+(const void* src);
	ByteStream& append(const ByteStream& src);
	ByteStream& append(const string& src);
	ByteStream& append(const void* src, int length);
	void clear();

	char operator[](int pos) const;

	size_t find(char ch, int start = 0) const;
	size_t find(const char* str, int start = 0) const;
	ByteStream subStream(int start, int len) const;

	char* getBytes() const;
	int getLength() const;
};