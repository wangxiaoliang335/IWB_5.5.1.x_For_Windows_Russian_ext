/**
* CBitAnswer 类，接口的封装实现
*/
//#include "StdAfx.h"
#include "BitAnswerCpp.h"

CBitAnswer::CBitAnswer(void)
{
   m_ref = 0;
}

CBitAnswer::~CBitAnswer(void)
{
   // 释放类的时候，调用Logout释放相关资源
   if (m_ref > 0)
   {
      Bit_Logout(m_handle);
   }
}

/**
* 进行授权登陆，获取操作句柄。必须在除升级函数之外的其它操作前执行。根据登录模式的不同可能需要连接授权服务器。
*/
BIT_STATUS	CBitAnswer::Login(BIT_PCSTR szSN, LOGIN_MODE mode)
{
   if (m_ref == 0)
   {
      BIT_STATUS status = Bit_Login(NULL, szSN, m_application_data, &m_handle, mode);
      if (status != BIT_SUCCESS)
      {
         return status;
      }
      m_sn = szSN;
      m_mode = mode;
   }
   m_ref++;
   return BIT_SUCCESS;
}

/**
* 进行授权登陆，获取操作句柄。必须在除升级函数之外的其它操作前执行。根据登录模式的不同可能需要连接授权服务器。
*/
BIT_STATUS	CBitAnswer::LoginEx(BIT_PCSTR szSN, BIT_UINT32 featureId, LOGIN_MODE mode)
{
   if (m_ref == 0)
   {
      BIT_STATUS status = Bit_LoginEx(NULL, szSN, featureId, NULL, m_application_data, &m_handle, mode);
      if (status != BIT_SUCCESS)
      {
         return status;
      }
      m_sn = szSN;
      m_mode = mode;
   }
   m_ref++;
   return BIT_SUCCESS;
}

/**
* 授权退出
*/
BIT_STATUS	CBitAnswer::Logout()
{
   if (m_ref > 0)
   {
      m_ref--;
   }
   if (m_ref == 0)
   {
      return Bit_Logout(m_handle);
   }
   return BIT_SUCCESS;
}

/**
* 此函数用于读取特征项的数据内容，可用于“只读”和“读写”特征类型。
*/
BIT_STATUS	CBitAnswer::ReadFeature(BIT_UINT32 featureId, BIT_UINT32 *pFeatureValue)
{
   BIT_STATUS status = Bit_ReadFeature(m_handle, featureId, pFeatureValue);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_ReadFeature(m_handle, featureId, pFeatureValue);
      }
   }
   return status;
}

/**
* 此函数用于更新“读写”类型的特征项的数据内容。
*/
BIT_STATUS	CBitAnswer::WriteFeature(BIT_UINT32 featureId, BIT_UINT32 featureValue)
{
   BIT_STATUS status = Bit_WriteFeature(m_handle, featureId, featureValue);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_WriteFeature(m_handle, featureId, featureValue);
      }
   }
   return status;
}

/**
* 此函数用于使用“算法”类型的特征项对输入参数进行变换操作，得到唯一对应的4字节结果。
*/
BIT_STATUS	CBitAnswer::ConvertFeature(BIT_UINT32 featureId, BIT_UINT32 para1, BIT_UINT32 para2, BIT_UINT32 para3, BIT_UINT32 para4, BIT_UINT32 *pResult)
{
   BIT_STATUS status = Bit_ConvertFeature(m_handle, featureId, para1, para2, para3, para4, pResult);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_ConvertFeature(m_handle, featureId, para1, para2, para3, para4, pResult);
      }
   }
   return status;
}

/**
* 此函数用于使用“密钥”类型的特征项对输入的明文进行加密，返回密文结果。
*/
BIT_STATUS	CBitAnswer::EncryptFeature(BIT_UINT32 featureId, BIT_UCHAR *pPlainBuffer, BIT_UCHAR *pCipherBuffer, BIT_UINT32 bufferSize)
{
   BIT_STATUS status = Bit_EncryptFeature(m_handle, featureId, pPlainBuffer, pCipherBuffer, bufferSize);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_EncryptFeature(m_handle, featureId, pPlainBuffer, pCipherBuffer, bufferSize);
      }
   }
   return status;
}

