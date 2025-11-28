/*
 *
 * CBitAnswer 类的定义，开发人员可以直接调用操作授权
 *
 * BitAnswer Ltd. (C) 2009 - ?. All rights reserved.
 *
*/
#pragma once
#include <string>
using namespace std;

#if !defined(WITH_AKSTYPES) && !defined(WITH_OEFTYPES)
#if defined(_MSC_VER) || defined(__WATCOMC__) || defined(__BORLANDC__)
typedef unsigned __int64   BIT_UINT64;
typedef signed __int64     BIT_INT64;
#else
typedef unsigned long long BIT_UINT64;
typedef signed long long   BIT_INT64;
#endif
#if defined(_MSC_VER)
typedef unsigned long      BIT_UINT32;
typedef signed long        BIT_INT32;
#else
typedef unsigned int       BIT_UINT32;
typedef signed int         BIT_INT32;
#endif
typedef unsigned short     BIT_USHORT;
typedef signed short       BIT_SHORT;
typedef unsigned char      BIT_UCHAR;
typedef char               BIT_CHAR;
typedef const char         *BIT_PCSTR;
#endif

typedef void               *BIT_HANDLE;
typedef void               BIT_VOID;
typedef enum               BIT_ERROR_CODES BIT_STATUS;

#if defined(_WIN32) || defined(WIN32) || defined(_MSC_VER) || defined(__BORLANDC__)
#define BIT_CALLCONV __stdcall
#else
#define BIT_CALLCONV
#endif

#if _MSC_VER >= 1900
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif

#ifdef _WIN64
#ifdef _MT
#ifdef _DLL
#pragma comment(lib, "00003027_00000004_md_x64.lib")
#else
#pragma comment(lib, "00003027_00000004_mt_x64.lib")
#endif
#else
#pragma comment(lib, "00003027_00000004_mt_x64.lib")
#endif
#else if defined(_WIN32) || defined(WIN32) 
#ifdef _MT
    #ifdef _DLL
        #pragma comment(lib, "..\\bitanswer\\00003027_00000004_md.lib")
    #else
        #pragma comment(lib, "..\\bitanswer\\00003027_00000004_mt.lib")
    #endif
#else
        #pragma comment(lib, "..\\bitanswer\\00003027_00000004_mt.lib")
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
/** BITANSWER  下面部分不要修改 */

   /** The Application Data is confidential information, please keep it carefully! */
   static BIT_UCHAR m_application_data[] = {
   0x40,0x80,0xbc,0x29,0x80,0x22,0x0c,0x44,0x4a,0x78,0xf5,0x0f,0xc7,0xa2,0xca,0x30,
    0xcd,0x28,0xcd,0x08,0xda,0xe2,0xe3,0x4c,0x5e,0x4f,0x7e,0xce,0xc2,0xda,0xa7,0xca,
    0x55,0x53,0xc0,0x29,0x6d,0x63,0x7b,0xfb,0x3c,0x0b,0x63,0x87,0xcd,0x62,0x25,0xf9,
    0xd3,0x1c,0x9f,0x42,0x6c,0x1e,0xdf,0xba,0x81,0x2e,0xb0,0xef,0x66,0xc9,0x0f,0x47,
    0xdf,0x02,0xa8,0x5c,0x1a,0x5e,0x93,0xad,0x3e,0xc9,0x4a,0x15,0x30,0xdc,0xe6,0x4f,
    0xec,0xf2,0x8d,0xad,0x7d,0xa0,0x6f,0xb4,0x72,0xd3,0xce,0xb1,0x92,0xca,0xa1,0xeb,
    0x3d,0x09,0xe0,0x50,0xa4,0xf2,0xad,0xd6,0x95,0xc8,0xca,0x23,0x98,0xd2,0x49,0x8d,
    0xed,0x9e,0x51,0xaf,0x34,0xd8,0xa3,0x5d,0x49,0x4f,0xa8,0xe9,0x9b,0x2d,0x48,0x15,
    0xf8,0x7e,0xf1,0x90,0x63,0x14,0x6e,0x44,0x76,0x1d,0x54,0x81,0xda,0xf5,0x80,0x30,
    0x7f,0x5f,0xa8,0x66,0x7f,0x0d,0xb0,0x50,0x76,0xc2,0x84,0xfc,0x0c,0x9b,0x2d,0xde,
    0x75,0x02,0x84,0xe9,0x1e,0x57,0xba,0x22,0xd5,0xe2,0x3a,0xc2,0x8d,0x4a,0x3c,0xd9,
    0x8b,0x68,0x57,0x1e,0x48,0x59,0x16,0xf8,0x46,0xcc,0xb5,0xec,0x41,0x85,0x7c
   };

   typedef enum _FEATURE_TYPE
   {
      BIT_FEATURE_CONVERT        = 0x03,
      BIT_FEATURE_READ_ONLY      = 0x04,
      BIT_FEATURE_READ_WRITE     = 0x05,
      BIT_FEATURE_CRYPTION       = 0x09,
      BIT_FEATURE_USER           = 0x0a,
      BIT_FEATURE_UNIFIED        = 0x0b,
      BIT_FEATURE_NONE           = 0xFFFFFFFF,
   } FEATURE_TYPE;

   typedef enum _LOGIN_MODE
   {
      BIT_MODE_LOCAL             = 0x01,
      BIT_MODE_REMOTE            = 0x02,
      BIT_MODE_AUTO              = 0x03,
      BIT_MODE_AUTO_CACHE        = 0x07
   } LOGIN_MODE;

