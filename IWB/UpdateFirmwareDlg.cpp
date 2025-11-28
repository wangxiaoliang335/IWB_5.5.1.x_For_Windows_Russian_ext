// UpdateFirmwareDlg.cpp : implementation file
//

#include "stdafx.h"
#include <shlwapi.h>
#include "IWB.h"
#include "UpdateFirmwareDlg.h"
static const int E2PROM_SEG_HEADER_LEN = 13;
static const int E2PROM_SEG_FLAG_LEN = 8;
static const int E2PROM_START_POSITION = 8;

static const int E2PROM_TAG_0 = 0x55;
static const int E2PROM_TAG_1 = 0xAA;
static const int E2PROM_TAG_2 = 0x33;
static const int E2PROM_TAG_3 = 0x99;

BOOL IsValidFirmwareFile(const BYTE* pData, int nLength)
{


	if (nLength < E2PROM_START_POSITION + E2PROM_SEG_HEADER_LEN)
		return FALSE;

	if ((pData[E2PROM_START_POSITION + 0] != E2PROM_TAG_0) ||
		(pData[E2PROM_START_POSITION + 1] != E2PROM_TAG_1) ||
		(pData[E2PROM_START_POSITION + 2] != E2PROM_TAG_2) ||
		(pData[E2PROM_START_POSITION + 3] != E2PROM_TAG_3))
		return FALSE;



	return TRUE;

	BYTE firm_ver   = pData[E2PROM_START_POSITION + 5];
	BYTE firm_year  = pData[E2PROM_START_POSITION + 6];
	BYTE firm_month = pData[E2PROM_START_POSITION + 7];
	BYTE firm_day   = pData[E2PROM_START_POSITION + 8];


	//校验码验证失败，校验方法不对?数据格式有误？
	WORD wROMDataLength = (WORD)(pData[E2PROM_START_POSITION + 9 ] << 8) + (WORD)pData[E2PROM_START_POSITION + 10];
	WORD wCheckSum      = (WORD)(pData[E2PROM_START_POSITION + 11] << 8) + (WORD)pData[E2PROM_START_POSITION + 12];

	if (nLength < (E2PROM_SEG_HEADER_LEN + wROMDataLength))
		return FALSE;

	WORD wCount = wROMDataLength  - E2PROM_START_POSITION - E2PROM_SEG_FLAG_LEN;
	WORD wCheckSumCalculated = 0;
	const BYTE* pRomData = pData + E2PROM_START_POSITION + E2PROM_SEG_HEADER_LEN + E2PROM_SEG_FLAG_LEN;
	while (wCount)
	{
		wCheckSumCalculated += *pRomData++;
		wCount--;
	}


	if (wCheckSumCalculated == wCheckSum)
	{
		return TRUE;
	}

	return FALSE;

}

//@功能:根据读取的ROM数据，在指定的目录下搜索匹配的文件
BOOL SearchMatchedFile(const BYTE* pSrcData, int nDataLength, const TCHAR* lpszBaseFolder, TCHAR* lpszFoundFileName, int FoundFileNameBufLength)
{

	WIN32_FIND_DATA fd;
	CString strSearchString;
	strSearchString.Format(
		_T("%s\\*.*"),
		lpszBaseFolder);

	HANDLE hFind = ::FindFirstFile(strSearchString, &fd);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;

	BOOL bFound = FALSE;
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do {
			
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{
				continue;
			}
			else
			{
				CString strFilePath;
				strFilePath.Format(
					_T("%s\\%s"),
					lpszBaseFolder,
					fd.cFileName);



				HANDLE hFile = CreateFile(
					strFilePath,          // file to open
					GENERIC_READ,          // open for reading
					FILE_SHARE_READ,       // share for reading
					NULL,                  // default security
					OPEN_EXISTING,         // existing file only
					FILE_ATTRIBUTE_NORMAL , // normal file
					NULL);                  // no attr. template
				if (hFile == INVALID_HANDLE_VALUE) continue;

				BYTE buf[1024 * 2];

				DWORD dwLength;
				if (ReadFile(
					hFile,
					buf,
					sizeof(buf),
					&dwLength,
					NULL
				))
				{

					if (IsValidFirmwareFile(buf, (int)dwLength))
					{

						if(0 == memcmp(pSrcData, buf, min((size_t)dwLength, (size_t)nDataLength)))
						{
							_tcscpy_s(lpszFoundFileName, FoundFileNameBufLength, fd.cFileName);
							bFound = TRUE;
							
						}
					}


				}

				CloseHandle(hFile);

			}

		} while (!bFound && ::FindNextFile(hFind, &fd));
		
		::FindClose(hFind);
	}

	return bFound;

}

