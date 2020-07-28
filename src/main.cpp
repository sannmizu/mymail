/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-02 18:51:14
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-15 02:24:25
 */ 
#include "main.h"
#include "MyMail.h"
#include "Utils.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <string>
#include <signal.h>
// #include <termios.h>

#define ESC     "\033"
#define UP      "\033[A"
#define DOWN    "\033[B"
#define LEFT    "\033[D"
#define RIGHT   "\033[C"

#define NONE_OPT    -1
#define EXIT_OPT	0
#define	UP_OPT		1
#define DOWN_OPT	2
#define LEFT_OPT	3
#define RIGHT_OPT	4
#define SAVE_OPT	5
#define ESC_OPT		9
#define CHECK_OPT	10

using std::string;
using std::vector;

vector<MailContext> recevie_mail_list;

const string smtp_server_list[4] =
{
	"smtp.whu.edu.cn",
	"smtp.163.com",
	"smtp.sina.com.cn",
	"smtp.qq.com"
};
const string pop3_server_list[4] =
{
	"pop.whu.edu.cn",
	"pop.163.com",
	"pop.sina.com.cn",
	"pop.qq.com"
};

int operate = NONE_OPT;
int refresh = FALSE;

const char* smtp_list_name[7] = {
	"邮件服务器", "账号", "密码", "收件人账号", "邮件标题", "发件人姓名", "邮件内容"
};
const char* pop3_list_name[3] = {
	"邮件服务器", "账号", "密码"
};

string smtp_list[7] = {
	string(), string(), string(), string(), string(), string(), string()
};
string pop3_list[3] = {
	string(), string(), string()
};

#define smtp_server 	smtp_list[0]
#define smtp_account 	smtp_list[1]
#define smtp_password 	smtp_list[2]
#define smtp_target 	smtp_list[3]
#define smtp_title 		smtp_list[4]
#define smtp_sendMan	smtp_list[5]
#define smtp_content 	smtp_list[6]

#define pop3_server 	pop3_list[0]
#define pop3_account 	pop3_list[1]
#define pop3_password 	pop3_list[2]

string pop3_account_old;

static void index(int);
static void send_index(int);
static void send_server(int);
static void send_guide(int);
static void send_guide_second_page(int&, int&);
static void send_mail();
static void receive_index(int);
static void receive_server(int);
static void receive_guide(int);
static void receive_guide_second_page(int&, int&);
static void receive_mail();
static void show_mails();
static void show_mail_details(int);

static int set_smtpserver(int&);
static int set_smtpcontent();
static int set_pop3server(int&);
static int set_simple_str(string&);

static int replace_all(string& str, const string& _old, const string& _new);
static int send_check();
static int receive_check();
static int mymail_send();
static int mymail_receive(int);
static void save_mail(int, bool show  = true);
static void save_all_mails();

// 通用方法

int getop() {
	system("stty -icanon");
	system("stty -echo");
	int ret = -1;
	char buf[5];
	int len = read(STDIN_FILENO, buf, 5);
	if (len == 1) {
		switch(buf[0]) {
		case 'q':
			ret = EXIT_OPT;
			break;
		case 's':
			ret = SAVE_OPT;
		break;
		case '\n':
			ret = CHECK_OPT;
			break;
		case ESC[0]:
			ret = ESC_OPT;
		default:
			break;
		}
	} else if (len >= 3) {
		if (!memcmp(buf, UP, 3)) {
			ret = UP_OPT;
		} else if (!memcmp(buf, DOWN, 3)) {
			ret = DOWN_OPT;
		} else if (!memcmp(buf, LEFT, 3)) {
			ret = LEFT_OPT;
		} else if (!memcmp(buf, RIGHT, 3)) {
			ret = RIGHT_OPT;
		}
	}
	system("stty icanon");
	system("stty echo");
	return ret;
}

