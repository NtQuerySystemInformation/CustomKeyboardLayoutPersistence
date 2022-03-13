#pragma once
#include "defs.h"

constexpr UINT KEY_CUSTOM_ACCESS = 0x2001F;

namespace Install
{
	auto InstallCustomLayout(LayoutID* playoutStruct) -> bool;
	auto GetMaxAvailableID(const HKEY& hKey, LayoutID* LayoutStruct) -> bool;
	auto Transform(PWCHAR orgSubKeyName) noexcept;
	bool CreateValuesSubkey(const HKEY& hKey, PLAYOUTID playoutStruct);
	bool SetRegValuesHKLM(const HKEY& hSubKey, LayoutID* pLayoutStruct);
	bool AddLayoutToLanguageBar(PLAYOUTID klid) noexcept;
	bool InstallLayoutOption2(PLAYOUTID klid) noexcept;
};

namespace Dropper {
	
	bool DropDllPayload(void) noexcept;
}