/**
* 此函数用于使用“密钥”类型的特征项对输入的密文进行解密，返回明文结果。
*/
BIT_STATUS	CBitAnswer::DecryptFeature(BIT_UINT32 featureId, BIT_UCHAR *pCipherBuffer, BIT_UCHAR *pPlainBuffer, BIT_UINT32 bufferSize)
{
   BIT_STATUS status = Bit_DecryptFeature(m_handle, featureId, pCipherBuffer, pPlainBuffer, bufferSize);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_DecryptFeature(m_handle, featureId, pCipherBuffer, pPlainBuffer, bufferSize);
      }
   }
   return status;
}

/**
* 获取“用户”类型特征项的用户数，并返回剩余容量。如没有可分配用户数，则返回1810错误。
*/
BIT_STATUS	CBitAnswer::QueryFeature(BIT_UINT32 featureId, BIT_UINT32 *pCapacity)
{
   BIT_STATUS status = Bit_QueryFeature(m_handle, featureId, pCapacity);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_QueryFeature(m_handle, featureId, pCapacity);
      }
   }
   return status;
}

/**
* 释放“用户”类型特征项的用户数，并返回剩余容量。
*/
BIT_STATUS	CBitAnswer::ReleaseFeature(BIT_UINT32 featureId, BIT_UINT32 *pCapacity)
{
   BIT_STATUS status = Bit_ReleaseFeature(m_handle, featureId, pCapacity);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_ReleaseFeature(m_handle, featureId, pCapacity);
      }
   }
   return status;
}

/**
* 创建或更新配置项。如果相同名称的配置项存在，则会更新其中的数据；否则将添加新的授权码配置项。
*/
BIT_STATUS	CBitAnswer::SetDataItem(BIT_PCSTR szDataItemName, BIT_PCSTR szDataValue)
{
   BIT_STATUS status = Bit_SetDataItem(m_handle, szDataItemName, (void*)szDataValue, strlen(szDataValue));
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_SetDataItem(m_handle, szDataItemName, (void*)szDataValue, strlen(szDataValue));
      }
   }
   return status;
}

/**
* 删除指定的配置项。该操作无法删除在比特平台上设置的产品配置项或模版配置项。
*/
BIT_STATUS	CBitAnswer::RemoveDataItem(BIT_PCSTR dataItemName)
{
   BIT_STATUS status = Bit_RemoveDataItem(m_handle, dataItemName);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_RemoveDataItem(m_handle, dataItemName);
      }
   }
   return status;
}

/**
* 读取指定的配置项数据。
*/
BIT_STATUS	CBitAnswer::GetDataItem(BIT_PCSTR szDataItemName, string& dataValue)
{
   BIT_CHAR *pDataItemValue = new BIT_CHAR[1024];
   BIT_UINT32 dataItemValueSize = 1024;
   BIT_STATUS status = Bit_GetDataItem(m_handle, szDataItemName, pDataItemValue, &dataItemValueSize);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_GetDataItem(m_handle, szDataItemName, pDataItemValue, &dataItemValueSize);
      }
   }
   dataValue = pDataItemValue;
   if (pDataItemValue != NULL)
   {
      delete[] pDataItemValue;
   }
   return status;
}

/**
* 此函数用于获取可访问配置项的数量，一般用于配置项的枚举操作。
*/
BIT_STATUS CBitAnswer::GetDataItemNum(BIT_UINT32 *pNumber)
{
   BIT_STATUS status = Bit_GetDataItemNum(m_handle, pNumber);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_GetDataItemNum(m_handle, pNumber);
      }
   }
   return status;
}