int getop_ex() {
	int ret = -1;
	char buf[10];
	int len = read(STDIN_FILENO, buf, 10);
	if (len == 1) {
		switch(buf[0]) {
		case 'q':
			ret = EXIT_OPT;
			break;
		case 's':
			ret = SAVE_OPT;
			break;
		case '\n':
			ret = CHECK_OPT;
			break;
		case ESC[0]:
			ret = ESC_OPT;
		default:
			break;
		}
	} else if (len >= 3) {
		if (!memcmp(buf, UP, 3)) {
			ret = UP_OPT;
		} else if (!memcmp(buf, DOWN, 3)) {
			ret = DOWN_OPT;
		} else if (!memcmp(buf, LEFT, 3)) {
			ret = LEFT_OPT;
		} else if (!memcmp(buf, RIGHT, 3)) {
			ret = RIGHT_OPT;
		}
	}
	return ret;
}

void sig_int_handler(int sign) {
    printf(NONE);
    printf(SHOWCUR);
	system("stty icanon");
	system("stty echo");
	exit(1);
}

/* Main函数在此 */
int main() {
	signal(SIGINT, sig_int_handler);
    index(0);
	printf(NONE);
    printf(SHOWCUR);
}

// 用户交互方法

void index(int idx) {
	while (1) {
		printf(CLEARTOP);
		printf(HIDECUR);
		printf("欢迎使用邮件收发系统，您可以选择发送邮件服务和接收邮件服务，\n");
		printf("使用方向键可以选择服务，按回车确定，按q退出，请选择：\n");
		OPTION(INDEX, idx, NONE, NONE, L_BLUE)
		operate = getop();
		switch (operate) {
		case EXIT_OPT:
		case ESC_OPT:
			return;
			break;
		case UP_OPT:
			idx = (--idx) < 0 ? 0 : idx;
			break;
		case DOWN_OPT:
			idx = (++idx) >= OPT_INDEX_SIZE ? OPT_INDEX_SIZE - 1 : idx;
			break;
		case CHECK_OPT:
			if (idx == 0) {
				send_index(0);
			} else if (idx == 1) {
				receive_index(0);
			} else {
				return;
			}
			break;
		default:
			break;
		}
	}
}

// 发送邮件

void send_index(int idx) {
	while (1) {
		printf(CLEARTOP);
		printf(HIDECUR);
		printf("欢迎使用发送邮件服务，接下来会有引导指导您发送邮件，\n");
		printf("请按照要求填写信息：\n");
		OPTION(CONFIRM, idx, NONE, NONE, L_BLUE)
		operate = getop();
		switch (operate) {
		case ESC_OPT:
			return;
			break;
		case UP_OPT:
			idx = (--idx) < 0 ? 0 : idx;
			break;
		case DOWN_OPT:
			idx = (++idx) >= OPT_CONFIRM_SIZE ? OPT_CONFIRM_SIZE - 1 : idx;
			break;
		case CHECK_OPT:
			if (idx == 0) {
				send_server(0);
				return;
			} else {
				return;
			}
			break;
		default:
			break;
		}
	}
}

void send_server(int idx) {
	while (1) {
		printf(CLEARTOP);
		printf(HIDECUR);
		printf("发送邮件服务，请先设置内容：\n");
		OPTION(MAILTO, idx, NONE, NONE, L_BLUE)
		operate = getop();
		switch (operate) {
		case EXIT_OPT:
		case ESC_OPT:
			return;
			break;
		case UP_OPT:
			idx = (--idx) < 0 ? 0 : idx;
			break;
		case DOWN_OPT:
			idx = (++idx) >= OPT_MAILTO_SIZE ? OPT_MAILTO_SIZE - 1 : idx;
			break;
		case CHECK_OPT:
			if (idx == 0) {
				send_guide(0);
			} else if (idx == 1) {
				send_mail();
			} else {
				return;
			}
			break;
		default:
			break;
		}
	}
}

void send_guide(int idx) {
	int second_idx = 0;
	operate = NONE_OPT;
	while (1) {
		printf(CLEARTOP);
		printf("发送邮件服务，请填写信息(回车输入信息，按ESC取消)：\n");
		OPTION_EX(MAILSETTING, idx, NONE, GREEN, L_BLUE, send_guide_second_page, second_idx)
		if (!refresh) {
			operate = getop();
		} else {
			refresh = FALSE;
		}
		if (operate == ESC_OPT) {
			return;
		}
	}
}

