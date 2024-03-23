// DataEncrypt.h: interface for the CDataEncrypt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAENCRYPT_H__98718E01_0BCF_4FBD_88D8_BC212207D22D__INCLUDED_)
#define AFX_DATAENCRYPT_H__98718E01_0BCF_4FBD_88D8_BC212207D22D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CDataEncrypt  
{
public:
	CDataEncrypt();
	virtual ~CDataEncrypt();

	BOOL IsEncrypted(LPCTSTR lpszData);

	std::string EncryptKeyA(LPCSTR lpszKey);
	std::string EncryptA(LPCSTR lpszData, LPCSTR lpszKey);
	

private:
	std::string SimpleEncode(const char* szPlain);
	std::string P_SimpleEncode(const char* szPlain);
	std::string N_SimpleEncode(const char* szPlain);
	std::string SimpleDecode(const char* pEncode);
	std::string P_SimpleDecode(const char* pEncode);
	std::string N_SimpleDecode(const char* pEncode);
	static int AesRoundSize( int iSize, int iAlignSize);

public://标准的AES ECB加密
	static std::string EncryptA_ECB(LPCSTR lpszData, LPCSTR lpszKey);
	static std::string DecryptA_ECB(LPCSTR lpszData, LPCSTR lpszKey);
};

#endif // !defined(AFX_DATAENCRYPT_H__98718E01_0BCF_4FBD_88D8_BC212207D22D__INCLUDED_)
