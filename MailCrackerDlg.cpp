
// MailCrackerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MailCracker.h"
#include "MailCrackerDlg.h"
#include "afxdialogex.h"
#include "base64.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMailCrackerDlg 对话框



CMailCrackerDlg::CMailCrackerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAILCRACKER_DIALOG, pParent)
	, m_strServer(_T("")) //smtp.163.com smtp.qq.com
	, m_strUsername(_T("")) 
	, m_strPassword(_T("")) 
	, m_strRciever(_T(""))
	, m_strSubject(_T(""))
	, m_strContent(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CMailCrackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITSERVER, m_strServer);
	DDX_Text(pDX, IDC_EDITUSERNAME, m_strUsername);
	DDX_Text(pDX, IDC_EDITPASSWD, m_strPassword);
	DDX_Text(pDX, IDC_EDITRECIVER, m_strRciever);
	DDX_Text(pDX, IDC_EDITTHEME, m_strSubject);
	DDX_Text(pDX, IDC_EDIT, m_strContent);
}

BEGIN_MESSAGE_MAP(CMailCrackerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOGIN, &CMailCrackerDlg::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_SEND, &CMailCrackerDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_CLEAR, &CMailCrackerDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_EXIT, &CMailCrackerDlg::OnBnClickedExit)
END_MESSAGE_MAP()


// CMailCrackerDlg 消息处理程序

BOOL CMailCrackerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	GetDlgItem(IDC_SEND)->EnableWindow(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMailCrackerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMailCrackerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMailCrackerDlg::AddListBoxText(char szText[])
{
	((CListBox*)GetDlgItem(IDC_LIST))->AddString(szText);
}

//登录
void CMailCrackerDlg::OnBnClickedLogin()
{
	GetDlgItemText(IDC_EDITSERVER, m_strServer);
	GetDlgItemText(IDC_EDITPASSWD, m_strPassword);
	GetDlgItemText(IDC_EDITUSERNAME, m_strUsername);
	//判断输入框是否为空
	if (m_strServer.IsEmpty()) {
		MessageBox("请输入服务器地址");
		return;
	}
	if (m_strUsername.IsEmpty()) {
		MessageBox("请输入用户名");
		return;
	}
	if (m_strPassword.IsEmpty()) {
		MessageBox("请输入密码");
		return;
	}

	//创建客户端套接字
	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_socket == INVALID_SOCKET) {
		AddListBoxText("初始化网络失败");
		return;
	}

	//连接SMTP服务器 先将域名转化为IP地址
	hostent* host = gethostbyname(m_strServer);
	in_addr in_addr_string;
	//拷贝4个字节 获取IP地址
	memcpy(&in_addr_string, host->h_addr_list[0], 4);

	//连接服务器
	sockaddr_in ServerAddr;
	ServerAddr.sin_addr = in_addr_string;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(25);

	//TCP连接

	if (SOCKET_ERROR == connect(m_socket, (sockaddr*)&ServerAddr, sizeof(sockaddr)))
	{
		AddListBoxText("连接服务器失败");
		return;
	}

	AddListBoxText("连接服务器成功\n");

	//接收服务器反馈信息
	char szRecv[128];

	ZeroMemory(szRecv, 128);
	if (SOCKET_ERROR == recv(m_socket, szRecv, sizeof(szRecv), 0)) {
		AddListBoxText("接收服务器数据异常\n");
		return;
	}

	CString strSend = "HELO smtp\r\n";
	if (SOCKET_ERROR == send(m_socket, strSend, strSend.GetLength(), 0)) {
		AddListBoxText("发送数据失败\n");
		return;
	}

	ZeroMemory(szRecv, 128);
	if (SOCKET_ERROR == recv(m_socket, szRecv, sizeof(szRecv), 0)) {
		AddListBoxText("接收服务器数据异常\n");
		return;
	}

	strSend = "auth login\r\n";
	if (SOCKET_ERROR == send(m_socket, strSend, strSend.GetLength(), 0)) {
		AddListBoxText("发送数据失败\n");
		return;
	}

	ZeroMemory(szRecv, 128);
	if (SOCKET_ERROR == recv(m_socket, szRecv, sizeof(szRecv), 0)) {
		AddListBoxText("接收服务器数据异常\n");
		return;
	}

	//发送加密后用户名 使用base64加密
	char base64_username[2028];
	ZeroMemory(base64_username, 2028);
	CString strUsername;
	//对字符串进行base64加密
	encode_to_base64(base64_username, (char*)(LPCSTR)m_strUsername, m_strUsername.GetLength());
	strUsername.Format("%s\r\n", base64_username);
	
	if (SOCKET_ERROR == send(m_socket, strUsername, strUsername.GetLength(), 0)) {
		AddListBoxText("发送数据失败");
		return;
	}

	ZeroMemory(szRecv, 128);
	if (SOCKET_ERROR == recv(m_socket, szRecv, sizeof(szRecv), 0)) {
		AddListBoxText("接收数据异常\n");
		return;
	}

	//发送加密后密码
	char base64_password[2028];
	ZeroMemory(base64_password, 2028);
	CString strPassword;
	encode_to_base64(base64_password, (char*)(LPCSTR)m_strPassword, m_strPassword.GetLength());
	strPassword.Format("%s\r\n", base64_password);

	if (SOCKET_ERROR == send(m_socket, strPassword, strPassword.GetLength(), 0)) {
		AddListBoxText("发送数据失败");
		return;
	}

	ZeroMemory(szRecv, 128);
	if (SOCKET_ERROR == recv(m_socket, szRecv, sizeof(szRecv), 0)) {
		MessageBox("接收服务器数据失败");
		return;
	}

	if (!strcmp(szRecv, "235 Authentication successful\r\n")) {
		MessageBox("登录成功");
		AddListBoxText("身份验证成功\n");
		//解锁发送按钮
		GetDlgItem(IDC_SEND)->EnableWindow(TRUE);
	}
	else {
		MessageBox("登录失败");
		AddListBoxText("身份验证失败\n");
	}
}