void send_guide_second_page(int& idx, int& second_idx) {
	int ret = 0;
	switch (idx) {
	case 0:
		ret = set_smtpserver(second_idx);
		if (ret == TRUE) {
			idx++;
			refresh = TRUE;
		}
		break;
	case 1:
		set_simple_str(smtp_account);
		idx++;
		refresh = TRUE;
		break;
	case 2:
		set_simple_str(smtp_password);
		idx++;
		refresh = TRUE;
		break;
	case 3:
		set_simple_str(smtp_target);
		idx++;
		refresh = TRUE;
		break;
	case 4:
		set_simple_str(smtp_title);
		idx++;
		refresh = TRUE;
		break;
	case 5: 
		set_simple_str(smtp_sendMan);
		idx++;
		refresh = TRUE;
		break;
	case 6:
		set_smtpcontent();
		idx++;
		refresh = TRUE;
		break;
	case 7:
		getop();
		refresh = TRUE;
		operate = ESC_OPT;
		break;
	default:
		break;
	}
}

int set_smtpserver(int& idx) {
	int block = FALSE;
	switch (operate) {
	case UP_OPT:
		idx = (--idx) < 0 ? 0 : idx;
		break;
	case DOWN_OPT:
		idx = (++idx) >= OPT_SMTPSERVER_SIZE ? OPT_SMTPSERVER_SIZE - 1 : idx;
		break;
	case CHECK_OPT:
		if (idx == 4) {
			block = TRUE;
		} else {
			smtp_server = smtp_server_list[idx];
			return TRUE;
		}
		break;
	default:
		break;
	}
	OPTION(SMTPSERVER, idx, NONE, NONE, BLUE)
	if (block) {
		printf(SHOWCUR);
		printf("请输入服务器地址(域名或者ip地址)：");
		std::cin >> smtp_server;
		return TRUE;
	}
	return FALSE;
}

int set_smtpcontent() {
	printf(SHOWCUR);
	printf("每行输入自动末尾添加一个换行，空行不会读取\n");
	printf("输入行以\\n开头表示输入换行，直到第一个非\\n的字符都会视作换行\n");
	printf("如果这一行就想输入n个作为字符的\"\\n\"开头，加上\'\\\'给\\转义，\n转义只在\\n或者\\\\n开头的行有效，转义后后面的\\n将视作字符串处理\n");
	printf("输入一行开头为\\n.\\n表示结束，后面数据将忽略\n");
	printf("请开始输入正文：\n\n");
	string tmp;
	while (1) {
		std::cin >> tmp;
		if (tmp.compare(0, 5, "\\n.\\n") == 0) {
			break;
		}
		int i, num = 0;
		for (i = 0; i < tmp.size(); i+=2) {
			if (tmp.compare(i, 2, "\\n") == 0) {
				num++;
			} else {
				break;
			}
		}
		tmp = tmp.substr(i);
		if (num > 0 || tmp.compare(0, 3, "\\\\n") == 0) {
			// 转义
			replace_all(tmp, string("\\\\n"), string("\\n"));
		}
		smtp_content.append(num, '\n');
		smtp_content.append(tmp).append("\n");
	}
	return TRUE;
}

void send_mail() {
	printf(CLEARTOP);
	int i = send_check();
	if (i != 0) {
		printf("缺少必要信息：%s\n任意键返回\n", smtp_list_name[i]);
		getop();
		return;
	}
	printf("请确认信息：\n");
	for (int i = 0; i < 6; i++) {
		std::cout << smtp_list_name[i] << std::endl
			<< smtp_list[i] << std::endl;
	}
	printf("请确认信息，确认发送?(Y/N)：");
	printf(SHOWCUR);
	fflush(stdout);
	string isSend;
	std::cin >> isSend;
	if (isSend[0] == 'Y' || isSend[0] == 'y') {
		printf(CLEARTOP);
		printf("发送中....\n");
		int ret = mymail_send();
		if (ret >= 0) {
			printf("发送完成，共发送%d条!   按任意键返回...\n", ret);
			for (int i = 0; i < 7; i++) {
				smtp_list[i].clear();
			}
		} else {
			printf("发送失败!   按任意键返回...\n");
		}
		getop();
		return;
	} else {
		return;
	}
}

int send_check() {
	for (int i = 0; i < 6; i++) {
		if (smtp_list[i].empty()) {
			return i + 1;
		}
	}
	return 0;
}

// 接收邮件