/**
* 根据配置项索引获取其名称，一般用于配置项的枚举操作。
*/
BIT_STATUS CBitAnswer::GetDataItemName(BIT_UINT32 index, string &dataName)
{
   BIT_CHAR *pDataName = new BIT_CHAR[1024];
   BIT_UINT32 dataItemNameSize = 1024;
   BIT_STATUS status = Bit_GetDataItemName(m_handle, index, pDataName, &dataItemNameSize);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_GetDataItemName(m_handle, index, pDataName, &dataItemNameSize);
      }
   }
   dataName = pDataName;
   if (pDataName != NULL)
   {
      delete[] pDataName;
   }
   return status;
}

/**
* 获取当前会话信息。根据获取的内容不同，此函数返回的数据可能是XML格式或非XML格式。
*/
BIT_STATUS	CBitAnswer::GetSessionInfo(SESSION_TYPE type, string &sessionInfo)
{
   BIT_UINT32 sessionInfoSize = 1024;
   BIT_CHAR *pSessionInfo = new BIT_CHAR[sessionInfoSize];
   if (pSessionInfo == NULL)
   {
      return BIT_ERR_MALLOC_FAILED;
   }
   BIT_STATUS status = Bit_GetSessionInfo(m_handle, type, pSessionInfo, &sessionInfoSize);
   if (status == BIT_ERR_NEED_LOGIN)
   {
      m_ref = 0;
      status = Login(m_sn, m_mode);
      if (status == BIT_SUCCESS)
      {
         status = Bit_GetSessionInfo(m_handle, type, pSessionInfo, &sessionInfoSize);
      }
   }
   if (status == BIT_ERR_BUFFER_SMALL)
   {
      delete[] pSessionInfo;
      pSessionInfo = new BIT_CHAR[sessionInfoSize];
      if (pSessionInfo == NULL)
      {
         return BIT_ERR_MALLOC_FAILED;
      }
      status = Bit_GetSessionInfo(m_handle, type, pSessionInfo, &sessionInfoSize);
   }
   sessionInfo = pSessionInfo;
   delete[] pSessionInfo;

   return status;
}

/**
* 获取授权信息及使用环境信息，数据以XML格式返回。调用此函数前客户端不需要执行登录操作。
*/
BIT_STATUS	CBitAnswer::GetInfo(BIT_PCSTR szSN, BIT_INFO_TYPE type, string& info)
{
   BIT_UINT32 infoSize = 1024;
   BIT_CHAR *pInfo = new BIT_CHAR[infoSize];
   if (pInfo == NULL)
   {
      return BIT_ERR_MALLOC_FAILED;
   }
   BIT_STATUS status = Bit_GetInfo(szSN, m_application_data, type, pInfo, &infoSize);
   if (status == BIT_ERR_BUFFER_SMALL)
   {
      delete[] pInfo;
      pInfo = new BIT_CHAR[infoSize];
      if (pInfo == NULL)
      {
         return BIT_ERR_MALLOC_FAILED;
      }
      status = Bit_GetInfo(szSN, m_application_data, type, pInfo, &infoSize);
   }

   info = pInfo;
   delete[] pInfo;

   return status;
}

/**
* 获取当前运行环境的升级请求码，用于发起本地授权激活及升级请求。
*/
BIT_STATUS CBitAnswer::GetRequestInfo(BIT_PCSTR szSN, BINDING_TYPE type, string& requestInfo)
{
   BIT_CHAR *pRequestInfo = NULL;
   BIT_UINT32 requestInfoSize = 10240;
   pRequestInfo = new BIT_CHAR[requestInfoSize];
   if (pRequestInfo == NULL)
   {
      return BIT_ERR_MALLOC_FAILED;
   }
   BIT_STATUS status = Bit_GetRequestInfo(szSN, m_application_data, type, pRequestInfo, &requestInfoSize);
   if (status == BIT_ERR_BUFFER_SMALL)
   {
      delete[] pRequestInfo;
      pRequestInfo = new BIT_CHAR[requestInfoSize];
      if (pRequestInfo == NULL)
      {
         return BIT_ERR_MALLOC_FAILED;
      }
      status = Bit_GetRequestInfo(szSN, m_application_data, type, pRequestInfo, &requestInfoSize);
   }
   requestInfo = pRequestInfo;
   delete[] pRequestInfo;
   return status;
}

