/*
 * @Descripttion : 
 * @version      : 
 * @Author       : sannmizu
 * @Date         : 2020-06-06 20:30:22
 * @LastEditors  : sannmizu
 * @LastEditTime : 2020-06-14 12:54:26
 */ 
#pragma once

/* ANSI color */
#define NONE                 "\e[0m"
#define BLACK                "\e[0;30m"
#define L_BLACK              "\e[1;30m"
#define RED                  "\e[0;31m"
#define L_RED                "\e[1;31m"
#define GREEN                "\e[0;32m"
#define L_GREEN              "\e[1;32m"
#define BROWN                "\e[0;33m"
#define YELLOW               "\e[1;33m"
#define BLUE                 "\e[0;34m"
#define L_BLUE               "\e[1;34m"
#define PURPLE               "\e[0;35m"
#define L_PURPLE             "\e[1;35m"
#define CYAN                 "\e[0;36m"
#define L_CYAN               "\e[1;36m"
#define GRAY                 "\e[0;37m"
#define WHITE                "\e[1;37m"

#define BOLD                 "\e[1m"
#define UNDERLINE            "\e[4m"
#define BLINK                "\e[5m"
#define REVERSE              "\e[7m"
#define HIDE                 "\e[8m"
#define CLEAR                "\e[2J"
#define CLRLINE              "\r\e[K" //or "\e[1K\r"
#define CLEARTOP             "\e[2J\e[0;0H" //or "\e[1K\r"

#define HIDECUR              "\e[?25l"
#define SHOWCUR              "\e[?25h"

#define SETCURPOS(x, y) printf("\e[%d;%dH", x, y)

#define OPTION(_page_, idx, normal, over, choose) \
    for (int i = 0; i < OPT_##_page_##_SIZE; i++) { \
        if (i < idx) { \
            printf(over); \
        } else if (i == idx) { \
            printf(choose); \
        } else { \
            printf(normal); \
        } \
        printf("%s", OPT_##_page_##_OPTIONS[i]); \
        if (i == idx) { \
            printf(" <"); \
        } \
        printf("\n"); \
        printf(NONE); \
    }

#define OPTION_EX(_page_, idx, normal, over, choose, second_page, second_idx) \
    { \
        for (int i = 0; i < OPT_##_page_##_SIZE; i++) { \
            if (i < idx) { \
                printf(over); \
            } else if (i == idx) { \
                printf(choose); \
            } \
            printf("%s", OPT_##_page_##_OPTIONS[i]); \
            printf("\n"); \
            printf(NONE); \
            if (i == idx) { \
                break; \
            } \
        } \
        second_page(idx, second_idx); \
    }

#define SET_OPT(_name_, _size_) \
    const int   OPT_##_name_##_SIZE = _size_; \
    const char* OPT_##_name_##_OPTIONS[_size_] = {

#define SET_OPT_END() \
    };

SET_OPT(INDEX, 3)
	"发送邮件服务",
	"接收邮件服务",
    "退出程序"
SET_OPT_END()

SET_OPT(CONFIRM, 2)
	"确定",
	"返回"
SET_OPT_END()

// smtp
SET_OPT(MAILTO, 3)
	"设置内容",
    "发送",
	"返回"
SET_OPT_END()

SET_OPT(MAILSETTING, 8)
	"请选择邮件服务器(暂只支持可以使用非SSL方式连接的邮箱服务器)：",
	"请输入您的邮箱账号：",
    "请输入您的邮箱密码(QQ邮箱需要使用认证码登陆)：",
    "请输入您要发送到的对象的邮箱账号(多个对象用;隔开)：",
    "请输入邮件的标题：",
    "请设置发件人名字：",
    "请输入邮件的内容：",
    "设置完成，按任意键返回"
SET_OPT_END()

SET_OPT(SMTPSERVER, 5)
	"  1:武汉大学(smtp.whu.edu.cn)",
	"  2:网易邮箱(smtp.163.com)",
	"  3:新浪邮箱(smtp.sina.com.cn)",
	"  4:QQ邮箱(smtp.qq.com)",
	"  n:手动输入"
SET_OPT_END()


// pop3
SET_OPT(MAILFROM, 4)
	"设置账号",
    "接收邮件",
    "查看接收的邮件",
	"返回"
SET_OPT_END()

SET_OPT(ACCOUNTSETTING, 4)
	"请选择邮件服务器(暂只支持可以使用非SSL方式连接的邮箱服务器)：",
    "请输入您的邮箱账号：",
	"请输入您的邮箱密码(QQ邮箱需要使用认证码登陆)：",
    "设置完成，按任意键返回"
SET_OPT_END()

SET_OPT(POPSERVER, 5)
	"  1:武汉大学(pop.whu.edu.cn)",
	"  2:网易邮箱(pop.163.com)",
	"  3:新浪邮箱(pop.sina.com.cn)",
	"  4:QQ邮箱(pop.qq.com)",
	"  n:手动输入"
SET_OPT_END()