void receive_index(int idx) {
	while (1) {
		printf(CLEARTOP);
		printf(HIDECUR);
		printf("欢迎使用接收邮件服务，接下来会有引导指导您接收邮件，\n");
		printf("请按照要求填写信息：\n");
		OPTION(CONFIRM, idx, NONE, NONE, L_BLUE)
		operate = getop();
		switch (operate) {
		case ESC_OPT:
			return;
			break;
		case UP_OPT:
			idx = (--idx) < 0 ? 0 : idx;
			break;
		case DOWN_OPT:
			idx = (++idx) >= OPT_CONFIRM_SIZE ? OPT_CONFIRM_SIZE - 1 : idx;
			break;
		case CHECK_OPT:
			if (idx == 0) {
				receive_server(0);
				return;
			} else {
				return;
			}
			break;
		default:
			break;
		}
	}
}

void receive_server(int idx) {
	while (1) {
		printf(CLEARTOP);
		printf(HIDECUR);
		printf("接收邮件服务，请先设置内容：\n");
		OPTION(MAILFROM, idx, NONE, NONE, L_BLUE)
		operate = getop();
		switch (operate) {
		case EXIT_OPT:
		case ESC_OPT:
			return;
			break;
		case UP_OPT:
			idx = (--idx) < 0 ? 0 : idx;
			break;
		case DOWN_OPT:
			idx = (++idx) >= OPT_MAILFROM_SIZE ? OPT_MAILFROM_SIZE - 1 : idx;
			break;
		case CHECK_OPT:
			if (idx == 0) {
				receive_guide(0);
			} else if (idx == 1) {
				receive_mail();
			} else if (idx == 2) {
				show_mails();
			} else {
				return;
			}
			break;
		default:
			break;
		}
	}
}

void receive_guide(int idx) {
	int second_idx = 0;
	operate = NONE_OPT;
	while (1) {
		printf(CLEARTOP);
		printf("接收邮件服务，请填写信息(回车输入信息，按ESC取消)：\n");
		OPTION_EX(ACCOUNTSETTING, idx, NONE, GREEN, L_BLUE, receive_guide_second_page, second_idx)
		if (!refresh) {
			operate = getop();
		} else {
			refresh = FALSE;
		}
		if (operate == ESC_OPT) {
			return;
		}
	}
}

void receive_guide_second_page(int& idx, int& second_idx) {
	int ret = 0;
	switch (idx) {
	case 0:
		ret = set_pop3server(second_idx);
		if (ret == TRUE) {
			idx++;
			refresh = TRUE;
		}
		break;
	case 1:
		pop3_account_old = pop3_account;
		set_simple_str(pop3_account);
		pop3_password.clear();
		idx++;
		refresh = TRUE;
		break;
	case 2:
		set_simple_str(pop3_password);
		if (pop3_account.compare(pop3_account_old) != 0)
			recevie_mail_list.clear();
		idx++;
		refresh = TRUE;
		break;
	case 3:
		getop();
		refresh = TRUE;
		operate = ESC_OPT;
		break;
	default:
		break;
	}
}

int set_pop3server(int& idx) {
	int block = FALSE;
	switch (operate) {
	case UP_OPT:
		idx = (--idx) < 0 ? 0 : idx;
		break;
	case DOWN_OPT:
		idx = (++idx) >= OPT_POPSERVER_SIZE ? OPT_POPSERVER_SIZE - 1 : idx;
		break;
	case CHECK_OPT:
		if (idx == 4) {
			block = TRUE;
		} else {
			pop3_server = pop3_server_list[idx];
			return TRUE;
		}
		break;
	default:
		break;
	}
	OPTION(POPSERVER, idx, NONE, NONE, BLUE)
	if (block) {
		printf(SHOWCUR);
		printf("请输入服务器地址(域名或者ip地址)：");
		std::cin >> smtp_server;
		return TRUE;
	}
	return FALSE;
}

