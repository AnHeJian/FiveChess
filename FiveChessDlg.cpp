// FiveChessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FiveChess.h"
#include "FiveChessDlg.h"
#include "SetMenu.h"
#include "MainMenu.h"
#include <mmsystem.h>
#include "Board.h"
#include "Rule.h"
#include "ConnectData.h"
#pragma comment(lib,"winmm.lib")   

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFiveChessDlg dialog

CFiveChessDlg::CFiveChessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFiveChessDlg::IDD, pParent)
	, m_TimeCount(0)
	, whoTurn(0)
	, m_history(_T(""))
{
	//{{AFX_DATA_INIT(CFiveChessDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CFiveChessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFiveChessDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT_SHOW, m_show);
	DDX_Control(pDX, IDC_EDIT_TIME, m_Time);
	DDX_Text(pDX, IDC_HISTORY_EDIT, m_history);
}

BEGIN_MESSAGE_MAP(CFiveChessDlg, CDialog)
	//{{AFX_MSG_MAP(CFiveChessDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_UPDATE_COMMAND_UI(ID_NEW_GAME_MENU, OnUpdateNewGameMenu)
	ON_UPDATE_COMMAND_UI(ID_EXIT_GAME_MENU, OnUpdateExitGameMenu)
	ON_UPDATE_COMMAND_UI(ID_DRAW_GAME_MENU, OnUpdateDrawGameMenu)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_SET_MENU, &CFiveChessDlg::OnSetMenu)
	ON_COMMAND(ID_BACK_MENU, &CFiveChessDlg::OnBackMenu)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CFiveChessDlg::OnBnClickedButtonSend)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFiveChessDlg message handlers

BOOL CFiveChessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_main_menu.LoadMenu(IDR_MAIN_MENU);

	SetMenu(&m_main_menu);

	m_main_menu.EnableMenuItem(ID_DRAW_GAME_MENU, MF_GRAYED | MF_DISABLED);
	m_main_menu.EnableMenuItem(ID_BACK_MENU, MF_GRAYED);

	m_bIsConnect = FALSE;

    CRect rect(0, 0, 200, 200);
    m_board.CreateEx( WS_EX_CLIENTEDGE, _T("ChessBoard"), NULL, WS_VISIBLE | WS_BORDER | WS_CHILD,
        CRect(0, 0, 825, 470), this, IDC_BOARD );
	//�������
	m_board.Clear( TRUE );
	
	GetDlgItem( IDC_BOARD )->SetFocus();

	//���ű�������
	if (BackmusicFlg)
	{
		if (backint == 1)
			sndPlaySound((LPCTSTR)IDR_WAVE_CSL, SND_RESOURCE | SND_LOOP | SND_ASYNC | SND_NOSTOP);
		if (backint == 2)
			sndPlaySound((LPCTSTR)IDR_WAVE_YDGQQ, SND_RESOURCE | SND_LOOP | SND_ASYNC | SND_NOSTOP);
		if (backint == 3)
			sndPlaySound((LPCTSTR)IDR_WAVE_MTHFQM, SND_RESOURCE | SND_LOOP | SND_ASYNC | SND_NOSTOP);
	}

	//���ñ༭������
	CEdit* pEdtSend = (CEdit*)GetDlgItem(IDC_EDIT_SEND);
	CEdit* pEdtShow = (CEdit*)GetDlgItem(IDC_EDIT_SHOW);
	font1.CreatePointFont(120, "����");
	pEdtSend->SetFont(&font1);
	pEdtShow->SetFont(&font1);

	CEdit* pEdtTime = (CEdit*)GetDlgItem(IDC_EDIT_TIME);
	font2.CreatePointFont(130, "����");
	pEdtTime->SetFont(&font2);

	CEdit* pEdtHistory = (CEdit*)GetDlgItem(IDC_HISTORY_EDIT);
	font3.CreatePointFont(120, "����");
	pEdtHistory->SetFont(&font3);

	//�˻�ģʽ�����ص���ʱ�ؼ�������ؼ�
	if (isCompter)
	{
		GetDlgItem(IDC_EDIT_TIME)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_SHOW)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_SEND)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SEND)->ShowWindow(FALSE);
	}

	return TRUE; 
}

void CFiveChessDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void CFiveChessDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();	
	}
}

void CFiveChessDlg::Restart()
{
	m_conncet.Close();
	m_sock.Close();
}

HCURSOR CFiveChessDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//�˵�������Ϸ��
void CFiveChessDlg::OnUpdateNewGameMenu(CCmdUI* pCmdUI) 
{
	//�˻�ģʽ
	if (isCompter)
	{
		//������̣����õȴ���־ΪFALSE
		m_board.Clear(FALSE);
		return;
	}


	//����ģʽ
	if (m_bIsConnect)
	{
		CFiveChessDlg * pDlg = (CFiveChessDlg*)AfxGetMainWnd();
		// ���õȴ���־
		m_board.SetWait(TRUE);
		MSGSTRUCT msg;
		msg.msgType = MSG_NEW_GAME;
		pDlg->m_sock.Send((LPCVOID)&msg, sizeof(MSGSTRUCT));
	}
	else
	{
		if (IDOK == m_setup_dlg.DoModal())
		{
			Restart();
			NewGameStart(m_setup_dlg.m_isHost);
			m_isHost = m_setup_dlg.m_isHost;
			strName = m_setup_dlg.m_setname;
		}
	}
}

//�˵����˳���Ϸ��
void CFiveChessDlg::OnUpdateExitGameMenu(CCmdUI* pCmdUI) 
{
	//������Ϸ����,�ر�����
	int err = 0;
	m_conncet.OnClose(err);
	m_sock.OnClose(err);
	MainMenu* mainMenu = new MainMenu;
	AfxGetApp()->m_pMainWnd = mainMenu;

	HWND hWnd = ::FindWindow(NULL, _T("FiveChess"));
	CFiveChessDlg * temp = (CFiveChessDlg*)CFiveChessDlg::FromHandle(hWnd);
	temp->DestroyWindow();

	mainMenu->DoModal();
}

//�˵������塱ѡ��
void CFiveChessDlg::OnUpdateDrawGameMenu(CCmdUI* pCmdUI) 
{
	if(m_bIsConnect)
	{
		m_board.DrawGame();
	}
}

//�˵������á�ѡ��
void CFiveChessDlg::OnSetMenu()
{
	setMenu m_set;
	m_set.DoModal();
}

//�˵������塱ѡ��
void CFiveChessDlg::OnBackMenu()
{
	// TODO: �ڴ���������������
	//�˻�ģʽ�������
	if (isCompter)
	{
		//ɾ���������壬�ػ�����
		m_board.SetData(m_board.a, m_board.b, -1);
		m_board.SetData(m_board.c, m_board.d, -1);
		m_board.Invalidate(FALSE);

		m_board.a = -1;
		m_board.b = -1;
		m_board.c = -1;
		m_board.d = -1;

		m_board.SetWait(FALSE);
		return;
	}


	if (m_bIsConnect)
	{
		//m_board.BackGame();
		CFiveChessDlg * pDlg = (CFiveChessDlg*)AfxGetMainWnd();
		// ���õȴ���־
		m_board.SetWait(TRUE);
		MSGSTRUCT msg;
		msg.msgType = MSG_BACK;
		pDlg->m_sock.Send((LPCVOID)&msg, sizeof(MSGSTRUCT));
	}
}

//������Ϣ���Ͱ�ť
void CFiveChessDlg::OnBnClickedButtonSend()
{
	CString strWord;
	CString strSend;
	GetDlgItemText(IDC_EDIT_SEND, strWord);
	strSend = (strName + _T(" ��") + strWord);
	if (strWord == _T(""))
		MessageBox(_T("������Ϣ����Ϊ�գ�"), _T("��ʾ"), MB_ICONINFORMATION);
	else if (!m_bIsConnect)
		UpdateShow(_T("δ���ӣ�"));
	else
	{
		newshowedit();
		UpdateShow(strSend);
		SendWord(strSend);//����
	}
	SetDlgItemText(IDC_EDIT_SEND, _T(""));//��շ��ͱ༭��
}