#define LOGIN_MODE_CACHE         4
#define LOGIN_MODE_USB           8
#define LOGIN_MODE_PROCESS       0x10

   typedef enum _BINDING_TYPE
   {
      BINDING_EXISTING           = 0,
      BINDING_LOCAL              = 1,
      BINDING_USB_STORAGE        = 2
   } BINDING_TYPE;

   typedef enum _BIT_INFO_TYPE
   {
      BIT_INFO_SERVER_ADDRESS    = 0,
      BIT_INFO_SN                = 1,
      BIT_INFO_SN_FEATURE        = 2,
      BIT_INFO_SN_LICENSE        = 3,
      BIT_INFO_UPDATE_ERROR      = 4,
      BIT_INFO_CONFIG            = 5
   } BIT_INFO_TYPE;
#pragma pack(1)

   typedef struct _BIT_DATE_TIME
   {
      BIT_USHORT  year;
      BIT_UCHAR   month;
      BIT_UCHAR   dayOfMonth;
      BIT_UCHAR   hour;
      BIT_UCHAR   minute;
      BIT_UCHAR   second;
      BIT_UCHAR   unused;
   } BIT_DATE_TIME, *PBIT_DATE_TIME;

   typedef struct _BIT_FEATURE_INFO
   {
      BIT_UINT32      featureId;
      FEATURE_TYPE    type;
      BIT_CHAR        featureName[128];
      BIT_DATE_TIME   endDateTime;
      BIT_UINT32      expirationDays;
      BIT_UINT32      users;
   } BIT_FEATURE_INFO, *PBIT_FEATURE_INFO;

