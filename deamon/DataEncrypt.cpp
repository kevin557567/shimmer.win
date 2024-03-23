// DataEncrypt.cpp: implementation of the CDataEncrypt class.
//
//////////////////////////////////////////////////////////////////////
#include "DataEncrypt.h"
#include "algorighm/base64.h"
#include "algorighm/sha2.h"
#include "algorighm/EfAes.H"
#include "algorighm/md5checksum.h"
#include <time.h>


//using namespace SystemCommon::StringHelper;

#define MASTER_KEY		  _T("8DE6E635-D3C3-41e1-9A76-4BAE64E58695")
#define MASTER_KEY_A		"8DE6E635-D3C3-41e1-9A76-4BAE64E58695"
#define PASSWORD_SECU_KEY _T("(4C471108D766)")
#define PASSWORD_LOCK_KEY "(4B01F200ED01)"


unsigned char input_vector[16] = 
{
0xff, 0xff, 0xFf, 0xff, 0xff, 0xff, 0xFf, 0xff,
0xff, 0xff, 0xFf, 0xff, 0xff, 0xff, 0xFf, 0xff
};
namespace EN
{
	struct CSetting
	{
		CSetting()
		{
			_bUseProtectFunc = false;
		}
		static CSetting* GetInstance()
		{
			static CSetting setting;
			return &setting;
		};