HBRUSH CFiveChessDlg::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		if (backint == 1)
			pDC->SetTextColor(RGB(25, 25, 112));
		if (backint == 2)
			pDC->SetTextColor(RGB(0, 0, 0));
		if (backint == 3)
			pDC->SetTextColor(RGB(255, 255, 255));

		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	else if (pWnd->GetDlgCtrlID() == IDC_EDIT_TIME&&nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)(GetStockObject(HOLLOW_BRUSH));
	}
	else if (pWnd->GetDlgCtrlID() == IDC_HISTORY_EDIT&&nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)(GetStockObject(HOLLOW_BRUSH));
	}
}

void CFiveChessDlg::UpdateShow(CString str)
{
	str += _T("\r\n");
	m_show.ReplaceSel(str);
}

void CFiveChessDlg::SendWord(CString str)
{
	MSGSTRUCT msg;
	char word[128] = { 0 };

	strncpy(word, (LPCTSTR)str, sizeof(word));
	for (int i = 0; word[i] != NULL; i++)
	{
		msg.byMsg[i] = word[i];
	}
	msg.msgType = MSG_EXTERN;
	Send(&msg);
}

//��������
void CFiveChessDlg::NewGameStart(BOOL isHost)
{
	
	if (isHost)
	{//��ǰѡ���������
		m_conncet.Create(m_setup_dlg.m_net_port);	//�����˿ڶ���
		m_conncet.Listen();							//����
	}
	else
	{//��ǰѡ����ǿͻ���
		m_sock.Create();							//�����˿ڶ���
													//��������
		m_sock.Connect(m_setup_dlg.m_strHostIP, m_setup_dlg.m_net_port);
	}
}

void CFiveChessDlg::Accept()
{
	//��������
	m_conncet.Accept(m_sock);
	//�������ӳɹ���־
	m_bIsConnect = TRUE;
	//���õ�ǰ������ɫ
	m_board.SetColor(BLACK);
	m_show.ReplaceSel(_T(""));
	m_board.Clear(FALSE);
	//������ʾ�Ի���
	MessageBox( _T("���ӳɹ������Կ�ʼ��Ϸ��"), _T("������"), MB_ICONINFORMATION);
	UpdateShow(_T("������Ϸ�ɹ�,\r\n���Կ�ʼ����!"));
	CString temp = "������!";
	GetDlgItem(IDC_EDIT_TIME)->SetWindowText(temp);
}

void CFiveChessDlg::Connect()
{
	//�������ӳɹ���־
	m_bIsConnect = TRUE;
	//���õ�ǰ������ɫ
	m_board.SetColor(WHITE);
	m_board.Clear(TRUE);
	m_show.ReplaceSel(_T(""));

	//������ʾ�Ի���
	MessageBox( _T("���ӳɹ������Կ�ʼ��Ϸ��"), _T("������"), MB_ICONINFORMATION);
	UpdateShow(_T("������Ϸ�ɹ�,\r\n���Կ�ʼ����!"));
    CString temp = "��ȴ�!";
	GetDlgItem(IDC_EDIT_TIME)->SetWindowText(temp);

}

void CFiveChessDlg::Send(MSGSTRUCT * pmsg)
{
	m_sock.Send((LPVOID)pmsg, sizeof(MSGSTRUCT));
	CFiveChessDlg * pDlg = (CFiveChessDlg*)AfxGetMainWnd();
	pDlg->SetMenuState(NULL, 1);
}