#pragma pack()

   /**
   * @defgroup Run-time API Status Codes
   *
   * @{
   */
   typedef enum BIT_ERROR_CODES
   {
      BIT_SUCCESS                       = 0x0000,
      
      /** client side error */
      BIT_ERR_NETWORK                   = 0x0101,
      BIT_ERR_WRONG_HANDLE              = 0x0102,
      BIT_ERR_INVALID_PARAMETER         = 0x0103,
      BIT_ERR_BUFFER_SMALL              = 0x0104,
      BIT_ERR_APPLICATION_DATA          = 0x0105,
      BIT_ERR_HEARTBEAT_THREAD          = 0x0106,
      BIT_ERR_SERVER_BUSY               = 0x0107,
      BIT_ERR_SERVER_DOWN               = 0x0108,
      BIT_ERR_RETURN_NULL               = 0x0109,
      BIT_ERR_CMD_TYPE                  = 0x010A,
      BIT_ERR_BADSTRING                 = 0x010B,
      BIT_ERR_HANDLE_NULL               = 0x010C,
      BIT_ERR_INVALID_OFF_LINE_SN       = 0x010E,
      BIT_ERR_OFF_LINE_SN_DENIED        = 0x010F,
      BIT_ERR_SN_FILE_NOT_FOUND         = 0x0110,
      BIT_ERR_OFF_LINE_SN_SIGNATURE     = 0x0111,
      BIT_ERR_RUN_TIME_ERROR            = 0x0112,
      BIT_ERR_CHECK_THREAD              = 0x0113,
      BIT_ERR_LICENSE_NOT_FOUND         = 0x0114,
      BIT_ERR_UNEXPECTED                = 0x0115,
      BIT_ERR_DOWNLOAD_UNCOMPLETE       = 0x0116,
      BIT_ERR_POINTS_OVER               = 0x0117,
      BIT_ERR_TLV_BUF_OVER              = 0x0118,
      BIT_ERR_MORE_USB_DISK             = 0x0119,
      BIT_ERR_NO_USB_DISK               = 0x011A,
      BIT_ERR_GET_MID                   = 0x011B,
      BIT_ERR_SN_INVALID                = 0x011C,
      BIT_ERR_TIME_TAMPER               = 0x011D,
      BIT_ERR_NOT_ENCRYPTED             = 0x011E,
      BIT_ERR_KEY_NOT_MATCH             = 0x011F,
      BIT_ERR_BINDINGPOLICY_NOT_MATCHED = 0x0120,
      BIT_ERR_LOAD_EXTENSION_FAILED     = 0x0121,
      BIT_ERR_MALLOC_FAILED             = 0x0122,
      BIT_ERR_REVOKED                   = 0x0123,
      BIT_ERR_EXTENSION_EXIST           = 0x0124,
      BIT_ERR_INIT_DBPATH               = 0x0125,
      BIT_ERR_LOCAL_HOST_NULL           = 0x0126,
      BIT_ERR_WRONG_SERVER_TYPE         = 0x0127,
      BIT_ERR_SOAP                      = 0x0128,
      BIT_ERR_UDP                       = 0x0129,
      BIT_ERR_UPDATE_CODE               = 0x012A,
      BIT_ERR_UPDATE_INFO_VERSION       = 0x012B,
      BIT_ERR_TYPE_NOT_MATCHED          = 0x012C,
      BIT_ERR_INVALID_URL               = 0x012D,
      BIT_ERR_UPDATE_INFO_FORMAT        = 0x012E,
      BIT_ERR_SN_EMPTY                  = 0x012F,

      BIT_ERR_SN_ENTRY_NOT_FOUND        = 0x0131,
      BIT_ERR_FUNCTION_NOT_SUPPORT      = 0x0132,

      BIT_ERR_ACCESS_ERROR              = 0x0151,
      BIT_ERR_ACCESS_DENIED             = 0x0152,
      BIT_ERR_ACCESS_NOT_FOUND          = 0x0153,

      BIT_ERR_TOKEN_NOT_SUPPORT         = 0x0181,
      BIT_ERR_TOKEN_NOT_FOUND           = 0x0182,
      BIT_ERR_TOKEN_ACCESS_FAILED       = 0x0183,
      BIT_ERR_TOKEN_READ_FAILED         = 0x0184,
      BIT_ERR_TOKEN_WRITE_FAILED        = 0x0185,
      BIT_ERR_TOKEN_NOT_INIT            = 0x0186,
      BIT_ERR_TOKEN_ALREADY_INIT        = 0x0187,

      /** server side error */
      BIT_ERR_NEED_LOGIN                = 0x0201,
      BIT_ERR_NULL_PARAMETER            = 0x0202,
      BIT_ERR_SRV_INVALID_PARAMETER     = 0x0203,
      BIT_ERR_UNKNOWN_EXCEPTION         = 0x0204,
      BIT_ERR_UNKNOWN_VAR               = 0x0205,
      BIT_ERR_PRODUCT_DISABLED          = 0x0206,
      BIT_ERR_DEVELOPER_DISABLED        = 0x0207,
      BIT_ERR_WEB_SERVICE_STOP          = 0x0208,
      BIT_ERR_PRODUCT_NOT_MATCH         = 0x020A,
      BIT_ERR_SERVER_KICKOUT            = 0x0212,
      BIT_ERR_SERVER_DENY               = 0x0213,
      BIT_ERR_OVER_USERS_IN_IP          = 0x0214,

      BIT_ERR_STRING_LENGH              = 0x0301,
      BIT_ERR_BADCOMMAND                = 0x0302,
      BIT_ERR_CMD_LENGTH                = 0x0303,

      BIT_ERR_FEATURE_ADDR              = 0x0501,
      BIT_ERR_FEATURE_ZERO              = 0x0502,
      BIT_ERR_FEATURE_NOT_FOUND         = 0x0503,
      BIT_ERR_FEATURE_TYPE_NOT_MATCH    = 0x0504,
      BIT_ERR_FEATURE_TYPE_PARAM        = 0x0505,
      BIT_ERR_FEATURE_EXPIRATION        = 0x0509,

      BIT_ERR_DATA_NOT_FOUND            = 0x0601,
      BIT_ERR_DATA_INDEX_OVER_LENGTH    = 0x0602,
      BIT_ERR_DATA_OVER_MAX_SIZE        = 0x0603,
      BIT_ERR_DATA_OVER_MAX_COUNT       = 0x0604,
      
      BIT_ERR_SN_LICENSE_EXPIRATION     = 0x0701,
      BIT_ERR_SN_USER_NUMBER_OVER       = 0x0702,
      BIT_ERR_SN_VOLUME_NUMBER_OVER     = 0x0703,
      BIT_ERR_SN_EXECUTION_COUNTER_OVER = 0x0704,
      BIT_ERR_SN_DISABLED               = 0x0705,
      BIT_ERR_SN_NOT_FOUND              = 0x0706,
      BIT_ERR_SN_OVER_MAX_SESSION_COUNT = 0x0707,
      BIT_ERR_SN_EXECUTE_MORE_COMMAND   = 0x0708,
      BIT_ERR_SN_DISABLED_FOR_ATTACK    = 0x0709,
      BIT_ERR_MISSING_USB               = 0x070D,
      BIT_ERR_SN_INVALID_FORMAT         = 0x070E,
      BIT_ERR_CAPACITY_EXHAUSTED        = 0x0712,
      BIT_ERR_CAPACITY_NOT_USED         = 0x0713,
      BIT_ERR_SN_TYPE_NOT_MATCH         = 0x0714,
      BIT_ERR_SN_OFFLINE_EXPIRED        = 0x0780,
      BIT_ERR_SN_MID_CHECK              = 0x0781,
      BIT_ERR_MACHINE_DISABLED          = 0x0782,
      BIT_ERR_NO_AVAILABLE_SN           = 0x0783,
      BIT_ERR_NO_AVAILABLE_MODULE       = 0x0784,
      BIT_ERR_DEMO_UPDATE               = 0x0785,
      BIT_ERR_VM_DISABLED               = 0x0786,
      BIT_ERR_SN_INVALID_STATUS         = 0x0787,

      BIT_ERR_OFF_LINE_ERR              = 0x0801,
      BIT_ERR_OFF_LINE                  = 0x0802,
      BIT_ERR_DOWNLOAD_OVER             = 0x0803,
      BIT_ERR_SIGNATURE                 = 0x0804,
      BIT_ERR_OFF_LINE_UPDATE_AGAIN     = 0x0806,
      BIT_ERR_WRONG_LOCAL_TIME          = 0x0807,
      BIT_ERR_NO_AVAILABLE_UPDATE       = 0x0808,
      BIT_ERR_LOCAL_SERVICE_UPDATE      = 0x0809,  
          
      BIT_ERR_PRODUCT_NOT_FOUND         = 0x0901,
      BIT_ERR_DEVELOPER_NOT_FOUND       = 0x0902,
      BIT_ERR_INVALID_MID_FROMAT        = 0x0903,
      BIT_ERR_COMMAND_NOT_SUPPORT       = 0x0904,
      BIT_ERR_VERSION_NOT_SUPPORT       = 0x0905,
      BIT_ERR_BINDING_INFO              = 0x0906,
      BIT_ERR_CMD_DATA_LENGTH           = 0x0907,
      BIT_ERR_TABLE_SIGN                = 0x090A,
      BIT_ERR_NOT_INIT                  = 0x090B,

      /** for bit service */
      BIT_ERR_BIT_SERVICER_SIGN         = 0x0A01,
      BIT_ERR_CONFIG_FILE               = 0x0A03,
      BIT_ERR_INVALID_USER              = 0x0A04,
      BIT_ERR_OLD_PASSWORD_WRONG        = 0x0A06,
      BIT_ERR_PASSWORD_NOT_MATCHED      = 0x0A07,
      BIT_ERR_UPLOAD_FAILURE            = 0x0A09,
      BIT_ERR_LOAD_EXTENTION            = 0x0A11,
      BIT_ERR_PAGE_NOT_FOUND            = 0x0A12,
      BIT_ERR_SN_EXISTED                = 0x0A13,
      BIT_ERR_OLD_EXTENTION             = 0x0A14,
      BIT_ERR_SRV_ROLE_INVALID          = 0x0A15,
      BIT_ERR_SRV_ROLE_EXIST            = 0x0A16
  }BIT_STATUS;

  #define BIT_DATA_ITEM_NAME_MAX_LEN   128   
  #define BIT_DATA_ITEM_MAX_LEN        1024
  #define BIT_CIPH_DATA_MAX_LEN        256


   /**
   * @defgroup SESSION_TYPE of session information in XML or plain format
   *
   * @{
   */
   typedef enum _SESSION_TYPE 
   { 
      XML_TYPE_SN_INFO          = 3,
      XML_TYPE_FEATURE_INFO     = 4,
      BIT_SERVER_ADDRESS        = 0x101,
      BIT_SERVER_TIME           = 0x201,
      BIT_CONTROL_TYPE          = 0x302, 
      BIT_VOLUME_NUMBER         = 0x303,
      BIT_START_DATE            = 0x304, 
      BIT_END_DATE              = 0x305, 
      BIT_EXPIRATION_DAYS       = 0x306, 
      BIT_USAGE_NUMBER          = 0x307,
      BIT_CONSUMED_USAGE_NUMBER = 0x308,
      BIT_ACTIVATE_DATE         = 0x30A, 
      BIT_USER_LIMIT            = 0x30B,
      BIT_LAST_UPDATE_DATE      = 0x30C,
      BIT_MAX_OFFLINE_MINUTES   = 0x30D,
      BIT_NEXT_CONNECT_DATE     = 0x30E,
      BIT_MID_BINARY            = 0x1001
   } SESSION_TYPE;

   /**
   * @}
   */

   /**
   * @defgroup The Basic Run-time API
   *
   * @{
   */
   BIT_STATUS BIT_CALLCONV Bit_Login( 
      BIT_PCSTR       szURL,
      BIT_PCSTR       szSN,
      BIT_UCHAR       *pApplicationData,
      BIT_HANDLE      *pHandle,
      LOGIN_MODE      mode);

   BIT_STATUS BIT_CALLCONV Bit_LoginEx(
      BIT_PCSTR       szURL,
      BIT_PCSTR       szSN,
      BIT_UINT32      featureId,
      BIT_PCSTR       szReserved,
      BIT_UCHAR       *pApplicationData,
      BIT_HANDLE      *pHandle,
      LOGIN_MODE      mode);

   BIT_STATUS BIT_CALLCONV Bit_Logout(
      BIT_HANDLE      handle); 

   BIT_STATUS BIT_CALLCONV Bit_ReadFeature( 
      BIT_HANDLE      handle,
      BIT_UINT32      featureId,
      BIT_UINT32      *pFeatureValue);

   BIT_STATUS BIT_CALLCONV Bit_WriteFeature( 
      BIT_HANDLE      handle,
      BIT_UINT32      featureId,
      BIT_UINT32      featureValue);

   BIT_STATUS BIT_CALLCONV Bit_ConvertFeature( 
      BIT_HANDLE      handle,
      BIT_UINT32      featureId,
      BIT_UINT32      para1,
      BIT_UINT32      para2,
      BIT_UINT32      para3,
      BIT_UINT32      para4,
      BIT_UINT32      *pResult);

   BIT_STATUS BIT_CALLCONV Bit_EncryptFeature( 
      BIT_HANDLE      handle,
      BIT_UINT32      featureId,
      BIT_VOID        *pPlainBuffer,
      BIT_VOID        *pCipherBuffer,
      BIT_UINT32      dataBufferSize);

   BIT_STATUS BIT_CALLCONV Bit_DecryptFeature( 
      BIT_HANDLE      handle,
      BIT_UINT32      featureId,
      BIT_VOID        *pCipherBuffer,
      BIT_VOID        *pPlainBuffer,
      BIT_UINT32      dataBufferSize);

   BIT_STATUS BIT_CALLCONV Bit_QueryFeature(
      BIT_HANDLE      handle,
      BIT_UINT32      featureId,
      BIT_UINT32      *pCapacity);

   BIT_STATUS BIT_CALLCONV Bit_ReleaseFeature(
      BIT_HANDLE      handle,
      BIT_UINT32      featureId,
      BIT_UINT32      *pCapacity);

   BIT_STATUS BIT_CALLCONV Bit_SetDataItem( 
      BIT_HANDLE      handle,
      BIT_PCSTR       szDataItemName,
      BIT_VOID        *pDataItemValue,
      BIT_UINT32      dataItemValueSize);

   BIT_STATUS BIT_CALLCONV Bit_RemoveDataItem( 
      BIT_HANDLE      handle,
      BIT_PCSTR       szDataItemName);

   BIT_STATUS BIT_CALLCONV Bit_GetDataItem( 
      BIT_HANDLE      handle,
      BIT_PCSTR       szDataItemName,
      BIT_VOID        *pDataItemValue,
      BIT_UINT32      *pDataItemValueSize);

   BIT_STATUS BIT_CALLCONV Bit_GetDataItemNum( 
      BIT_HANDLE      handle,
      BIT_UINT32      *pNum);

   BIT_STATUS BIT_CALLCONV Bit_GetDataItemName( 
      BIT_HANDLE      handle,
      BIT_UINT32      index,
      BIT_CHAR        *pDataItemName,
      BIT_UINT32      *pDataItemNameSize);

   BIT_STATUS BIT_CALLCONV Bit_GetFeatureInfo(
      BIT_HANDLE       handle,
      BIT_UINT32       featureId,
      BIT_FEATURE_INFO *pFeatureInfo);

   BIT_STATUS BIT_CALLCONV Bit_GetSessionInfo( 
      BIT_HANDLE      handle,
      SESSION_TYPE    type,
      BIT_CHAR        *pSessionInfo,
      BIT_UINT32      *pSessionInfoSize);

   BIT_STATUS BIT_CALLCONV Bit_GetRequestInfo( 
      BIT_PCSTR       szSN,
      BIT_UCHAR       *pApplicationData,
      BINDING_TYPE    type,
      BIT_CHAR        *pRequestInfo,
      BIT_UINT32      *pRrequestInfoSize);

   BIT_STATUS BIT_CALLCONV Bit_Revoke(
      BIT_PCSTR       szURL,
      BIT_PCSTR       szSN,
      BIT_UCHAR       *pApplicationData,
      BIT_CHAR        *pRevocationInfo,
      BIT_UINT32      *pRevocationInfoSize);

   BIT_STATUS BIT_CALLCONV Bit_GetUpdateInfo( 
      BIT_PCSTR       szURL,
      BIT_PCSTR       szSN,
      BIT_UCHAR       *pApplicationData,
      BIT_PCSTR       pRequestInfo,
      BIT_CHAR        *pUpdateInfo,
      BIT_UINT32      *pUpdateInfoSize);

   BIT_STATUS BIT_CALLCONV Bit_ApplyUpdateInfo( 
      BIT_UCHAR       *pApplicationData,
      BIT_PCSTR       pUpdateInfo,
      BIT_CHAR        *pReceiptInfo,
      BIT_UINT32      *pReceiptInfoSize);

   BIT_STATUS BIT_CALLCONV Bit_UpdateOnline(
      BIT_PCSTR       szURL,
      BIT_PCSTR       szSN,
      BIT_UCHAR       *pApplicationData);

   BIT_STATUS BIT_CALLCONV Bit_SetRootPath(
      BIT_PCSTR       szPath);

   BIT_STATUS BIT_CALLCONV Bit_GetProductPath(
      BIT_UCHAR       *pApplicationData,
      BIT_CHAR        *buf,
      BIT_UINT32      lenBuf);

   BIT_STATUS BIT_CALLCONV Bit_SetLocalServer(
      BIT_UCHAR       *pApplicationData,
      BIT_PCSTR       szHostName,
      BIT_UINT32      nPort,
      BIT_UINT32      nTimeoutSeconds);

   BIT_STATUS BIT_CALLCONV Bit_SetProxy(
      BIT_UCHAR       *pApplicationData,
      BIT_PCSTR       szHostName,
      BIT_UINT32      nPort,
      BIT_PCSTR       szUserID,
      BIT_PCSTR       szPassword);

   BIT_STATUS BIT_CALLCONV Bit_GetInfo(
      BIT_PCSTR       szSN,
      BIT_UCHAR       *pApplicationData,
      BIT_INFO_TYPE   type,
      BIT_CHAR        *pInfo,
      BIT_UINT32      *pInfoSize);

   BIT_STATUS BIT_CALLCONV Bit_GetVersion(
      BIT_UINT32      *pVersion);

   BIT_STATUS BIT_CALLCONV Bit_RemoveSn(
      BIT_PCSTR       szSN,
      BIT_UCHAR       *pApplicationData);
      
   BIT_STATUS BIT_CALLCONV Bit_CheckOutSn(
      BIT_PCSTR       szURL, 
      BIT_UINT32      featureId, 
      BIT_UCHAR       *pApplicationData,
      BIT_UINT32      nDurationDays);
      
   BIT_STATUS BIT_CALLCONV Bit_CheckOutFeatures(
      BIT_PCSTR       szURL, 
      BIT_UCHAR       *pApplicationData,
      BIT_UINT32      *pFeatureList, 
      BIT_UINT32      nFeatureInList, 
      BIT_UINT32      nDurationDays);

   BIT_STATUS BIT_CALLCONV Bit_CheckIn(
      BIT_PCSTR       szURL, 
      BIT_UINT32      featureId,
      BIT_UCHAR       *pApplicationData);
         
   BIT_STATUS BIT_CALLCONV Bit_TestBitService(
      BIT_PCSTR       szURL, 
      BIT_PCSTR       szSN, 
      BIT_UINT32      featureId,
      BIT_UCHAR       *pApplicationData);

   // 以上部分不要修改
