// OnlineRegisterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IWB.h"
#include "OnlineRegisterDlg.h"

#include "../bitanswer/BitAnswerCpp.h"
#include "../bitanswer/bitanswerError.h"
extern CBitAnswer  g_bitanswer;
// COnlineRegisterDlg dialog


IMPLEMENT_DYNAMIC(COnlineRegisterDlg, CDialog)

inline const TCHAR*  GetPalmTouchTypeString(EPalmTouchControlType ePalmTouchControlType)
{
	switch (ePalmTouchControlType)
	{
	    case E_PLAM_TOUCHCONTROL_P0:
		     return _T("P0");
		     break;
	    case E_PLAM_TOUCHCONTROL_P1:
		     return  _T("P1");
		     break;
	    case E_PLAM_TOUCHCONTROL_P2:
		     return _T("P2");
		     break;
	    case E_PLAM_TOUCHCONTROL_P3:
		     return  _T("P3");
		     break;
	    case E_PLAM_TOUCHCONTROL_P4:
		     return  _T("P4");
		     break;
	    case E_PLAM_TOUCHCONTROL_P5:
		     return  _T("P5");
		     break;
	    case E_PLAM_TOUCHCONTROL_T0:
		     return  _T("T0");
		     break;
	    case E_PLAM_TOUCHCONTROL_T1:
		     return _T("T1");
		     break;
	    case E_PLAM_TOUCHCONTROL_T2:
		     return  _T("T2");
		     break;
	    case E_PLAM_TOUCHCONTROL_T3:
		     return  _T("T3");
		     break;
	    case E_PLAM_TOUCHCONTROL_T4:
		     return _T("T4");
		     break;
	    case E_PLAM_TOUCHCONTROL_T5:
		     return _T("T5");
		     break;
		case E_PLAM_TOUCHCONTROL_TX1:
			return _T("TX1");
			break;
		case E_PLAM_TOUCHCONTROL_TX2:
			return _T("TX2");
			break;
	    default:
		     break;
	}
	return _T("");
}
inline const TCHAR* GetFingerTouchTypeString(EFingerTouchControlType eFingerTouchControlType)
{
	switch (eFingerTouchControlType)
	{
	   case E_FINGER_TOUCHCONTROL_F0:
		    return  _T("F0");
		    break;
	   case E_FINGER_TOUCHCONTROL_F1:
		    return  _T("F1");
		    break;
	   case E_FINGER_TOUCHCONTROL_F2:
		    return  _T("F2");
		    break;
	   case E_FINGER_TOUCHCONTROL_F3:
		    return  _T("F3");
		    break;
	   case E_FINGER_TOUCHCONTROL_F4:
		    return  _T("F4");
		    break;
	   case E_FINGER_TOUCHCONTROL_F5:
		     return  _T("F5");
		     break;
	  default:
		   break;
	}
	return _T("");
}


COnlineRegisterDlg::COnlineRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COnlineRegisterDlg::IDD, pParent),
    m_bRegisteredOk(FALSE)
    , m_strSN(_T(""))
    ,m_eTouchType(E_DEVICE_PEN_TOUCH_WHITEBOARD)
    ,m_eScreenMode(EScreenModeSingle)

{

}

COnlineRegisterDlg::~COnlineRegisterDlg()
{
}

void COnlineRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_SN_NUMBER, m_strSN);
    DDX_Text(pDX, IDC_STATIC_ONLINE_REGISTER_INFO, m_strText);
}


BEGIN_MESSAGE_MAP(COnlineRegisterDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ONLINE_REGISTER, &COnlineRegisterDlg::OnBnClickedButtonOnlineRegister)
END_MESSAGE_MAP()


// COnlineRegisterDlg message handlers