//���ò˵�����ѡ��ɼ���
//m_bWait���û���ѡ��ɼ��ԣ�����˵����0-���ɼ���1-�ɼ���2-������
void CFiveChessDlg::SetMenuState(BOOL bEnable, int m_bWait)
{
	UINT uEnable, uDisable;
	if (m_bWait == 2)
	{
		if (bEnable)
		{
			uEnable = MF_ENABLED;
			uDisable = MF_GRAYED;//| MF_DISABLED
		}
		else
		{
			uEnable = MF_GRAYED;//| MF_DISABLED
			uDisable = MF_ENABLED;
		}

		m_main_menu.EnableMenuItem(ID_NEW_GAME_MENU, uEnable);
		m_main_menu.EnableMenuItem(ID_DRAW_GAME_MENU, uDisable);
	}
	else
	{
		if (m_bWait == 1)
		{
			m_bWait = MF_ENABLED;
		}
		else
		{
			m_bWait = MF_GRAYED | MF_DISABLED;
		}
		m_main_menu.EnableMenuItem(ID_BACK_MENU, m_bWait);
	}
}

//����ʱ�ص�����
void CFiveChessDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_TimeCount--;
	if (m_TimeCount < 0)
	{
		m_TimeCount = 51;

		switch (whoTurn)
		{
		case 1:
			whoTurn = 0;
			AfxMessageBox("���峬ʱ������Ϊ��������Ӯ��");
			if (m_isHost)
				newhistory(_WIN);
			else
				newhistory(_LOST);

			SetMenuState(TRUE, 2);
			SetMenuState(TRUE, 0);
			m_board.SetWait(TRUE);
			return;
		case 2:
			whoTurn = 0;
			AfxMessageBox("���峬ʱ������Ϊ��������Ӯ��");
			if (m_isHost)
				newhistory(_LOST);
			else
				newhistory(_WIN);

			SetMenuState(TRUE, 2);
			SetMenuState(TRUE, 0);
			m_board.SetWait(TRUE);
			return;
		}
	}

	CString temp;
	switch (whoTurn)
	{
	case 1:
		if(m_isHost)
			temp.Format("��ȴ���\r\n����ʣ�ࣺ%d S", m_TimeCount);
		if(!m_isHost)
			temp.Format("�����ӣ�\r\n����ʣ�ࣺ%d S", m_TimeCount);;
		break;
	case 2:
		if (!m_isHost)
			temp.Format("��ȴ���\r\n����ʣ�ࣺ%d S", m_TimeCount);
		if (m_isHost)
			temp.Format("�����ӣ�\r\n����ʣ�ࣺ%d S", m_TimeCount);
		break;
	case 3:
		if (m_isHost)
			temp="������:";
		if (!m_isHost)
			temp = "��ȴ�:";
		break;
	default:
		temp.Format("��Ϸ����\r\n�뿪ʼ����Ϸ��");
		break;
	}
	CRect rect;
	GetDlgItem(IDC_EDIT_TIME)->GetWindowRect(rect);
	ScreenToClient(rect);
	InvalidateRect(rect);
	m_Time.SetSel(0, -1);
	m_Time.ReplaceSel(temp);
	//RefreshControl(IDC_EDIT_TIME);
	//GetDlgItem(IDC_STATIC)->SetWindowText(temp);
	//GetDlgItem(IDC_STATIC)->GetParent()->RedrawWindow();
	CDialog::OnTimer(nIDEvent);
} 

void CFiveChessDlg::initSetTimer()
{
	m_TimeCount = 51;
	SetTimer(1, 1000, NULL);
	whoTurn = (whoTurn == 1) ? 2 : 1;
}

