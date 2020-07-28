vpath %.h ./include
vpath %.cpp ./src
CC = g++
CFLAGS = -I./include -include log.h
LIB = -lpthread
OBJS = main.o ByteStream.o ConnectFactory.o CTimeOutSocket.o MailContext.o MailConnect.o MailSocket.o Pop3Connect.o \
		Pop3Socket.o SmtpConnect.o SmtpSocket.o Utils.o

all:main clean

main:$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -o main

$(OBJS):%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -f $(OBJS)

cleanall:
	rm -f $(OBJS) main