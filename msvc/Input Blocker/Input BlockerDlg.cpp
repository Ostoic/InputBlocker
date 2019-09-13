
// Input BlockerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Input Blocker.h"
#include "Input BlockerDlg.h"
#include "afxdialogex.h"

#include "DeviceManager.h"
#include "KeyboardHook.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::vector<DeviceInfoPair> deviceList;

// CInputBlockerDlg dialog

CComboBox *comboBlockedDevices = NULL;
CListBox *listBlockedDevices   = NULL;

CButton  *btnAddDevice		   = NULL;
CButton  *btnRemoveDevice	   = NULL;
CButton  *btnBlockDevices	   = NULL;

CInputBlockerDlg::CInputBlockerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInputBlockerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInputBlockerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInputBlockerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_DROPDOWN(IDC_COMBO_DEVICES, &CInputBlockerDlg::OnCbnDropdownComboDevices)
	ON_BN_CLICKED(IDC_BUTTON_ADD_DEVICE, &CInputBlockerDlg::OnBnClickedButtonAddDevice)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_DEVICE, &CInputBlockerDlg::OnBnClickedButtonRemoveDevice)
	ON_BN_CLICKED(IDC_BUTTON_BLOCK_DEVICES, &CInputBlockerDlg::OnBnClickedButtonBlockDevices)
END_MESSAGE_MAP()


// CInputBlockerDlg message handlers

BOOL CInputBlockerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	btnAddDevice	 = reinterpret_cast<CButton *>(GetDlgItem(IDC_BUTTON_ADD_DEVICE));
	btnRemoveDevice  = reinterpret_cast<CButton *>(GetDlgItem(IDC_BUTTON_REMOVE_DEVICE));
	btnBlockDevices  = reinterpret_cast<CButton *>(GetDlgItem(IDC_BUTTON_BLOCK_DEVICES));

	comboBlockedDevices  = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_DEVICES));
	listBlockedDevices   = reinterpret_cast<CListBox *>(GetDlgItem(IDC_LIST_BLOCKED_DEVICES));

	// Start hook thread.
	CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)KeyboardHook, NULL, NULL, NULL));

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInputBlockerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CInputBlockerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CInputBlockerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CInputBlockerDlg::OnCbnDropdownComboDevices()
{
	puts("Devices ComboBox drop down");

	comboBlockedDevices->ResetContent();
	deviceManager.PopulateList(comboBlockedDevices);
}

void CInputBlockerDlg::OnBnClickedButtonAddDevice()
{
	DeviceInfoPair device;

	WCHAR buffer[MAX_PATH];

	int sel = 0;
	int deviceIndex = 0;

	puts("Add device clicked");

	// Current combo box selection.
	sel = comboBlockedDevices->GetCurSel();

	// If selection is valid, handle it.
	if (sel >= 0)
	{	
		// Copy combo box selection text into list box.
		comboBlockedDevices->GetLBText(sel, buffer);
		listBlockedDevices->InsertString(0, buffer);

		device.name = buffer;

		// Get device index.
		deviceIndex = deviceManager.GetDeviceByName(device.name);

		if (deviceIndex >= 0)
		{
			// Get device hardware id.
			device.hwId = deviceManager.deviceList[deviceIndex].hardwareId;
			deviceList.push_back(device);
		}
	}
}

int GetDevicePairHwId(const WCHAR *name)
{		
	for (int x = 0; x < deviceList.size(); x++)
	{
		if (!StrCmpW(wcsstr(name, deviceList[x].name.c_str()), deviceList[x].name.c_str()))
		{
			return x;
		}
	}

	return -1;
}

void CInputBlockerDlg::OnBnClickedButtonRemoveDevice()
{
	int sel = 0;
	int devicePairIndex = 0;

	WCHAR buffer[MAX_PATH];

	puts("Remove device clicked");

	listBlockedDevices->GetCurSel();
	if (sel >= 0)
	{
		listBlockedDevices->GetText(sel, buffer);
		listBlockedDevices->DeleteString(sel);

		// Get index of given device.
		devicePairIndex = GetDevicePairHwId(buffer);

		if (devicePairIndex >= 0)
		{
			// Remove the device from our vector.
			deviceList.erase(deviceList.begin() + devicePairIndex);
			if (deviceList.size() == 0)
			{
				if (deviceManager.blockMode)
					this->OnBnClickedButtonBlockDevices();
			}

		}
	}
}

void CInputBlockerDlg::OnBnClickedButtonBlockDevices()
{
	puts("Block devices button pressed");

	if (deviceManager.blockMode)
	{
		deviceManager.blockMode = false;
		btnBlockDevices->SetWindowTextW(L"Block Devices");
	}
	else
	{
		deviceManager.blockMode = true;
		btnBlockDevices->SetWindowTextW(L"Unblock Devices");
	}
}