void receive_mail() {
	printf(CLEARTOP);
	printf(SHOWCUR);
	int i = receive_check();
	if (i != 0) {
		printf("缺少必要信息%s\n按任意键返回\n", pop3_list_name[i]);
		getop();
		return;
	}
	string size_str;
	int size;
	printf("该账号已接收%d封邮件，请继续\n", (int) recevie_mail_list.size());
	printf("请输入这次接收邮件数量，输入不合法默认为50：\n");
	std::cin >> size_str;
	size = atoi(size_str.c_str());
	if (size <= 0) {
		size = 50;
	}
	printf("请确认信息：\n");
	for (int i = 0; i < 3; i++) {
		std::cout << pop3_list_name[i] << std::endl
			<< pop3_list[i] << std::endl;
	}
	printf("请确认信息，确认接收?(Y/N)：");
	fflush(stdout);
	string isSend;
	std::cin >> isSend;
	if (isSend[0] == 'Y' || isSend[0] == 'y') {
		printf(CLEARTOP);
		printf("接收中....\n");
		int ret = mymail_receive(size);
		if (ret >= 0) {
			printf("成功接收%d封邮件，共接收%d封邮件！   按任意键返回查看...\n", ret, (int) recevie_mail_list.size());
		} else {
			printf("接收失败!   按任意键返回重试...\n");
		}
		getop();
		return;
	} else {
		return;
	}
}

void show_mails() {
	int idx = 0;
	int list_size = (int) recevie_mail_list.size();
	int page_size = 10;
	int pages = list_size > 0 ? ((list_size - 1) / page_size + 1) : 1;
	int current_page = 0;
	while (1) {
		int current_page_size = current_page + 1 == pages ?
				list_size - page_size * current_page :
				page_size;
		printf(CLEARTOP);
		printf(HIDECUR);
		printf("请选择要查看的邮件，按q或者ESC返回，按s全部保存到本地：\n");
		for (int i = 0; i < current_page_size; i++) {
			if (i == idx) {
				printf(L_BLUE);
			} else {
				printf(NONE);
			}
			int pos = current_page * page_size + i;
			if (pos >= list_size) {
				break;
			}
			std::cout <<
				recevie_mail_list[pos].id << " " <<
				recevie_mail_list[pos].getTitle() << " " <<
				recevie_mail_list[pos].getSendMan() << " " <<
				recevie_mail_list[pos].getDate();
			if (i == idx) {
				printf(" <");
			}
			printf("\n");
			printf(NONE);
		}
		printf("第%d页 共%d页\n", current_page + 1, pages);
		operate = getop();
		switch (operate) {
		case EXIT_OPT:
		case ESC_OPT:
			return;
			break;
		case UP_OPT:
			idx = (--idx) < 0 ? 0 : idx;
			break;
		case DOWN_OPT:
			idx = (++idx) >= current_page_size ? current_page_size - 1 : idx;
			break;
		case LEFT_OPT:
			current_page = (--current_page) < 0 ? 0 : current_page;
			idx = 0;
			break;
		case RIGHT_OPT:
			current_page = (++current_page) >= pages ? pages - 1 : current_page;
			idx = 0;
			break;
		case SAVE_OPT:
			save_all_mails();
			break;
		case CHECK_OPT:
			show_mail_details(idx);
			break;
		default:
			break;
		}
	}
	return;
}

void show_mail_details(int index) {
	using std::cout;
	using std::endl;
	printf(CLEARTOP);
	printf(HIDECUR);
	printf("邮件详细信息，按q或者ESC返回：\n");
	string subType = recevie_mail_list[index].getContentType();
	BOOL needTranslate = subType.empty() || (subType.compare(0, 4, "text") == 0) || (subType.compare(0, 5, "plain") == 0);
	BOOL canTryTranslate = (subType.compare(0, 4, "html") == 0);
	cout << "标题：" << recevie_mail_list[index].getTitle() << endl
		 << "发件人：" << recevie_mail_list[index].getSendMan() << endl
		 << "发件时间：" << recevie_mail_list[index].getDate() << endl
		 << "邮件格式：" << subType << endl
		 << "邮件内容：(不能解析html)" << endl <<
				(needTranslate ?
				(canTryTranslate ?
				tryEncodeHtmldata(recevie_mail_list[index].getContentDecoded()) :
				recevie_mail_list[index].getData() ) :
				recevie_mail_list[index].getContentDecoded());
	
	if (needTranslate && !canTryTranslate) {
		printf("内容为富文本或者多块文本，无法解析，显示原本数据\n");
	}
	printf("按q或者ESC返回，按s保存邮件到本地：\n");
	system("stty -icanon");
	system("stty -echo");
	while (1) {
		int op = getop_ex();
		if (op == EXIT_OPT || op == ESC_OPT) {
			break;
		}
		if (op == SAVE_OPT) {
			save_mail(index);
			getop_ex();
			break;
		}
	}
	system("stty icanon");
	system("stty echo");
}