void CFiveChessDlg::Invali()
{
	CBoard* board = (CBoard*)AfxGetMainWnd();
	board->Invalidate();
}
void CFiveChessDlg::OnClose()
{
	exit(0);
	CDialog::OnClose();
}
void CFiveChessDlg::showhistory()
{
	m_history = "";
	pszFileName = "D://myFile.txt";
	if (myFile.Open(pszFileName, CFile::shareDenyWrite, &fileException))

	{
		CString temp = "";
		myFile.ReadString(temp);
		m_history += "�ҷ�:\r\n";
		m_history += "Ӯ:" + temp;
		myFile.ReadString(temp);
		m_history += " ��:" + temp;
		myFile.ReadString(temp);
		m_history += " ƽ��:" + temp + "\r\n";
		GetDlgItem(IDC_HISTORY_EDIT)->SetWindowText(m_history);
		myFile.Close();
	}

	else

	{
		m_history = "Ӯ:0";
		m_history += " ��:0";
		m_history += " ƽ��:0\r\n";
		GetDlgItem(IDC_HISTORY_EDIT)->SetWindowText(m_history);
	}

}
void CFiveChessDlg::showhistory(int win, int lost, int draw)
{

	showhistory();
	CString temp = "";
	temp.Format("%i", win);
	m_history += "�Է���\r\n";
	m_history += "Ӯ:" + temp;
	temp.Format("%i", lost);
	m_history += " ��:" + temp;
	temp.Format("%i", draw);
	m_history += " ƽ��:" + temp + "\r\n";
	GetDlgItem(IDC_HISTORY_EDIT)->SetWindowText(m_history);
}
void CFiveChessDlg::newhistory(int a)
{
	pszFileName = "D://myFile.txt";
	if (!myFile.Open(pszFileName, CFile::modeReadWrite, &fileException))

	{
		myFile.Open(pszFileName, CFile::modeCreate | CFile::modeReadWrite, &fileException);
		if (a == _WIN)
		{
			myFile.WriteString("1\n");
			myFile.WriteString("0\n");
			myFile.WriteString("0\n");
		}
		if (a == _LOST)
		{
			myFile.WriteString("0\n");
			myFile.WriteString("1\n");
			myFile.WriteString("0\n");
		}
		if (a == MSG_AGREE_DRAW)
		{
			myFile.WriteString("0\n");
			myFile.WriteString("0\n");
			myFile.WriteString("1\n");
		}
	}

	else

	{
		myFile.ReadString(m_win);
		myFile.ReadString(m_lost);
		myFile.ReadString(m_draw);
		myFile.SetLength(0);
		if (a == _WIN)
		{
			int win = atoi(m_win);
			m_win.Format("%i", win + 1);
			myFile.WriteString(m_win);
			myFile.WriteString("\n");
			myFile.WriteString(m_lost);
			myFile.WriteString("\n");
			myFile.WriteString(m_draw);
			myFile.WriteString("\n");
		}
		if (a == MSG_AGREE_DRAW)
		{
			int draw = atoi(m_draw);
			m_draw.Format("%i", draw + 1);
			myFile.WriteString(m_win);
			myFile.WriteString("\n");
			myFile.WriteString(m_lost);
			myFile.WriteString("\n");
			myFile.WriteString(m_draw);
			myFile.WriteString("\n");
		}
		if (a == _LOST)
		{
			int lost = atoi(m_lost);
			m_lost.Format("%i", lost + 1);
			myFile.WriteString(m_win);
			myFile.WriteString("\n");
			myFile.WriteString(m_lost);
			myFile.WriteString("\n");
			myFile.WriteString(m_draw);
			myFile.WriteString("\n");
		}
	}
	myFile.Close();
}

void CFiveChessDlg::showwindow(BOOL is)
{
	GetDlgItem(IDC_HISTORY_EDIT)->ShowWindow(is);
}

void CFiveChessDlg::RefreshControl(UINT uCtlID)
{
	CRect rc;
	GetDlgItem(uCtlID)->GetWindowRect(&rc);

	ScreenToClient(&rc);
	InvalidateRect(rc);
}

void CFiveChessDlg::newshowedit()
{
	CRect rect;
	GetDlgItem(IDC_EDIT_SHOW)->GetWindowRect(rect);
	ScreenToClient(rect);
	InvalidateRect(rect);
}