// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2003-2004 - Tim Kemp and Stefan Kueng

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#pragma once

#include "ReportCtrl/ReportCtrl.h"

/**
 * \ingroup TortoiseProc
 * Implements a CTreeCtrl which browses a subversion repository. The constructor
 * takes the URL of the repository.
 *
 * \par requirements
 * win95 or later
 * winNT4 or later
 * MFC
 *
 * \version 1.0
 * first version
 *
 * \date 02-07-2003
 *
 * \author Tim Kemp
 *
 * \par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 */
class CRepositoryTree : public CReportCtrl
{
	DECLARE_DYNAMIC(CRepositoryTree)

public:
	CRepositoryTree(const CString& strUrl);
	virtual ~CRepositoryTree();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult);

	void Init(LONG revision);
	CString MakeUrl(HTREEITEM hItem);
	CString GetFolderUrl(HTREEITEM hItem);
	BOOL IsFolder(HTREEITEM hItem);
	HTREEITEM ItemExists(HTREEITEM parent, CString item);
	void Refresh(HTREEITEM hItem);
	void RefreshMe(HTREEITEM hItem);

private:
	CString		m_strUrl;
	SVN			m_svn;
	BOOL		bInit;
	LONG		m_nRevision;

public:
	int			m_nIconFolder;
};


