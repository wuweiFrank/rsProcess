/************************************************************************
* 发送邮件模块
*可以发送文本
*Date:2011-12-01
************************************************************************/
#include "CSendMail.h"

#pragma comment(lib,"WSOCK32")
#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)

CSendMail::CSendMail(void)
{

}

CSendMail::~CSendMail(void)
{
}

void CSendMail::Char2Base64(unsigned char* dest, unsigned char* src, int srclen)
{
	int sign = 0;

	const unsigned char EncodeIndex[] = {
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
		'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
		'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
		'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
	};

	for (int i = 0; i != srclen; i++, src++, dest++)
	{
		switch (sign)
		{
		case 0:
			*(dest) = EncodeIndex[*src >> 2];
			break;
		case 1:
			*dest = EncodeIndex[((*(src - 1) & 0x03) << 4) | (((*src) & 0xF0) >> 4)];
			break;
		case 2:
			*dest = EncodeIndex[((*(src - 1) & 0x0F) << 2) | ((*(src)& 0xC0) >> 6)];
			*(++dest) = EncodeIndex[(*(src)& 0x3F)];
			break;
		}
		(sign == 2) ? (sign = 0) : (sign++);
	}
	switch (sign)
	{
	case 0:
		break;
	case 1:
		*(dest++) = EncodeIndex[((*(src - 1) & 0x03) << 4) | (((*src) & 0xF0) >> 4)];
		*(dest++) = '=';
		*(dest++) = '=';
		break;
	case 2:
		*(dest++) = EncodeIndex[((*(src - 1) & 0x0F) << 2) | ((*(src)& 0xC0) >> 6)];
		*(dest++) = '=';
		break;
	default:
		break;
	}
}

bool CSendMail::CReateSocket(SOCKET &sock)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return false;
	}
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return false;
	}
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sock == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(25);//发邮件一般都是25端口
	if (m_sMailInfo.m_pcIPName == "")
	{
		servaddr.sin_addr.s_addr = inet_addr(m_sMailInfo.m_pcIPAddr);//直接使用IP地址
	}
	else
	{
		struct hostent *hp = gethostbyname(m_sMailInfo.m_pcIPName);//使用名称
		if (hp == NULL)
		{
			DWORD dwErrCode = GetLastError();
			return false;
		}
		servaddr.sin_addr.s_addr = *(int*)(*hp->h_addr_list);
	}


	int ret = connect(sock, (sockaddr*)&servaddr, sizeof(servaddr));//建立连接
	if (ret == SOCKET_ERROR)
	{
		DWORD dwErr = GetLastError();
		return false;
	}

	return true;
}
bool CSendMail::Logon(SOCKET &sock)
{
	char local_host[MAX_PATH];

	if (gethostname(local_host, MAX_PATH) != 0)
	{
		return false;
	}
	recv(sock, m_cReceiveBuff, 1024, 0);
	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "HELO %s\r\n", local_host);


	send(sock, m_cSendBuff, strlen(m_cSendBuff), 0);//开始会话
	recv(sock, m_cReceiveBuff, 1024, 0);
	if (m_cReceiveBuff[0] != '2' || m_cReceiveBuff[1] != '5' || m_cReceiveBuff[2] != '0')
	{
		return false;
	}

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "AUTH LOGIN\r\n");
	send(sock, m_cSendBuff, strlen(m_cSendBuff), 0);//请求登录
	recv(sock, m_cReceiveBuff, 1024, 0);
	if (m_cReceiveBuff[0] != '3' || m_cReceiveBuff[1] != '3' || m_cReceiveBuff[2] != '4')
	{
		return false;
	}

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	Char2Base64((unsigned char*)m_cSendBuff, (unsigned char*)m_sMailInfo.m_pcUserName, strlen(m_sMailInfo.m_pcUserName));
	m_cSendBuff[strlen(m_cSendBuff)] = '\r';
	m_cSendBuff[strlen(m_cSendBuff)] = '\n';
	send(sock, m_cSendBuff, strlen(m_cSendBuff), 0);//发送用户名
	recv(sock, m_cReceiveBuff, 1024, 0);
	if (m_cReceiveBuff[0] != '3' || m_cReceiveBuff[1] != '3' || m_cReceiveBuff[2] != '4')
	{
		return false;
	}

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	Char2Base64((unsigned char*)m_cSendBuff, (unsigned char*)m_sMailInfo.m_pcUserPassWord, strlen(m_sMailInfo.m_pcUserPassWord));
	m_cSendBuff[strlen(m_cSendBuff)] = '\r';
	m_cSendBuff[strlen(m_cSendBuff)] = '\n';
	send(sock, m_cSendBuff, strlen(m_cSendBuff), 0);//发送用户密码
	recv(sock, m_cReceiveBuff, 1024, 0);
	if (m_cReceiveBuff[0] != '2' || m_cReceiveBuff[1] != '3' || m_cReceiveBuff[2] != '5')
	{
		return false;
	}
	return true;//登录成功
}

