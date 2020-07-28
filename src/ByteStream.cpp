/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-05 19:55:53
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-13 22:53:29
 */ 
#include "ByteStream.h"
#include "Utils.h"
#include <cstring>
#include <cstdlib>

ByteStream::ByteStream()
{
	buffer = malloc(BUFFER_BLOCK);
	memset(buffer, 0, BUFFER_BLOCK);
	buffer_length = 0;
	alloc_length = BUFFER_BLOCK;
}

ByteStream::ByteStream(const ByteStream& src) : ByteStream()
{
	if (src.buffer) {
		buffer = malloc(BUFFER_BLOCK);
		memset(buffer, 0, BUFFER_BLOCK);
		buffer_length = 0;
		alloc_length = BUFFER_BLOCK;
		copy(src.buffer, src.buffer_length);
	}
}

ByteStream::ByteStream(const string& str1) : ByteStream()
{
	copy(str1.c_str(), str1.length());
}

ByteStream::ByteStream(const char* str) : ByteStream()
{
	copy(str, strlen(str));
}

ByteStream::ByteStream(const char* str, int lenth) : ByteStream()
{
	copy(str, lenth);
}

ByteStream::ByteStream(const void* str, int lenth) : ByteStream()
{
	copy(str, lenth);
}

ByteStream& ByteStream::operator=(const ByteStream& src)
{
	copy(src.buffer, src.buffer_length);
	return *this;
}

ByteStream& ByteStream::operator+=(const string& src)
{
	append(src);
	return *this;
}

void ByteStream::copy(const void* str, int lenth)
{
	if (str != NULL) {
		buffer_length = 0;
		append(str, lenth);
	}
}

ByteStream::~ByteStream()
{
	if (buffer)
		free(buffer);
	buffer = nullptr;
}

ByteStream& ByteStream::operator+(const void* src)
{
	ByteStream::append(src, strlen((char*)src));
	return *this;
}

ByteStream& ByteStream::operator+(const ByteStream& src)
{
	ByteStream::append(src.getBytes(), src.getLength());
	return *this;
}

ByteStream& ByteStream::operator+(const string& src)
{
	ByteStream::append(src);
	return *this;
}

ByteStream& ByteStream::append(const void* src, int length)
{
	if (alloc_length < (buffer_length + length)) {
		do {
			alloc_length *= 2;
		} while (alloc_length <= (buffer_length + length));
		buffer = realloc(buffer, alloc_length);
	}
	memcpy((char*)buffer + buffer_length, src, length);
	buffer_length += length;
	((char *) buffer)[buffer_length] = 0;
	return *this;
}

ByteStream& ByteStream::append(const string& src)
{
	ByteStream::append(src.c_str(), src.length());
	return *this;
}

ByteStream& ByteStream::append(const ByteStream& src)
{
	ByteStream::append(src.buffer, src.buffer_length);
	return *this;
}

void ByteStream::clear()
{
	buffer_length = 0;
	((char *) buffer)[buffer_length] = 0;
	alloc_length = BUFFER_BLOCK;
}

char ByteStream::operator[](int pos) const
{
	return ((char*)buffer)[pos];
}

size_t ByteStream::find(char ch, int start /*= 0*/) const
{
	int i = start;
	for (i; i < buffer_length; i++) {
		if (((char*)buffer)[i] == ch)
			return i;
	}
	return (size_t) -1;
}

size_t ByteStream::find(const char* str, int start /*= 0*/) const
{
	// 不写算法了，算法课写过了
	using std::string;
	int i = start;
	string temp = string((char*)buffer, buffer_length);
	return temp.find(str, start);
}

ByteStream ByteStream::subStream(int start, int len) const
{
	if (start > buffer_length)
		return ByteStream("");
	if (start + len > buffer_length)
		return ByteStream((char*)buffer + start, buffer_length - start);
	return ByteStream((char*)buffer + start, len);
}

char* ByteStream::getBytes() const
{
	return (char*)buffer;
}

int ByteStream::getLength() const
{
	return buffer_length;
}