// CUpdateFirmwareDlg dialog

IMPLEMENT_DYNAMIC(CUpdateFirmwareDlg, CDialog)

CUpdateFirmwareDlg::CUpdateFirmwareDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_UPDATE_FIRMWARE, pParent),
	m_uMsgID(0)
{

}

CUpdateFirmwareDlg::~CUpdateFirmwareDlg()
{
}

void CUpdateFirmwareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUpdateFirmwareDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER_FIRMWARE_FILE, &CUpdateFirmwareDlg::OnBnClickedButtonBrowserFirmwareFile)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_FIRMWARE, &CUpdateFirmwareDlg::OnBnClickedButtonWriteFirmware)
	ON_BN_CLICKED(IDC_BUTTON_READ_FIRMWARE, &CUpdateFirmwareDlg::OnBnClickedButtonReadFirmware)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_RESULT, &CUpdateFirmwareDlg::OnBnClickedButtonClearResult)
END_MESSAGE_MAP()


// CUpdateFirmwareDlg message handlers


void CUpdateFirmwareDlg::OnBnClickedButtonBrowserFirmwareFile()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDlg(
		TRUE, 
		_T(".bin"), 
		NULL, 
		OFN_READONLY,
		_T("firmware file(*.bin)|*.*|"), 
		this);

	fileDlg.m_ofn.lpstrInitialDir = theApp.m_strFirmwareDirectory;

	if (fileDlg.DoModal())
	{
		GetDlgItem(IDC_EDIT_FIRMWARE_FILE_PATH)->SetWindowText(fileDlg.GetPathName());

	}
}


void CUpdateFirmwareDlg::OnBnClickedButtonWriteFirmware()
{
	// TODO: Add your control notification handler code here
	//停止播放
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_MENU_STOP, 0L);

	CString strMsg;

	if (m_oUVCToEEPROM.GetInstanceCount() == 0)
	{//未发现设备
		strMsg.Format(_T("%s"), g_oResStr[IDS_STRING466]);//No UVC device exists!
		AppendResultMsg(strMsg);
		return;
	}

	CString strFirmwareFilePath;
	GetDlgItem(IDC_EDIT_FIRMWARE_FILE_PATH)->GetWindowText(strFirmwareFilePath);

	if (!PathFileExists(strFirmwareFilePath))
	{
		strMsg.Format(g_oResStr[IDS_STRING467], (LPCTSTR)strFirmwareFilePath);//"Firmware file %s not found!"
		AppendResultMsg(strMsg);
		return;
	}

	FILE* fh;
	errno_t err = fopen_s(&fh, CT2CA(strFirmwareFilePath), "rb");
	if (err != 0)
	{
		strMsg.Format(g_oResStr[IDS_STRING468], errno);//Error: opend firmware file failed with error 0x%x!
		AppendResultMsg(strMsg);
		return;
	}

	BYTE buf[1024 * 2];
	size_t fileLength = fread(buf, 1, sizeof(buf), fh);

	{
		CWaitCursor waitCursor;
		if (m_oUVCToEEPROM.Write(0, buf, fileLength, 0))
		{
			//Write firmware \"%s\" succeeded. \r\n Please unplug then plug in the device again.
			strMsg.Format(g_oResStr[IDS_STRING469], PathFindFileName(strFirmwareFilePath));//
			AppendResultMsg(strMsg);
		}
		else
		{
			strMsg.Format(g_oResStr[IDS_STRING470]);//_T("Write firmware failed.")
			AppendResultMsg(strMsg);
		}
	}

	fclose(fh);
}