bool CSendMail::SendHead(SOCKET &sock)
{
	int rt;
	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "MAIL FROM:<%s>\r\n", m_sMailInfo.m_pcSender);
	rt = send(sock, m_cSendBuff, strlen(m_cSendBuff), 0);

	if (rt != strlen(m_cSendBuff))
	{
		return false;
	}
	recv(sock, m_cReceiveBuff, 1024, 0);

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "RCPT TO:<%s>\r\n", m_sMailInfo.m_pcReceiver);
	rt = send(sock, m_cSendBuff, strlen(m_cSendBuff), 0);
	if (rt != strlen(m_cSendBuff))
	{
		return false;
	}
	recv(sock, m_cReceiveBuff, 1024, 0);

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	memcpy(m_cSendBuff, "DATA\r\n", strlen("DATA\r\n"));
	rt = send(sock, m_cSendBuff, strlen(m_cSendBuff), 0);
	if (rt != strlen(m_cSendBuff))
	{
		return false;
	}
	recv(sock, m_cReceiveBuff, 1024, 0);

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "From:\"%s\"<%s>\r\n", m_sMailInfo.m_pcSenderName, m_sMailInfo.m_pcSender);
	sprintf_s(&m_cSendBuff[strlen(m_cSendBuff)], 150, "To:\"INVT.COM.CN\"<%s>\r\n", m_sMailInfo.m_pcReceiver);
	sprintf_s(&m_cSendBuff[strlen(m_cSendBuff)], 150, "Subject:%s\r\nMime-Version: 1.0\r\nContent-Type: multipart/mixed;   boundary=\"INVT\"\r\n\r\n", m_sMailInfo.m_pcTitle);
	rt = send(sock, m_cSendBuff, strlen(m_cSendBuff), 0);
	if (rt != strlen(m_cSendBuff))
	{
		return false;
	}

	return true;
}
bool CSendMail::SendTextBody(SOCKET &sock)
{
	int rt;
	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "--INVT\r\nContent-Type: text/plain;\r\n  charset=\"gb2312\"\r\n\r\n%s\r\n\r\n", m_sMailInfo.m_pcBody);
	rt = send(sock, m_cSendBuff, strlen(m_cSendBuff), 0);
	if (rt != strlen(m_cSendBuff))
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool CSendMail::SendEnd(SOCKET &sock)
{
	sprintf_s(m_cSendBuff, "--INVT--\r\n.\r\n");
	send(sock, m_cSendBuff, strlen(m_cSendBuff), 0);

	sprintf_s(m_cSendBuff, "QUIT\r\n");
	send(sock, m_cSendBuff, strlen(m_cSendBuff), 0);
	closesocket(sock);
	WSACleanup();
	return true;
}
bool CSendMail::SendMail(SMailInfo &smailInfo)
{
	memcpy(&m_sMailInfo, &smailInfo, sizeof(smailInfo));
	if (m_sMailInfo.m_pcBody == NULL
		|| m_sMailInfo.m_pcIPAddr == NULL
		|| m_sMailInfo.m_pcIPName == NULL
		|| m_sMailInfo.m_pcReceiver == NULL
		|| m_sMailInfo.m_pcSender == NULL
		|| m_sMailInfo.m_pcSenderName == NULL
		|| m_sMailInfo.m_pcTitle == NULL
		|| m_sMailInfo.m_pcUserName == NULL
		|| m_sMailInfo.m_pcUserPassWord == NULL)
	{
		return false;
	}
	SOCKET sock;
	if (!CReateSocket(sock))//建立连接
	{
		return false;
	}

	if (!Logon(sock))//登录邮箱
	{
		return false;
	}

	if (!SendHead(sock))//发送邮件头
	{
		return false;
	}

	if (!SendTextBody(sock))//发送邮件文本部分
	{
		return false;
	}

	if (!SendEnd(sock))//结束邮件，并关闭sock
	{
		return false;
	}

	return true;
}