int receive_check() {
	for (int i = 0; i < 3; i++) {
		if (pop3_list[i].empty()) {
			return i + 1;
		}
	}
	return 0;
}

// 一般方法

int replace_all(string& str, const string& _old, const string& _new) {
	int num = 0;
	string::size_type pos(0);
	while (1) {
		pos = str.find(_old, pos);
		if(pos != string::npos) {
			str.replace(pos, _old.length(), _new);
			pos += _new.length();
			num++;
		} else {
			break;
		}
    }
	return num;
}

int set_simple_str(string& str) {
	printf(SHOWCUR);
	std::cin >> str;
	return TRUE;
}

int mymail_send() {
	int result, ret = 0;
	SmtpConnect *smtp_conn = (SmtpConnect*)ConnectFactory(smtp_server).build(ConnectFactory::SMTP);
	if (smtp_conn == nullptr) {
		//error
		return -1;
	}
	vector<string> targets = split(smtp_target, ";");
	MailContext send_context;
	send_context.setData(smtp_content);
	send_context.setTitle(smtp_title);
	send_context.setSendMan(smtp_sendMan);
	send_context.setContentType("text/plain; charset=UTF-8");
	for (int i = 0; i < targets.size(); i++) {
		result = (*smtp_conn).setAccount(smtp_account)
			.setPassword(smtp_password)
			.setMailTo(targets[i])
			.setContext(send_context)
			.run();
		if (result == TRUE) {
			printf("成功发送一条邮件-->%s\n", targets[i].c_str());
			ret++;
		} else {
			printf("发送邮件失败-->%s\n", targets[i].c_str());
			printf("%s", smtp_conn->getErrorMsg().c_str());
			if (smtp_conn->getErrorno() == MAILCONN_LOGINERR) {
				printf ("账号密码有误\n");
			} else if (smtp_conn->getErrorno() == MAILCONN_COREERR) {
				printf ("对方邮箱账号有误\n");
			}
		}
	}
	delete smtp_conn;
	return ret;
}

int mymail_receive(int size) {
	int result;
	Pop3Connect* pop_conn;
	pop_conn = (Pop3Connect*)ConnectFactory(pop3_server).build(ConnectFactory::POP3);
	if (pop_conn == nullptr) {
		// error
		return -1;
	}
	MailContext *recv_context = new MailContext[size];
	int context_size = size;
	result = (*pop_conn).setAccount(pop3_account)
		.setPassword(pop3_password)
		.setRecvTask(recv_context, &context_size)
		.run();
	if (result == TRUE) {
		// 你的代码，一共获得了context_size个结果，放在context中了
		printf("get %d mails\n", context_size);
		for (int i = 0; i < context_size; i++) {
			recevie_mail_list.push_back(recv_context[i]);
		}
		delete pop_conn;
		delete[] recv_context;
		return context_size;
	} else {
		if (pop_conn->getErrorno() == MAILCONN_LOGINERR) {
			printf ("账号密码有误\n");
		}
	}
	delete pop_conn;
	delete[] recv_context;
	return -1;
}

void save_mail(int index, bool show) {
	if (show) {
		printf("正在保存...\n");
	}
	string dir = "./saved_mails/" + pop3_account.substr(0, pop3_account.find_first_of("@"));
	string com = "mkdir -p " + dir;
	system(com.c_str());
	std::ofstream ofs;
	string file_name = dir + "/" + recevie_mail_list[index].getTitle() + ".txt";
	ofs.open(file_name, std::ios::out | std::ios::trunc);
	ofs << recevie_mail_list[index].getRawStream().getBytes();
	ofs.close();
	if (show) {
		printf("保存成功-->%s\n任意键退出\n", dir.c_str());
	}
}

void save_all_mails() {
	printf(CLEARTOP);
	printf("正在保存...\n");
	for (int i = 0; i < recevie_mail_list.size(); i++) {
		save_mail(i, false);
	}
	printf("保存完成，任意键返回\n");
	getop();
}