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

#include "stdafx.h"

#pragma warning (disable : 4786)

// Initialize GUIDs (should be done only and at-least once per DLL/EXE)
#define INITGUID
#include <initguid.h>
#include <shlguid.h>
#include "Guids.h"
#include "Globals.h"
#include "ShellExt.h"
#include "..\\version.h"
#include "UnicodeStrings.h"
#include "atltrace.h"
#include "Accctrl.h"
#include "Aclapi.h"
#include "libintl.h"

// *********************** CShellExt *************************
CShellExt::CShellExt(FileState state)
{
    m_State = state;
	
    m_cRef = 0L;
    g_cRefThisDll++;

	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("TortoiseSVNShell"));
	if (hMutex == NULL)
	{
		hMutex = CreateMutex(NULL, NULL,  _T("TortoiseSVNShell"));
		SetSecurityInfo(hMutex,SE_KERNEL_OBJECT,DACL_SECURITY_INFORMATION,0,0,0,0);
		ATLTRACE2(_T("created mutex\n"));
	}
	else
	{
		ATLTRACE2(_T("opened mutex\n"));
	}
	
    INITCOMMONCONTROLSEX used = {
        sizeof(INITCOMMONCONTROLSEX),
			ICC_LISTVIEW_CLASSES | ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES
    };
    InitCommonControlsEx(&used);
	LoadLangDll();
	apr_initialize();
}

CShellExt::~CShellExt()
{
	g_cRefThisDll--;
	CloseHandle(hMutex);
	apr_terminate();
}

void LoadLangDll()
{
	if (g_langid != g_ShellCache.GetLangID())
	{
		g_langid = g_ShellCache.GetLangID();
		DWORD langId = g_langid;
		TCHAR langDll[MAX_PATH*4];
		HINSTANCE hInst = NULL;
		TCHAR langdir[MAX_PATH] = {0};
		char langdirA[MAX_PATH] = {0};
		if (GetModuleFileName(g_hmodThisDll, langdir, MAX_PATH)==0)
			return;
		if (GetModuleFileNameA(g_hmodThisDll, langdirA, MAX_PATH)==0)
			return;
		TCHAR * dirpoint = _tcsrchr(langdir, '\\');
		char * dirpointA = strrchr(langdirA, '\\');
		if (dirpoint)
			*dirpoint = 0;
		if (dirpointA)
			*dirpointA = 0;
		dirpoint = _tcsrchr(langdir, '\\');
		dirpointA = strrchr(langdirA, '\\');
		if (dirpoint)
			*dirpoint = 0;
		if (dirpointA)
			*dirpointA = 0;
		strcat(langdirA, "\\Languages");
		bindtextdomain ("subversion", langdirA);

		do
		{
			_stprintf(langDll, _T("%s\\Languages\\TortoiseProc%d.dll"), langdir, langId);
			BOOL versionmatch = TRUE;

			struct TRANSARRAY
			{
				WORD wLanguageID;
				WORD wCharacterSet;
			};

			DWORD dwReserved,dwBufferSize;
			dwBufferSize = GetFileVersionInfoSize((LPTSTR)langDll,&dwReserved);

			if (dwBufferSize > 0)
			{
				LPVOID pBuffer = (void*) malloc(dwBufferSize);

				if (pBuffer != (void*) NULL)
				{
					UINT        nInfoSize = 0,
						nFixedLength = 0;
					LPSTR       lpVersion = NULL;
					VOID*       lpFixedPointer;
					TRANSARRAY* lpTransArray;
					TCHAR       strLangProduktVersion[MAX_PATH];

					if (GetFileVersionInfo((LPTSTR)langDll,
						dwReserved,
						dwBufferSize,
						pBuffer))
					{
						// Abfragen der aktuellen Sprache
						if (VerQueryValue(	pBuffer,
							_T("\\VarFileInfo\\Translation"),
							&lpFixedPointer,
							&nFixedLength))
						{
							lpTransArray = (TRANSARRAY*) lpFixedPointer;

							_stprintf(strLangProduktVersion, _T("\\StringFileInfo\\%04x%04x\\ProductVersion"),
								lpTransArray[0].wLanguageID, lpTransArray[0].wCharacterSet);

							if (VerQueryValue(pBuffer,
								(LPTSTR)strLangProduktVersion,
								(LPVOID *)&lpVersion,
								&nInfoSize))
							{
								versionmatch = FALSE;
								const TCHAR * p = _tcsrchr((LPCTSTR)lpVersion, ',');
								if (p)
								{
									DWORD num = ((DWORD)p-(DWORD)lpVersion)/sizeof(TCHAR);
									versionmatch = (_tcsncmp((LPCTSTR)lpVersion, _T(STRPRODUCTVER), num) == 0);							
								}
							}

						}
					}
					free(pBuffer);
				} // if (pBuffer != (void*) NULL) 
			} // if (dwBufferSize > 0) 
			else
				versionmatch = FALSE; 

			if (versionmatch)
				hInst = LoadLibrary(langDll);
			if (hInst != NULL)
			{
				if (g_hResInst != g_hmodThisDll)
					FreeLibrary(g_hResInst);
				g_hResInst = hInst;
			} // if (hInst != NULL) 
			else
			{
				DWORD lid = SUBLANGID(langId);
				lid--;
				if (lid > 0)
				{
					langId = MAKELANGID(PRIMARYLANGID(langId), lid);
				}
				else
					langId = 0;
			} 
		} while ((hInst == NULL) && (langId != 0));
		if (hInst == NULL)
		{
			if (g_hResInst != g_hmodThisDll)
				FreeLibrary(g_hResInst);
			g_hResInst = g_hmodThisDll;
			CRegStdWORD lid(_T("Software\\TortoiseSVN\\LanguageID"), GetUserDefaultLangID());
			lid.removeValue();
			g_langid = 1033;
		}
	} // if (g_langid != g_ShellCache.GetLangID()) 
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = NULL;
	
    if (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
        *ppv = (LPSHELLEXTINIT)this;
    }
    else if (IsEqualIID(riid, IID_IContextMenu))
    {
        *ppv = (LPCONTEXTMENU)this;
    }
    else if (IsEqualIID(riid, IID_IContextMenu2))
    {
        *ppv = (LPCONTEXTMENU2)this;
    }
    else if (IsEqualIID(riid, IID_IContextMenu3))
    {
        *ppv = (LPCONTEXTMENU3)this;
    }
    else if (IsEqualIID(riid, IID_IShellIconOverlayIdentifier))
    {
        *ppv = (IShellIconOverlayIdentifier*)this;
    }
    else if (IsEqualIID(riid, IID_IShellPropSheetExt))
    {
        *ppv = (LPSHELLPROPSHEETEXT)this;
    }
#if _WIN32_IE >= 0x0500
	else if (IsEqualIID(riid, IID_IColumnProvider)) 
	{ 
		*ppv = (IColumnProvider *)this;
	} 
#endif
    if (*ppv)
    {
        AddRef();
		
        return NOERROR;
    }
	
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{
    if (--m_cRef)
        return m_cRef;
	
    delete this;
	
    return 0L;
}

// IPersistFile members
STDMETHODIMP CShellExt::GetClassID(CLSID *pclsid) 
{
    *pclsid = CLSID_TortoiseSVN_UNCONTROLLED;
    return S_OK;
}

STDMETHODIMP CShellExt::Load(LPCOLESTR pszFileName, DWORD dwMode)
{
    return S_OK;
}