//发送按钮
void CMailCrackerDlg::OnBnClickedSend()
{
	UpdateData(TRUE);

	//判断是否为空
	if (m_strRciever.IsEmpty() || m_strSubject.IsEmpty() || m_strContent.IsEmpty())
	{
		MessageBox("请输入邮件内容或收件人信息");
		return;
	}

	CString strSend;
	strSend = "MAIL FROM:<" + m_strUsername + ">\r\n";
	if (SOCKET_ERROR == send(m_socket, strSend, strSend.GetLength(), 0)) {
		AddListBoxText("发送数据失败");
		return;
	}
	
	char szRecv[128];
	ZeroMemory(szRecv, 128);
	if (SOCKET_ERROR == recv(m_socket, szRecv, sizeof(szRecv), 0)) {
		AddListBoxText("接收服务器数据失败");
		return;
	}

	

	strSend = "RCPT TO:<" + m_strRciever + ">\r\n";
	if (SOCKET_ERROR == send(m_socket, strSend, strSend.GetLength(), 0)) {
		AddListBoxText("发送数据失败");
		return;
	}

	ZeroMemory(szRecv, 128);
	if (SOCKET_ERROR == recv(m_socket, szRecv, sizeof(szRecv), 0)) {
		AddListBoxText("接收服务器数据失败");
		return;
	}

	//MessageBox(szRecv);

	strSend = "DATA\r\n";
	if (SOCKET_ERROR == send(m_socket, strSend, strSend.GetLength(), 0)) {
		AddListBoxText("发送数据失败");
		return;
	}

	ZeroMemory(szRecv, 128);
	if (SOCKET_ERROR == recv(m_socket, szRecv, sizeof(szRecv), 0)) {
		AddListBoxText("接收服务器数据失败");
		return;
	}

	//发送真正的数据
	strSend = "";
	strSend = strSend + "From:" + m_strUsername + "\r\n";
	strSend = strSend + "Subject:" + m_strSubject + "\r\n\r\n";
	strSend = strSend + m_strContent + "\r\n\r\n";
	strSend = strSend + "\r\n.\r\n";

	if (SOCKET_ERROR == send(m_socket, strSend, strSend.GetLength(), 0)) {
		AddListBoxText("发送数据失败");
		return;
	}

	ZeroMemory(szRecv, 128);
	if (SOCKET_ERROR == recv(m_socket, szRecv, sizeof(szRecv), 0)) {
		AddListBoxText("接收服务器数据失败");
		return;
	}

	AddListBoxText("邮件发送成功\n");
}


//清空编辑框
void CMailCrackerDlg::OnBnClickedClear()
{
	GetDlgItem(IDC_EDIT)->SetWindowTextA("");
}

//退出时关闭连接
void CMailCrackerDlg::OnBnClickedExit()
{
	CString strSend;
	char szRecv[128];

	strSend = "QUIT\r\n";
	if (SOCKET_ERROR == send(m_socket, strSend, strSend.GetLength(), 0)) {
		AddListBoxText("发送数据失败");
		return;
	}

	ZeroMemory(szRecv, 128);
	if (SOCKET_ERROR == recv(m_socket, szRecv, sizeof(szRecv), 0)) {
		AddListBoxText("接收服务器数据失败");
		return;
	}

	if (!strncmp(szRecv, "221", 3)) {
		AddListBoxText("关闭连接成功\n");
	}
}