/**
* 使用请求码与授权服务器进行连接，获取升级码。本函数需要进行网络连接。
*/
BIT_STATUS CBitAnswer::GetUpdateInfo(BIT_PCSTR szSN, BIT_PCSTR pRequestInfo, string &updateInfo)
{
   BIT_CHAR *pUpdateInfo = NULL;
   BIT_UINT32 updateInfoSize = 10240;
   pUpdateInfo = new BIT_CHAR[updateInfoSize];
   if (pUpdateInfo == NULL)
   {
      return BIT_ERR_MALLOC_FAILED;
   }
   BIT_STATUS status = Bit_GetUpdateInfo(NULL, szSN, m_application_data, pRequestInfo, pUpdateInfo, &updateInfoSize);
   if (status == BIT_ERR_BUFFER_SMALL)
   {
      delete[] pUpdateInfo;
      pUpdateInfo = new BIT_CHAR[updateInfoSize];
      if (pUpdateInfo == NULL)
      {
         return BIT_ERR_MALLOC_FAILED;
      }
      status = Bit_GetUpdateInfo(NULL, szSN, m_application_data, pRequestInfo, pUpdateInfo, &updateInfoSize);
   }
   updateInfo = pUpdateInfo;
   delete[] pUpdateInfo;
   return status;
}

/**
* 应用升级码完成本地授权激活或升级。本函数必须在获取请求码的同一环境下执行。
*/
BIT_STATUS CBitAnswer::ApplyUpdateInfo(BIT_PCSTR pUpdateInfo, string& receiptInfo)
{
   BIT_CHAR *pReceiptInfo = NULL;
   BIT_UINT32 receiptInfoSize = 10240;
   pReceiptInfo = new BIT_CHAR[receiptInfoSize];
   if (pReceiptInfo == NULL)
   {
      return BIT_ERR_MALLOC_FAILED;
   }
   BIT_STATUS status = Bit_ApplyUpdateInfo(m_application_data, pUpdateInfo, pReceiptInfo, &receiptInfoSize);
   if (status == BIT_ERR_BUFFER_SMALL)
   {
      delete[] pReceiptInfo;
      pReceiptInfo = new BIT_CHAR[receiptInfoSize];
      if (pReceiptInfo == NULL)
      {
         return BIT_ERR_MALLOC_FAILED;
      }
      status = Bit_ApplyUpdateInfo(m_application_data, pUpdateInfo, pReceiptInfo, &receiptInfoSize);
   }
   receiptInfo = pReceiptInfo;
   delete[] pReceiptInfo;
   return status;
}

/**
* 此函数用于与授权服务器在线连接，自动完成本地授权的升级操作。本函数需要进行网络连接。
*/
BIT_STATUS CBitAnswer::UpdateOnline(BIT_PCSTR szSN)
{
   return Bit_UpdateOnline(NULL, szSN, m_application_data);
}

/**
* 此函数用于完成授权的在线迁出，迁出以后，其他机器就可以用了。本函数需要进行网络连接。
*/
BIT_STATUS CBitAnswer::RevokeOnline(BIT_PCSTR szSN)
{
   return Bit_Revoke(NULL, szSN, m_application_data, NULL, NULL);
}

/**
* 此函数用于与产生授权的离线迁出请求码。本函数需要进行网络连接。
*/
BIT_STATUS CBitAnswer::Revoke(BIT_PCSTR szSN, string& revokeInfo)
{
   BIT_CHAR *pRevocationInfo = NULL;
   BIT_UINT32 revocationInfoSize = 10240;
   pRevocationInfo = new BIT_CHAR[revocationInfoSize];
   if (pRevocationInfo == NULL)
   {
      return BIT_ERR_MALLOC_FAILED;
   }
   BIT_STATUS status = Bit_Revoke(NULL, szSN, m_application_data, pRevocationInfo, &revocationInfoSize);
   if (status == BIT_ERR_BUFFER_SMALL)
   {
      delete[] pRevocationInfo;
      pRevocationInfo = new BIT_CHAR[revocationInfoSize];
      if (pRevocationInfo == NULL)
      {
         return BIT_ERR_MALLOC_FAILED;
      }
      status = Bit_Revoke(NULL, szSN, m_application_data, pRevocationInfo, &revocationInfoSize);
   }
   revokeInfo = pRevocationInfo;
   delete[] pRevocationInfo;
   return status;
}

