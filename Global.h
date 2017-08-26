#include <Tlhelp32.h>

void AdjustProcessPrivilege(HANDLE hToken, LPCSTR szPrivName)
{
	TOKEN_PRIVILEGES tkp;
    LookupPrivilegeValue(NULL, szPrivName, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL);
}