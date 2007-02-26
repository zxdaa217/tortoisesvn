// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2003-2006 - Stefan Kueng

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "stdafx.h"
#include "TortoiseProc.h"
#include "RepoCreateDlg.h"

IMPLEMENT_DYNAMIC(CRepoCreateDlg, CStandAloneDialog)
CRepoCreateDlg::CRepoCreateDlg(CWnd* pParent /*=NULL*/)
	: CStandAloneDialog(CRepoCreateDlg::IDD, pParent)
{
}

CRepoCreateDlg::~CRepoCreateDlg()
{
}

void CRepoCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CStandAloneDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRepoCreateDlg, CStandAloneDialog)
	ON_BN_CLICKED(IDHELP, OnBnClickedHelp)
END_MESSAGE_MAP()


BOOL CRepoCreateDlg::OnInitDialog()
{
	CStandAloneDialog::OnInitDialog();

	CheckRadioButton(IDC_RADIOBDB, IDC_RADIOFSFS, IDC_RADIOFSFS);

	return TRUE;
}

void CRepoCreateDlg::OnOK()
{
	if (!UpdateData(TRUE))
		return; // don't dismiss dialog (error message already shown by MFC framework)

	if (GetCheckedRadioButton(IDC_RADIOBDB, IDC_RADIOFSFS) == IDC_RADIOBDB)
	{
		RepoType = _T("bdb");
	}
	else
	{
		RepoType = _T("fsfs");
	}

	CStandAloneDialog::OnOK();
}

void CRepoCreateDlg::OnBnClickedHelp()
{
	OnHelp();
}