#ifdef __cplusplus
} // extern "C"
#endif

class CBitAnswer
{
private:
   BIT_HANDLE m_handle;
   BIT_UINT32 m_ref;
   BIT_PCSTR  m_sn;
   LOGIN_MODE m_mode;

public:
   CBitAnswer(void);
   ~CBitAnswer(void);

   /**
   * 进行授权登陆，获取操作句柄。必须在除升级函数之外的其它操作前执行。根据登录模式的不同可能需要连接授权服务器。
   *   成功调用一次Login后，对应要调用一次Logout
   *   如果多次调用Login，没有调用Logout，除了第一次Login之外，其他的Login不会真正的连接授权文件或服务器
   *   对于同一台机器，当登陆到云授权时，最大允许10个进程处于Login状态
   * 参数：
   *   szSN - [IN] 授权码（SN），由比特平台产生。无论是单机授权还是集团授权，当SN不为NULL时，都表示只登陆这个授权码
   *           当SN为空时，根据配置文件会按照以下顺序查找：
   *               先查本地单机授权码，
   *               然后查找云授权码（如果本机记住云授权码）
   *               最后查找集团授权码，如果集团服务中存在多个授权码，均可用，服务会根据使用情况自动分配用户数
   *   mode - [IN] 登陆模式。 包含以下类型：
   *      BIT_MODE_LOCAL  // 登陆本地授权，即单机离线授权码、浮动授权码和演示授权码
   *      BIT_MODE_REMOTE // 登陆授权，如果授权码失效，会自动连接服务器，并返回连接服务器的错误码
   *      BIT_MODE_AUTO   // 登陆服务器，并自动升级授权
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	Login(BIT_PCSTR szSN, LOGIN_MODE mode);

   /**
   * 指定特征项进行授权登录，不连接授权服务器
   * 
   * 参数：
   *   szSN - [IN] 同Login
   *   featureId - [IN] 特征项ID
   *   mode - [IN] 同Login
   */
   BIT_STATUS LoginEx(BIT_PCSTR szSN, BIT_UINT32 featureId, LOGIN_MODE mode);

