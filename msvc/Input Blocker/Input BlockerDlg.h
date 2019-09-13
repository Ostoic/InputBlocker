#pragma once
#include "resource.h"
#include <string>
#include <vector>


// CInputBlockerDlg dialog
class CInputBlockerDlg : public CDialogEx
{
// Construction
public:
	CInputBlockerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_INPUTBLOCKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboDevices();
	afx_msg void OnCbnDropdownComboDevices();
	afx_msg void OnBnClickedButtonAddDevice();
	afx_msg void OnBnClickedButtonRemoveDevice();
	afx_msg void OnLbnSelchangeListBlockedDevices();
	afx_msg void OnBnClickedButtonBlockDevices();
};

struct DeviceInfoPair
{
	std::wstring name;
	std::wstring hwId;
};

extern std::vector<DeviceInfoPair> deviceList;