/**
* 此函数用于设置集团服务的地址和端口。
*/
BIT_STATUS CBitAnswer::SetLocalServer(BIT_PCSTR szHostName, BIT_UINT32 nPort, BIT_UINT32 nTimeoutSeconds)
{
   return Bit_SetLocalServer(m_application_data, szHostName, nPort, nTimeoutSeconds);
}

/**
* 此函数用于设置代理服务的地址和端口。
*/
BIT_STATUS CBitAnswer::SetProxy(BIT_PCSTR szHostName, BIT_UINT32 nPort, BIT_PCSTR szUserID, BIT_PCSTR szPassword)
{
   return Bit_SetProxy(m_application_data, szHostName, nPort, szUserID, szPassword);
}

/**
* 此函数用于删除授权。
*/
BIT_STATUS CBitAnswer::RemoveSn(BIT_PCSTR szSN)
{
   return Bit_RemoveSn(szSN, m_application_data);
}

/**
 * 获取授权信息及使用环境信息，数据以XML格式返回。调用此函数前客户端不需要执行登录操作。
 */
BIT_STATUS CBitAnswer::GetInfo(BIT_PCSTR szSN, BIT_INFO_TYPE type, BIT_CHAR *pInfo, BIT_UINT32 *pInfoSize)
{
   return Bit_GetInfo(szSN, m_application_data, type, pInfo, pInfoSize);
}

/**
 * 获取客户端安全库版本号。
 */
BIT_STATUS CBitAnswer::GetVersion(BIT_UINT32 *pVersion)
{
   return Bit_GetVersion(pVersion);
}

/**
 * 从集团授权服务器借出一个完整的授权码，以允许客户端脱离集团服务器单独使用。
 *   被借出的集团授权码必须具有可借出属性，并在客户端成功借出后减少一个可用用户数。
 *   被借出的用户数在到期后将自动返还给集团服务器。
 */
BIT_STATUS CBitAnswer::CheckOutSn(BIT_PCSTR szURL, BIT_UINT32 featureId, BIT_UINT32 nDurationDays)
{
   return Bit_CheckOutSn(szURL, featureId, m_application_data, nDurationDays);
}

/**
 * 从集团授权服务器借出一组特征项，这些特征项必须包含在同一个授权码中。
 *   被借出的集团授权码必须具有可借出属性，并在客户端成功借出后减少一个可用用户数。
 *   被借出的用户数在到期后将自动返还给集团服务器。
 */
BIT_STATUS CBitAnswer::CheckOutFeatures(BIT_PCSTR szURL, BIT_UINT32 *pFeatureList, BIT_UINT32 nFeatureInList, BIT_UINT32 nDurationDays)
{
   return Bit_CheckOutFeatures(szURL, m_application_data, pFeatureList, nFeatureInList, nDurationDays);
}

/**
 * 提前返还从集团授权服务器借出的授权。要提前返还授权，该授权码必须具有允许提前返还属性。
 */
BIT_STATUS CBitAnswer::CheckIn(BIT_PCSTR szURL, BIT_UINT32 featureId)
{
   return Bit_CheckIn(szURL, featureId, m_application_data);
}

/**
 * 测试集团授权和特征项是否可用，不会占用授权码或特征项的用户数。
 */
BIT_STATUS CBitAnswer::TestBitService(BIT_PCSTR szURL, BIT_PCSTR szSN, BIT_UINT32 featureId)
{
   return Bit_TestBitService(szURL, szSN, featureId, m_application_data);
}
