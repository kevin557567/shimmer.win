#ifndef _APPMGR_H_
#define _APPMGR_H_

#include <windows.h>
#include <string>

class UGToken{
public:
  static UGToken* shared();
  HANDLE createAdminToken();

private:
  bool getExplorerIdFromCurrentSessionId(int iCurSessionId, unsigned int& retPid);
  bool getTokenInfo(HANDLE hToken, TOKEN_INFORMATION_CLASS Tic, PVOID *pTokenInfoOut, unsigned int* pdwOutSize);
  bool getTokenFromPid(unsigned int uiPid, _TOKEN_TYPE TokenType, HANDLE& hToken);
  bool getLasPid(unsigned int& retPid);
  bool enableTokenPrivilege(HANDLE hToken, LPCTSTR szPrivName, bool bEnable);
  PVOID getFromToken(HANDLE hToken, TOKEN_INFORMATION_CLASS tic);
  HANDLE createUserToken(HANDLE hBasicToken, PLUID pUsersession);

private:
  UGToken();
  ~UGToken() {}
  typedef DWORD(WINAPI  *LoadWTSGetActiveConsoleSessionId)(VOID);
  LoadWTSGetActiveConsoleSessionId fWTSGetActiveConsoleSessionId;
  unsigned int m_uiLsaId;
};

bool createAdminProcess(
  const std::string& sPath, 
  const std::string& sCmdLine, 
  bool show = true, 
  HANDLE out = nullptr,
  HANDLE error = nullptr,
  PROCESS_INFORMATION *ppi = nullptr);

DWORD _stdcall LaunchAppIntoDifferentSession(LPTSTR lpCommand);

#endif //_APPMGR_H_