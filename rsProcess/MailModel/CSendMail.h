#pragma once  
#include <winsock2.h>
#include<stdio.h>
using namespace std;
struct SMailInfo //邮件信息  
{
	char*   m_pcUserName;//用户登录邮箱的名称  
	char*   m_pcUserPassWord;//用户登录邮箱的密码  
	char*   m_pcSenderName;//用户发送时显示的名称  
	char*   m_pcSender;//发送者的邮箱地址  
	char*   m_pcReceiver;//接收者的邮箱地址  
	char*   m_pcTitle;//邮箱标题  
	char*   m_pcBody;//邮件文本正文  
	char*   m_pcIPAddr;//服务器的IP  
	char*   m_pcIPName;//服务器的名称（IP与名称二选一，优先取名称）  
	SMailInfo() { memset(this, 0, sizeof(SMailInfo)); }
};
class CSendMail
{
public:
	CSendMail(void);
	~CSendMail(void);

public:
	bool SendMail(SMailInfo &smailInfo);//发送邮件，需要在发送的时候初始化邮件信息  
protected:
	void Char2Base64(unsigned char* pBuff64, unsigned char* pSrcBuff, int iLen);//把char类型转换成Base64类型  
	bool  CReateSocket(SOCKET &sock);//建立socket连接  
	bool Logon(SOCKET &sock);//登录邮箱，主要进行发邮件前的准备工作  

	bool SendHead(SOCKET &sock);//发送邮件头  
	bool SendTextBody(SOCKET &sock);//发送邮件文本正文  
	bool SendEnd(SOCKET &sock);//发送邮件结尾  
protected:
	SMailInfo m_sMailInfo;
	char m_cReceiveBuff[1024];
	char  m_cSendBuff[4096];//发送缓冲区  
};