   /**
   * 此函数用于释放上下文句柄，退出登录状态。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	Logout();

   /**
   * 此函数用于读取特征项的数据内容，可用于“只读”和“读写”特征类型。
   * 参数：
   *   featureId - [IN] 产品的特征项ID。
   *   pFeatureValue - [OUT] 特征项数据缓存区地址，用于存储读出的数据。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	ReadFeature(BIT_UINT32 featureId, BIT_UINT32 *pFeatureValue);

   /**
   * 此函数用于更新“读写”类型的特征项的数据内容。
   * 参数：
   *   featureId - [IN] 产品的特征项ID。
   *   featureValue - [IN] 特征项数据值。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	WriteFeature(BIT_UINT32 featureId, BIT_UINT32 featureValue); 

   /**
   * 此函数用于使用“算法”类型的特征项对输入参数进行变换操作，得到唯一对应的4字节结果。
   * 参数：
   *   featureId - [IN] 产品的特征项ID。
   *   para1 - [IN] 变换输入因子1
   *   para2 - [IN] 变换输入因子2
   *   para3 - [IN] 变换输入因子3
   *   para4 - [IN] 变换输入因子4
   *   pResult - [OUT] 变换结果。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	ConvertFeature(BIT_UINT32 featureId, BIT_UINT32 para1, BIT_UINT32 para2, BIT_UINT32 para3, BIT_UINT32 para4, BIT_UINT32 * pResult);

   /**
   * 此函数用于使用“密钥”类型的特征项对输入的明文进行加密，返回密文结果。
   * 参数：
   *   featureId - [IN] 产品的特征项ID。
   *   pPlainBuffer - [IN] 数据缓存区地址，用于存储明文数据。
   *   pCipherBuffer - [OUT] 数据缓存区地址，用于存储密文数据。
   *   bufferSize - [IN] 数据长度，最大长度为256字节。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS EncryptFeature(BIT_UINT32 featureId, BIT_UCHAR * pPlainBuffer, BIT_UCHAR * pCipherBuffer, BIT_UINT32 bufferSize); 

   /**
   * 此函数用于使用“密钥”类型的特征项对输入的密文进行解密，返回明文结果。
   * 参数：
   *   featureId - [IN] 产品的特征项ID。
   *   pCipherBuffer - [IN] 数据缓存区地址，用于存储密文数据。
   *   pPlainBuffer - [OUT] 数据缓存区地址，用于存储明文数据。
   *   bufferSize - [IN] 数据长度，最大长度为256字节。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS DecryptFeature(BIT_UINT32 featureId, BIT_UCHAR * pCipherBuffer, BIT_UCHAR * pPlainBuffer, BIT_UINT32 bufferSize); 

   /**
   * 获取“用户”类型特征项的用户数，并返回剩余容量。如没有可分配用户数，则返回1810错误。
   * 参数：
   *   featureId - [IN] 产品的特征项ID。
   *   pCapacity - [OUT] 特征项剩余容量。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	QueryFeature(BIT_UINT32 featureId, BIT_UINT32 *pCapacity); 

   /**
   * 释放“用户”类型特征项的用户数，并返回剩余容量。
   * 参数：
   *   featureId - [IN] 产品的特征项ID。
   *   pCapacity - [OUT] 特征项剩余容量。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	ReleaseFeature(BIT_UINT32 featureId, BIT_UINT32 *pCapacity); 

   /**
   * 创建或更新配置项。如果相同名称的配置项存在，则会更新其中的数据；否则将添加新的授权码配置项。
   * 参数：
   *   szDataItemName - [IN] 数据项名称，名称最大长度为128字节。
   *   pDataItemValue - [IN] 数据项数据缓存区地址，数据最大长度为1024字节。
   *   dataItemValueSize - [IN] 数据项长度。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	SetDataItem(BIT_PCSTR szDataItemName, BIT_PCSTR szDataValue); 

   /**
   * 删除指定的配置项。该操作无法删除在比特平台上设置的产品配置项或模版配置项。
   * 参数：
   *   szDataItemName - [IN] 数据项名称，名称最大长度为128字节。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	RemoveDataItem(BIT_PCSTR szDataItemName); 

   /**
   * 读取指定的配置项数据。
   * 参数：
   *   szDataItemName - [IN] 数据项名称，名称最大长度为128字节。
   *   dataValue - [OUT] 数据项数据缓存区地址，数据最大长度为1024字节。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	GetDataItem(BIT_PCSTR szDataItemName, string& dataValue); 

   /**
   * 此函数用于获取可访问配置项的数量，一般用于配置项的枚举操作。
   * 参数：
   *   pNumber - [OUT] 可访问的数据项总数，包括了所有可以访问到的产品、模版及授权码的数据项。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	GetDataItemNum(BIT_UINT32 *pNumber); 

   /**
   * 根据配置项索引获取其名称，一般用于配置项的枚举操作。
   * 参数：
   *   index - [IN] 数据项索引。
   *   dataName - [OUT] 用于存储数据项名称的存储区地址。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	GetDataItemName(BIT_UINT32 index, string &dataName); 

   /**
   * 获取当前会话信息。根据获取的内容不同，此函数返回的数据可能是XML格式或非XML格式。
   * 参数：
   *   type - [IN] 获取会话信息类型。
   *   sessionInfo - [OUT] 返回的结果存放位置。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	GetSessionInfo(SESSION_TYPE type, string &sessionInfo); 

   /**
   * 获取当前运行环境的升级请求码，用于发起本地授权激活及升级请求。
   * 参数：
   *   szSN - [IN] 授权码（SN），由比特平台产生，不能为空，必须输入
   *   type - [IN] 本地授权绑定类型。 包含以下类型：
   *      BINDING_EXISTING     // 使用当前绑定类型，仅适用于授权码已经存在于本机或USB存储器
   *      BINDING_LOCAL        // 使用本机指纹进行绑定
   *      BINDING_USB_STORAGE  // 使用USB存储器进行绑定
   *   requestInfo - [OUT] 用于存储请求码的存储区地址。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	GetRequestInfo(BIT_PCSTR szSN, BINDING_TYPE type, string& requestInfo); 

   /**
   * 使用请求码与授权服务器进行连接，获取升级码。本函数需要进行网络连接。
   * 参数：
   *   szSN - [IN] 授权码（SN），由比特平台产生。
   *   pRequestInfo - [IN] 由获取请求码函数得到的请求码。
   *   updateInfo - [OUT] 由授权服务器获得的本地授权升级码。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	GetUpdateInfo(BIT_PCSTR szSN, BIT_PCSTR requestInfo, string &updateInfo); 

   /**
   * 应用升级码完成本地授权激活或升级。本函数必须在获取请求码的同一环境下执行。
   * 参数：
   *   pUpdateInfo - [IN] 由授权服务器获得的本地授权升级码。
   *   receiptInfo - [OUT] 确认码，记录升级状态，可由比特平台进行解析。为NULL表示不返回确认码
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	ApplyUpdateInfo(BIT_PCSTR pUpdateInfo, string& receiptInfo); 

   /**
   * 此函数用于与授权服务器在线连接，自动完成本地授权的升级操作。本函数需要进行网络连接。
   * 参数：
   *   szSN - [IN] 授权码（SN），为空表示升级第一个可用的授权码，但是如果授权码是迁出状态，不能为空
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS	UpdateOnline(BIT_PCSTR szSN); 

   /**
   * 此函数用于完成授权的在线迁出，迁出以后，其他机器就可以用了。本函数需要进行网络连接。
   * 参数：
   *   szSN - [IN] 授权码（SN），为空表示迁出第一个可用的授权码。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS  RevokeOnline(BIT_PCSTR szSN);

   /**
   * 此函数用于与产生授权的离线迁出请求码。本函数可能需要进行网络连接。
   * 参数：
   *   szSN - [IN] 授权码（SN），不能为空
   *   revokeInfo - [OUT] 存放迁出请求码的存储区地址
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS  Revoke(BIT_PCSTR szSN, string& revokeInfo);

   /**
   * 此函数用于获取授权码或集团服务等的相关信息，调用前不需要登录。
   * 参数：
   *   szSerialNum - [IN] 授权码
   *   type - [IN] 获取信息类型。 
   *   info - [OUT] 存放信息结果的存储区地址
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS GetInfo(BIT_PCSTR szSN, BIT_INFO_TYPE type, string& info);

   /**
   * 此函数用于设置集团服务的地址和端口。
   * 参数：
   *   szHostName - [IN] 集团服务器的地址，可以为域名或IP
   *   nPort - [IN] 集团服务器的端口
   *   nTimeoutSeconds - [IN] 连接服务器超时时间，单位秒
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS SetLocalServer(BIT_PCSTR szHostName, BIT_UINT32 nPort, BIT_UINT32 nTimeoutSeconds);

   /**
   * 此函数用于设置代理服务的地址和端口。
   * 参数：
   *   szHostName - [IN] 代理服务器的地址，可以为域名或IP
   *   nPort - [IN] 代理服务器的端口
   *   szUserID - [IN] 连接代理服务器的用户名
   *   szPassword - [IN] 连接代理服务器的密码
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS SetProxy(BIT_PCSTR szHostName, BIT_UINT32 nPort, BIT_PCSTR szUserID, BIT_PCSTR szPassword);

   /**
   * 此函数用于获取授权存储目录
   * 参数：
   *   path - [OUT] 产品的存储路径
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS GetProductPath(string &path);

   /**
   * 此函数用于删除本地授权。
   * 参数：
   *   szSN - [IN] 授权码
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS RemoveSn(BIT_PCSTR szSN); 
   
   /**
   * 获取授权信息及使用环境信息，数据以XML格式返回。调用此函数前客户端不需要执行登录操作。
   * 参数：
   *   szSN - [IN] 授权码（SN）字符串
   *   type - [IN] 获取信息类型。 包含以下类型：
   *     BIT_INFO_SERVER_ADDRESS // 获取局域网内的集团授权服务器地址列表
   *     BIT_INFO_SN // 获取本机已激活的授权码列表
   *     BIT_INFO_SN_FEATURE // 获取本机已激活授权码的特征项列表
   *     BIT_INFO_SN_LICENSE // 获取本机已激活授权码的授权信息
   *     BIT_INFO_UPDATE_ERROR // 获取指定授权码的升级错误详细信息。是否存在详细信息由授权码类型及错误类型决定
   * pInfo - [OUT] 用于存储返回XML信息数据的存储区地址。
   * pInfoSize - [IN/OUT] 输入的存储区长度。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS GetInfo(BIT_PCSTR szSN, BIT_INFO_TYPE type, BIT_CHAR *pInfo, BIT_UINT32 *pInfoSize);

   /** 
   * 获取客户端安全库版本号。
   * 参数：
   *   pVersion - [OUT] 客户端安全库版本号。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS GetVersion(BIT_UINT32 *pVersion);
   
   /**
   * 从集团授权服务器借出一个完整的授权码，以允许客户端脱离集团服务器单独使用。
   *   被借出的集团授权码必须具有可借出属性，并在客户端成功借出后减少一个可用用户数。
   *   被借出的用户数在到期后将自动返还给集团服务器。
   * 参数：
   *   szURL - [IN] 集团授权服务器地址，包括端口。如输入NULL，则使用配置文件地址；如输入 * 号，则使用广播查找地址。
   *   featureId - [IN] 指定借出授权码需要包含的特征项ID，为 0 则寻找第一个可借出授权码。
   *   nDurationDays - [IN] 借出时间，单位为天。借出时间不能超过被借出集团授权的强制认证周期或有效期。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS CheckOutSn(BIT_PCSTR szURL, BIT_UINT32 featureId, BIT_UINT32 nDurationDays);

   /**
   * 从集团授权服务器借出一组特征项，这些特征项必须包含在同一个授权码中。
   *   被借出的集团授权码必须具有可借出属性，并在客户端成功借出后减少一个可用用户数。
   *   被借出的用户数在到期后将自动返还给集团服务器。
   * 参数：
   *   szURL - [IN] 集团授权服务器地址，包括端口。如输入NULL，则使用配置文件地址；如输入 * 号，则使用广播查找地址。
   *   pFeatureList - [IN] 要借出的特征项列表。
   *   nFeatureInList - [IN] 要借出的特征项数量。
   *   nDurationDays - [IN] 借出时间，单位为天。借出时间不能超过被借出集团授权的强制认证周期或有效期。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
  */
   BIT_STATUS CheckOutFeatures(BIT_PCSTR szURL, BIT_UINT32 *pFeatureList, BIT_UINT32 nFeatureInList, BIT_UINT32 nDurationDays);

   /**
   * 提前返还从集团授权服务器借出的授权。要提前返还授权，该授权码必须具有允许提前返还属性。
   * 参数：
   *   szURL - [IN] 集团授权服务器地址，包括端口。如输入NULL，则使用配置文件地址；如输入 * 号，则使用广播查找地址。
   *   featureId - [IN] 指定要返还授权码需要包含的特征项ID，为 0 则寻找第一个可返还授权码。
   * 返回：
   *   如果返回值为零，表示此函数调用成功。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS CheckIn(BIT_PCSTR szURL, BIT_UINT32 featureId);

   /**
   * 测试集团授权和特征项是否可用，不会占用授权码或特征项的用户数。
   * 参数：
   *   szURL - [IN] 集团授权服务器地址，包括端口。
   *   szSN - [IN] 授权码（SN），为空表示不指定授权码。
   *   featureId - [IN] 指定要测试的特征项，为 0 则不检查特征项。
   * 返回：
   *   如果返回值为零，表示指定的授权码和特征项可用。如果非零，可以根据返回值，从错误码表中查看具体的错误原因。
   */
   BIT_STATUS TestBitService(BIT_PCSTR szURL, BIT_PCSTR szSN, BIT_UINT32 featureId);
};