void CUpdateFirmwareDlg::OnBnClickedButtonReadFirmware()
{
	// TODO: Add your control notification handler code here
	//停止播放
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_MENU_STOP, 0L);

	CString strMsg;

	if (m_oUVCToEEPROM.GetInstanceCount() == 0)
	{//未发现设备
		strMsg.Format(_T("%s"), g_oResStr[IDS_STRING466]);//_T("No device exists!")
		AppendResultMsg(strMsg);
		return;
	}
	

	BYTE buf[1024 * 4];


	int nOffset = 0;
	int deviceInst = 0;
	{
		CWaitCursor waitCursor;
		if (m_oUVCToEEPROM.Read(nOffset, buf, sizeof(buf), deviceInst))
		{
			//strMsg.Format(_T("Read firmware succeeded."));
			strMsg.Format(g_oResStr[IDS_STRING471]);
			AppendResultMsg(strMsg);

			//将读取的内容与Firmware文件夹的文件逐一比较，确定同哪个固件文件的内容一致
			TCHAR szMatchedFileName[MAX_PATH];
			if (SearchMatchedFile(buf, _countof(buf), theApp.m_strFirmwareDirectory, szMatchedFileName, _countof(szMatchedFileName)))
			{
				//strMsg.Format(_T("The firmware content is consistent with file \"%s\"."), szMatchedFileName);
				strMsg.Format(g_oResStr[IDS_STRING472], szMatchedFileName);
				AppendResultMsg(strMsg);
			}
			else
			{
				//strMsg.Format(_T("No file found to match the firmware."));
				strMsg.Format(g_oResStr[IDS_STRING473]);
				AppendResultMsg(strMsg);

				//save to a tmp file

				CString strFileName;
				SYSTEMTIME localTime;
				GetLocalTime(&localTime);
				strFileName.Format(
					_T("%s\\firmware-read-%04d%02d%02d-%02d%02d%02d-%03d.bin"),
					(LPCTSTR)theApp.m_strFirmwareDirectory,
					localTime.wYear,
					localTime.wMonth,
					localTime.wDay,
					localTime.wHour,
					localTime.wMinute,
					localTime.wSecond,
					localTime.wMilliseconds);

				//
				if (!PathFileExists(theApp.m_strFirmwareDirectory))
				{
					CreateFullDirectory(theApp.m_strFirmwareDirectory);
				}

				FILE* fileSave;
				if (0 == fopen_s(&fileSave, CT2CA(strFileName.GetString()), "wb"))
				{
					WORD wDataLength = (WORD)(buf[E2PROM_START_POSITION + 9] << 8) + (WORD)buf[E2PROM_START_POSITION + 10];
					wDataLength += E2PROM_SEG_HEADER_LEN;
					
					wDataLength = min(wDataLength, sizeof(buf));

					wDataLength = sizeof(buf);

					fwrite(buf, 1, wDataLength, fileSave);
					fclose(fileSave);

					strMsg.Format(g_oResStr[IDS_STRING475], strFileName);
					AppendResultMsg(strMsg);
				}

				

			}
			
		}
		else
		{
			//strMsg.Format(_T("Error:read firmware failed!"));
			strMsg.Format(g_oResStr[IDS_STRING474]);
			AppendResultMsg(strMsg);
		}
	}


}


void CUpdateFirmwareDlg::AppendResultMsg(LPCTSTR lpszMsg)
{
	CEdit* pEdit = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_RESULT));
	if (NULL == pEdit) return;
	
	//先通过选择全部，将光标移到末尾
	pEdit->SetSel(0, -1);
	//再选择末尾
	pEdit->SetSel(-1, -1);
	CString strText;
	strText.Format(_T("[%04d]%s\r\n"), m_uMsgID, lpszMsg);
	m_uMsgID++;
	pEdit->ReplaceSel(strText);
}


void CUpdateFirmwareDlg::OnBnClickedButtonClearResult()
{
	// TODO: Add your control notification handler code here
	CEdit* pEdit = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_RESULT));
	if (NULL == pEdit) return;

	//先通过选择全部，将光标移到末尾
	pEdit->SetSel(0, -1);

	pEdit->ReplaceSel(_T(""));

	m_uMsgID = 0;

}