BOOL COnlineRegisterDlg::OnInitDialog()
{
   if(theApp.IsHardwareKeyExist())
   {
       GetDlgItem(IDC_EDIT_SN_NUMBER)->EnableWindow(FALSE);
       GetDlgItem(IDC_BUTTON_ONLINE_REGISTER)->EnableWindow(FALSE);

       CString strFusionInfo;
       //"%d Screens Fusion"
       strFusionInfo.Format(g_oResStr[IDS_STRING464], theApp.GetScreenModeFromUSBKeyCount());

	   //加密狗的类型显示
	   CString strUSBKey = g_oResStr[IDS_STRING460];
	   CString stPalmTouch = _T("---");
	   switch (theApp.GetUSBKeyTouchType())
	   {
	   case E_DEVICE_PEN_TOUCH_WHITEBOARD:
		   strUSBKey = g_oResStr[IDS_STRING460];
		   break;
	   case E_DEVICE_FINGER_TOUCH_WHITEBOARD:
		   strUSBKey = g_oResStr[IDS_STRING461];
		   break;
	   case E_DEVICE_FINGER_TOUCH_CONTROL:
		   strUSBKey = g_oResStr[IDS_STRING495];
		   stPalmTouch = GetFingerTouchTypeString(theApp.GetFingerTouchType());
		   break;
	   case E_DEVICE_PALM_TOUCH_CONTROL:
		    strUSBKey = g_oResStr[IDS_STRING496];
		    stPalmTouch = GetPalmTouchTypeString(theApp.GetPalmTouchType());
			break;
	     default:
		    break;
	   }

	   std::vector<TAutoCalibrateCompensateData> allCompensateData;
	   theApp.GetAllCompensateData(allCompensateData);
	   CString strFirmwareLensInfo;
	   if (allCompensateData.size() == 0)
	   {
		   strFirmwareLensInfo.Format(_T("%s:none"), g_oResStr[IDS_STRING498]);
	   }
	   else
	   {
		   strFirmwareLensInfo.Format(_T("%s("), g_oResStr[IDS_STRING498]);
		   for (uint32_t i = 0; i < allCompensateData.size(); i++)
		   {
			   CString strThrowRatio;

			   if (i == allCompensateData.size() - 1)
			   {
				   strThrowRatio.Format(_T("%.2f"), allCompensateData[i].throwRatioOfLens);
			   }
			   else
			   {
				   strThrowRatio.Format(_T("%.2f,"), allCompensateData[i].throwRatioOfLens);
			   }

			   strFirmwareLensInfo.Append(strThrowRatio);
		   }//for
		   
		   strFirmwareLensInfo.Append(_T(")"));
	   }

       m_strText.Format(
           _T("%s\r\n%s\r\n%s:%s(%s)\r\n%s:%s\r\n\r\n"), 
		   (LPCTSTR)strFirmwareLensInfo,
           g_oResStr[IDS_STRING458],//"使用硬件加密狗"信息
           g_oResStr[IDS_STRING459],
		   strUSBKey,
		   stPalmTouch,
           g_oResStr[IDS_STRING462],
           theApp.GetScreenModeFromUSBKey() >= EScreenModeDouble? (LPCTSTR)strFusionInfo :g_oResStr[IDS_STRING463]);

	   //int nCount = theApp.GetAllUSBKeyTouchTypeCount();

       const  std::unordered_map<std::string, USBKeyInformation>& allUsbkeyInformations = theApp.GetAllUSBKeyInformations();
	   //説明插入了多餘1個的加密狗
	   //if(nCount >1)
       if(allUsbkeyInformations.size() > 1)
	   {
	        // const USBKeyInformation *eAllUSBKeyTouchType = theApp.GetAllUSBKeyInformations();
           
	         m_strText.Append(g_oResStr[IDS_STRING497]);
			 int nUsbKeyCount = 0;

	         //for (int i = 0; i < nCount; i++)
             for(auto& it = allUsbkeyInformations.begin(); it != allUsbkeyInformations.end(); it++)
	         {
				 //if (eAllUSBKeyTouchType[i].eUSBKeyTouchType == E_DEVICE_NOFIND) continue;
                 if (it->second.eUSBKeyTouchType == E_DEVICE_NOT_FOUND) continue;

				 nUsbKeyCount++;
		          CString   strEachUSBKey = _T("");
		          CString   strEachKey = _T("");
		          CString   strEachPalmTouch = _T("--");
		          CString   strFusionInfo = _T("");
		          //switch(eAllUSBKeyTouchType[i].eUSBKeyTouchType)
                  switch(it->second.eUSBKeyTouchType)
		          {
		             case E_DEVICE_PEN_TOUCH_WHITEBOARD:
				          strEachUSBKey = g_oResStr[IDS_STRING460];
			              break;
		             case E_DEVICE_FINGER_TOUCH_WHITEBOARD:
				          strEachUSBKey = g_oResStr[IDS_STRING461];
			              break;
		             case E_DEVICE_FINGER_TOUCH_CONTROL:
				          strEachUSBKey = g_oResStr[IDS_STRING495];
						  strEachPalmTouch = GetFingerTouchTypeString(theApp.GetFingerTouchType());
			              break;
		             case E_DEVICE_PALM_TOUCH_CONTROL:
				          strEachUSBKey = g_oResStr[IDS_STRING496];
				          //strEachPalmTouch = GetPalmTouchTypeString(eAllUSBKeyTouchType[i].ePalmTouchControlType);
                          strEachPalmTouch = GetPalmTouchTypeString(it->second.ePalmTouchControlType);
				          break;
			         default:
				          break;
		           }
		           CString  strFusion =_T("");
		           //strFusion.Format(g_oResStr[IDS_STRING464], (int)eAllUSBKeyTouchType[i].eScreenModeFromUsbKey+1);
                   strFusion.Format(g_oResStr[IDS_STRING464], (int)it->second.eScreenModeFromUsbKey + 1);
		           strEachKey.Format(_T("\r\n(%d:)%s(%s),%s"), nUsbKeyCount, strEachUSBKey, strEachPalmTouch,
			       //eAllUSBKeyTouchType[i].eScreenModeFromUsbKey >= EScreenModeDouble ? (LPCTSTR)strFusion : g_oResStr[IDS_STRING463]);
                   it->second.eScreenModeFromUsbKey >= EScreenModeDouble ? (LPCTSTR)strFusion : g_oResStr[IDS_STRING463]);
	               m_strText.Append(strEachKey);
	         }
	   }       
   }
   else
   {
        // TODO:  Add extra initialization here
        BIT_STATUS status = g_bitanswer.Login("", BIT_MODE_AUTO);
        if(status == BIT_SUCCESS)
        {  //登录成功后，提取SN显示在对话框中
           
            m_strText = g_oResStr[IDS_STRING455];//"注册成功"信息

            string sessionInfo;
            if (g_bitanswer.GetSessionInfo(XML_TYPE_SN_INFO, sessionInfo) == BIT_SUCCESS)
            {
                CComPtr<IXMLDOMDocument> xmlDoc;  // xml 文档
                CComPtr<IXMLDOMNode> rootNode;    // BitConfig节点

                HRESULT hr = S_OK;

                CoInitialize(NULL); // 初始化COM, 解析XML需要使用
                hr = xmlDoc.CoCreateInstance(CLSID_DOMDocument);
                if (hr == S_OK)
                {
                    VARIANT_BOOL vb;
                    hr = xmlDoc->loadXML(CComBSTR(sessionInfo.c_str()), &vb); //加载XML串
                    if (hr == S_OK)
                    {
                        CComPtr<IXMLDOMElement> rootElement;
                        hr = xmlDoc->get_documentElement(&rootElement); //根节点
                        if (hr == S_OK)
                        {
                            hr = rootElement->selectSingleNode(OLESTR("snInfo/sn"), &rootNode);
                            if(hr == S_OK)
                            {
                                BSTR sn;
                                hr = rootNode->get_text(&sn);
                                if (hr == S_OK)
                                {
                                    m_strSN = CW2T(sn);
                                    SysFreeString(sn);
                                }
                            }
                        }
                        rootElement.Release();
                    }
                }//if(hr == ok)

                CoUninitialize();

            }//if(bitanswer.GetSessionInfo)

            //Read Features
            BIT_UINT32 value;
            BIT_STATUS status = g_bitanswer.ReadFeature(FEATURE_TOUCH_TYPE,&value);
            if(status == BIT_SUCCESS)
            {
                m_eTouchType = (value == 0)? E_DEVICE_PEN_TOUCH_WHITEBOARD : E_DEVICE_FINGER_TOUCH_WHITEBOARD;
            }
            
            status = g_bitanswer.ReadFeature(FEATURE_SCREEN_TYPE,&value);
            if(status == BIT_SUCCESS)
            {
                //m_eScreenType = (value == 0)?ESingleScreenMode:EDoubleScreenMode;
                if (EScreenModeSingle <= (EScreenMode)value && (EScreenMode)value < EScreenModeHexa)
                {
                    m_eScreenMode = (EScreenMode)value;
                }
                else
                {
                    m_eScreenMode = EScreenModeSingle;
                }

            }

           m_strText.Format(
           _T("%s\r\n%s:%s\r\n%s:%s"), 
           g_oResStr[IDS_STRING455],//注册成功
           g_oResStr[IDS_STRING459],
           m_eTouchType == E_DEVICE_PEN_TOUCH_WHITEBOARD ? g_oResStr[IDS_STRING460] : g_oResStr[IDS_STRING461],
           g_oResStr[IDS_STRING462],
           m_eScreenMode >= EScreenModeDouble ? g_oResStr[IDS_STRING464] : g_oResStr[IDS_STRING463]);

        }//if(status == BIT_SUCCESS)
        else
        {
            m_strText = g_oResStr[IDS_STRING456];//"未注册"信息
        }
   }
    CDialog::OnInitDialog();

	//设置对话框的缺省按钮,调用函数SetDefaultButton竟然不管用。
	//发现在对话框编辑器中, 将缺省按钮的tab stop编号设为1，可以
	//起到作用。
	//SetDefaultButton(IDC_BUTTON_ONLINE_REGISTER);
	
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void COnlineRegisterDlg::OnBnClickedButtonOnlineRegister()
{
    // TODO: Add your control notification handler code here
      UpdateData(TRUE);

      LOGIN_MODE mode = BIT_MODE_AUTO_CACHE;
          
      BIT_STATUS status = g_bitanswer.UpdateOnline(CT2CA(m_strSN));
      
      if(status == BIT_SUCCESS)
      {
         status = g_bitanswer.Login(CT2CA(m_strSN), mode);
      }
      if (status != BIT_SUCCESS)
      {
         CString message;
         message.Format(_T("%s, %s"), g_oResStr[IDS_STRING457], getBitanswerErrorText(status));
        m_strText = message;
        UpdateData(FALSE);
      }
      else
      {
         OnOK();
      }
}


void COnlineRegisterDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}


void COnlineRegisterDlg::SetDefaultButton(UINT uID)
{

	UINT prevId = GetDefID();
	if (prevId == uID) return;
	CButton* pPrevButton = reinterpret_cast<CButton*>(GetDlgItem(prevId));
	DWORD style = 0;
	if (pPrevButton)
	{
		// remove default push button style
		// get the style
		DWORD style = pPrevButton->GetStyle();
		style &= ~BS_DEFPUSHBUTTON;
		// set the style
		::SendMessage(pPrevButton->GetSafeHwnd(), BM_SETSTYLE, (WPARAM)style, (LPARAM)TRUE);
		
	}

	CButton* pNewDefaultButton = reinterpret_cast<CButton*>(GetDlgItem(uID));

	// inform the dialog about the new default control id
	SendMessage(DM_SETDEFID, uID);

	// get the style
	style = pNewDefaultButton->GetStyle();
	// add the default push button style
	style |= BS_DEFPUSHBUTTON;
	// set the style
	::SendMessage(pNewDefaultButton->GetSafeHwnd(), BM_SETSTYLE, (WPARAM)style, (LPARAM)TRUE);

	
}

