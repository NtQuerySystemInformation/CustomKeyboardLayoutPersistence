#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

using pfnInstallLayoutOrTip = HRESULT(WINAPI*)(LPCWSTR psz, DWORD dwFlasg);

enum class LAYOUT_OPTION : uint32_t{
	LAYOUT_TEXT,
	LAYOUT_ID, 
	LAYOUT_DISPLAY_NAME,
	LAYOUT_FILE 
};

#define SetRegistryValue(hSubkey, Name, subkeyOption) \
	RegSetValueExW(hSubKey, Name , 0, REG_SZ, reinterpret_cast<const BYTE*>(pLayoutStruct->m_subkeyOptions[static_cast<uint32_t>(subkeyOption)].data()), \
	pLayoutStruct->m_subkeyOptions[static_cast<uint32_t>(subkeyOption)].size() * sizeof(wchar_t)) 

struct LayoutID
{
	std::uint32_t m_maxLayoutId;
	std::uint32_t m_firstAvailableLayout;
	std::vector<std::wstring> m_subkeyOptions;

public:
	LayoutID(std::initializer_list<std::wstring> lst = {}) : m_maxLayoutId{ 0 }, m_firstAvailableLayout{ 0 }
	{
		m_subkeyOptions.insert(m_subkeyOptions.begin(), lst);
	}
};

using PLAYOUTID = LayoutID*;