		bool _bUseProtectFunc;
	};

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataEncrypt::CDataEncrypt()
{

}

CDataEncrypt::~CDataEncrypt()
{

}

//数据是否加密
BOOL CDataEncrypt::IsEncrypted(LPCTSTR lpszData)
{
	BOOL bRes = FALSE;

	return bRes;
}


std::string CDataEncrypt::EncryptKeyA(LPCSTR lpszKey)
{
	if (NULL == lpszKey)
		return "";
	//与master key进行合并
	std::string strMainKey = lpszKey;
	strMainKey += MASTER_KEY_A;
	
	//sha256处理
	unsigned char szSha256Data[33] = {0};
	sha256(szSha256Data, (unsigned char*)strMainKey.c_str(), strMainKey.size());
	szSha256Data[32] = '\0';
	
	//进行MD5处理
	return CMD5Checksum::GetMD5(szSha256Data, 32);
}

std::string CDataEncrypt::EncryptA(LPCSTR lpszData, LPCSTR lpszKey)
{
	// 判断字符串是否为空，如果为空，返回空字符串
	if(NULL == lpszData || NULL == lpszData[0])
	{
		return "";
	}
	// 对原始数据进行随机数转换
	std::string strRandData = SimpleEncode(lpszData).c_str();
	// 对Key进行加密
	std::string strKey = EncryptKeyA(lpszKey);
//	std::string strRandData = lpszData;

	// 计算长度
	int nSize = strRandData.size();
	int nAllocSize = nSize + AES_PADDING;
	//AES256加密
	AesCtx context;
	AesSetKey(&context,BLOCKMODE_ECB, (void*)strKey.c_str(), input_vector);		
	// char szAesData[1024] = {0};
	char* lpBuffer = new char[nAllocSize];
	AesEncryptECB(&context, lpBuffer, (void*)strRandData.c_str(), nSize);
	
	//base64编码(字符串)
	int nRealSize = AesRoundSize(nSize, 16);
	char* pEncodeStr = NULL;//Util::BASE64::base64_encode(lpBuffer, nRealSize);
	delete [] lpBuffer;

	//int rdsz = AesRoundSize(strRandData.length() , 16);
	// char* pEncodeStr = Util::BASE64::base64_encode(szAesData);

	strRandData = (pEncodeStr == NULL) ? "" : pEncodeStr;
	if(pEncodeStr != NULL)
		free(pEncodeStr);
	// 需要添加头部
	strRandData = PASSWORD_LOCK_KEY + strRandData;
	return strRandData;
}

int CDataEncrypt::AesRoundSize( int iSize, int iAlignSize )
{
	int iMul = (iSize/iAlignSize) * iAlignSize;
	
	if(iSize != iMul)
	{
		iSize = iMul + iAlignSize;
	}
	
	return  iSize;
}


std::string CDataEncrypt::SimpleEncode(const char* szPlain)
{
	if(EN::CSetting::GetInstance()->_bUseProtectFunc)//是否保护函数
		return P_SimpleEncode(szPlain).c_str();
	else
		return N_SimpleEncode(szPlain).c_str();
}

#pragma optimize("", off)
std::string CDataEncrypt::P_SimpleEncode(const char* szPlain)
{
	if(szPlain == NULL)
		return "";
	
	//QP_SDK_FAST_VIRTUALIZE_BEGIN
		
		//加入随机数，在整个字符串的奇偶位加入随机数，用首位来判断具体是奇位还是偶位
		int nPlainLen = (int)strlen(szPlain);
	srand( (unsigned)time( NULL ) );
	char cPos = rand() % 2 + 1;
	int nDestSize = nPlainLen * 2 + 2;
	char* pDestBuf = new char[nDestSize];
	ZeroMemory(pDestBuf, nDestSize);
	
	char* pDestBufTemp = pDestBuf;
	memcpy(pDestBufTemp, &cPos, 1);
	pDestBufTemp += 1;
	for(int i = 0; i < nPlainLen; i++)
	{
		if(cPos == 1) //偶数位
		{
			//产生128以下的随机数
			char cRand = rand() % 100 + 1;
			memcpy(pDestBufTemp, &cRand, 1);
			pDestBufTemp += 1;
			
			memcpy(pDestBufTemp, szPlain + i, 1);
			pDestBufTemp += 1;
		}
		else if(cPos == 2) //奇数位
		{		
			memcpy(pDestBufTemp, szPlain + i, 1);
			pDestBufTemp += 1;
			
			//产生128以下的随机数
			char cRand = rand() % 100 + 1;
			memcpy(pDestBufTemp, &cRand, 1);
			pDestBufTemp += 1;		
		}		
	}
	
	std::string strEncode = pDestBuf;
	delete [] pDestBuf;
	
	//QP_SDK_FAST_VIRTUALIZE_END
		
	return strEncode.c_str();
}
#pragma optimize("", on)

std::string CDataEncrypt::N_SimpleEncode(const char* szPlain)
{
	if(szPlain == NULL)
		return "";
		
	//加入随机数，在整个字符串的奇偶位加入随机数，用首位来判断具体是奇位还是偶位
	int nPlainLen = (int)strlen(szPlain);
	srand( (unsigned)time( NULL ) );
	char cPos = rand() % 2 + 1;
	int nDestSize = nPlainLen * 2 + 2;
	char* pDestBuf = new char[nDestSize];
	ZeroMemory(pDestBuf, nDestSize);
	
	char* pDestBufTemp = pDestBuf;
	memcpy(pDestBufTemp, &cPos, 1);
	pDestBufTemp += 1;
	for(int i = 0; i < nPlainLen; i++)
	{
		if(cPos == 1) //偶数位
		{
			//产生128以下的随机数
			char cRand = rand() % 100 + 1;
			memcpy(pDestBufTemp, &cRand, 1);
			pDestBufTemp += 1;
			
			memcpy(pDestBufTemp, szPlain + i, 1);
			pDestBufTemp += 1;
		}
		else if(cPos == 2) //奇数位
		{		
			memcpy(pDestBufTemp, szPlain + i, 1);
			pDestBufTemp += 1;
			
			//产生128以下的随机数
			char cRand = rand() % 100 + 1;
			memcpy(pDestBufTemp, &cRand, 1);
			pDestBufTemp += 1;		
		}		
	}
	
	std::string strEncode = pDestBuf;
	delete [] pDestBuf;
	
	return strEncode.c_str();
}

std::string CDataEncrypt::SimpleDecode(const char* pEncode)
{
	if(EN::CSetting::GetInstance()->_bUseProtectFunc)
		return P_SimpleDecode(pEncode);
	else
		return N_SimpleDecode(pEncode);
}

#pragma optimize("", off)
std::string CDataEncrypt::P_SimpleDecode(const char* pEncode)
{
	if(pEncode == NULL)
		return "";
	
	//QP_SDK_FAST_VIRTUALIZE_BEGIN
		
	//去掉奇偶数位还原数据
	int nLen = strlen(pEncode);
	if(nLen < 1)
		return "";
	
	char* pDestBuf = new char[nLen + 1];
	char* pDestBufTemp = pDestBuf;
	ZeroMemory(pDestBuf, nLen + 1);
	
	char cPos = 0;
	memcpy(&cPos, pEncode, 1);
	
	for(int i = 1; i < nLen; i++)
	{
		if(cPos == 1)
		{
			if(i % 2 == 0)
			{
				memcpy(pDestBufTemp, pEncode + i, 1);
				pDestBufTemp += 1;
			}
		}
		else if(cPos == 2)
		{
			if(i % 2 != 0)
			{
				memcpy(pDestBufTemp, pEncode + i, 1);
				pDestBufTemp += 1;
			}
		}
	}
	
	std::string strDecode = pDestBuf;
	delete [] pDestBuf;
	
	//QP_SDK_FAST_VIRTUALIZE_END	
		
	return strDecode.c_str();
}
#pragma optimize("", on)

std::string CDataEncrypt::N_SimpleDecode(const char* pEncode)
{
	if(pEncode == NULL)
		return "";
		
	//去掉奇偶数位还原数据
	int nLen = strlen(pEncode);
	if(nLen < 1)
		return "";
	
	char* pDestBuf = new char[nLen + 1];
	char* pDestBufTemp = pDestBuf;
	ZeroMemory(pDestBuf, nLen + 1);
	
	char cPos = 0;
	memcpy(&cPos, pEncode, 1);
	
	for(int i = 1; i < nLen; i++)
	{
		if(cPos == 1)
		{
			if(i % 2 == 0)
			{
				memcpy(pDestBufTemp, pEncode + i, 1);
				pDestBufTemp += 1;
			}
		}
		else if(cPos == 2)
		{
			if(i % 2 != 0)
			{
				memcpy(pDestBufTemp, pEncode + i, 1);
				pDestBufTemp += 1;
			}
		}
	}
	
	std::string strDecode = pDestBuf;
	delete [] pDestBuf;
	
	return strDecode.c_str();
}
extern char* base64_encode(LPVOID*data,int dlen);
std::string CDataEncrypt::EncryptA_ECB( LPCSTR lpszData, LPCSTR lpszKey )
{
	// 判断字符串是否为空，如果为空，返回空字符串
	if(NULL == lpszData || NULL == lpszData[0])
	{
		return "";
	}
	// 对原始数据进行随机数转换
	std::string strData = lpszData;
  int old_len = strData.length();


	// 计算长度
//	int nSize = strData.size();
	//int nAllocSize = nSize + AES_PADDING;

  int mod = strData.size() % 16;
  int padding = 16 - mod;
  char *fill = (char*)alloca(padding + 1);
  memset(fill, padding, padding);
  fill[padding] = 0;

  strData += fill;
  int nSize = strData.size();
  int nAllocSize = nSize + 1;


	//AES256加密
	AesCtx context;
	AesSetKey(&context,BLOCKMODE_ECB, (void*)lpszKey, input_vector);		

	// char szAesData[1024] = {0};
	char* lpBuffer = new char[nAllocSize];
	AesEncryptECB(&context, lpBuffer, (void*)strData.c_str(), nSize);

	//base64编码(字符串)
	int nRealSize = AesRoundSize(nSize, 16);
	char* pEncodeStr = 	base64_encode((LPVOID*)lpBuffer, nRealSize);NULL;//Util::BASE64::base64_encode(lpBuffer, nRealSize);
	delete [] lpBuffer;

	strData = (pEncodeStr == NULL) ? "" : pEncodeStr;
	if(pEncodeStr != NULL)
		free(pEncodeStr);
	return strData;
}

std::string CDataEncrypt::DecryptA_ECB( LPCSTR lpszData, LPCSTR lpszKey )
{
	//返回的数据
	std::string strData = "";
	
	//判断字符串是否为空，如果为空，返回空字符串
	if(NULL == lpszData || NULL == lpszData[0])
	{
		return "";
	}

		

	//Base64解码，必须要得到解密后数据的长度！！！
	int nDataLenOut = 0;
	char* pDencodeStr = Util::BASE64::base64_decode(lpszData, &nDataLenOut);
	if(pDencodeStr != NULL)
	{
		int nSize = nDataLenOut;
		//AES256解密
		AesCtx context;
		AesSetKey(&context,BLOCKMODE_ECB, (void*)lpszKey, input_vector);				

		int nAllocSize = nSize + AES_PADDING;
		char* lpBuffer = new char[nAllocSize];
		ZeroMemory(lpBuffer, nAllocSize * sizeof(char));
		AesDecryptECB(&context, lpBuffer, (void*)pDencodeStr, nSize);
		free(pDencodeStr);

		//去掉随机数
		strData = lpBuffer;
		delete [] lpBuffer;
	}

  if (strData.size() > 0) {
    char last = strData.back();
    std::size_t found = strData.find_last_not_of(last);
    if (found != std::string::npos)
      strData.erase(found + 1);
  }
	return strData;

}

#define BASE64_PAD64 '='

char base64_alphabet[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a',
'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1',
'2', '3', '4', '5', '6', '7', '8', '9', '+',
'/'};

static char cmove_bits(unsigned char src, unsigned lnum, unsigned rnum) {
	src <<= lnum;
	src >>= rnum;
	return src;
}

char* base64_encode(LPVOID*lpbuffer,int dlen) {
	char *ret, *retpos;
	int m, padnum = 0, retsize;
	
	char* data = (char*)lpbuffer;
	if(dlen == 0) return NULL;
	
	/* Account the result buffer size and alloc the memory for it. */
	if((dlen % 3) != 0)
		padnum = 3 - dlen % 3;
	retsize = (dlen + padnum) + ((dlen + padnum) * 1/3) + 1;
	if((ret = (char*)malloc(retsize)) == NULL) 
		return NULL;
	retpos = ret;
	
	
	/* Starting to convert the originality characters to BASE64 chracaters.
	Converting process keep to 4->6 principle. */
	for(m = 0; m < (dlen + padnum); m += 3) {
		/* When data is not suffice 24 bits then pad 0 and the empty place pad '='. */
		*(retpos) = base64_alphabet[cmove_bits(*data, 0, 2)];
		if(m == dlen + padnum - 3 && padnum != 0) {  /* Whether the last bits-group suffice 24 bits. */
			if(padnum == 1) {   /* 16bit need pad one '='. */
				*(retpos + 1) = base64_alphabet[cmove_bits(*data, 6, 2) + cmove_bits(*(data + 1), 0, 4)];
				*(retpos + 2) = base64_alphabet[cmove_bits(*(data + 1), 4, 2)];
				*(retpos + 3) = BASE64_PAD64;
			} else if(padnum == 2) { /* 8bit need pad two'='. */
				*(retpos + 1) = base64_alphabet[cmove_bits(*data, 6, 2)];
				*(retpos + 2) = BASE64_PAD64;
				*(retpos + 3) = BASE64_PAD64;
			}
		} else {  /* 24bit normal. */
			*(retpos + 1) = base64_alphabet[cmove_bits(*data, 6, 2) + cmove_bits(*(data + 1), 0, 4)];
			*(retpos + 2) = base64_alphabet[cmove_bits(*(data + 1), 4, 2) + cmove_bits(*(data + 2), 0, 6)];
			*(retpos + 3) = base64_alphabet[*(data + 2) & 0x3f];
		}
		
		retpos += 4;
		data += 3;
	}
	
	ret[retsize - 1] =0;
	
	return ret;
}