#define BACKTRACE_BUFFER_SIZE 32

#ifdef OS_LINUX
#	define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>

#include <macros.h>
#include <stacktrace.h>

#ifdef OS_WINDOWS
#	include <windows.h>
#	include <dbghelp.h>
#endif

#ifdef OS_LINUX
#   include <execinfo.h>
#endif

#ifdef OS_WINDOWS
typedef BOOL(*SYMINITIALIZEPROC)(HANDLE, PCSTR, BOOL);
typedef PVOID(*SYMFUNCTIONTABLEACCESS64PROC)(HANDLE, DWORD64);
typedef DWORD64(*SYMGETMODULEBASE64PROC)(HANDLE, DWORD64);
typedef BOOL(*SYMGETMODULEINFO64PROC)(HANDLE, DWORD64, PIMAGEHLP_MODULE64);
typedef BOOL(*STACKWALK64PROC)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID, PREAD_PROCESS_MEMORY_ROUTINE64, PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64);
typedef BOOL(*SYMFROMADDRPROC)(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO);
typedef BOOL(*SYMCLEANUPPROC)(HANDLE);

static SYMINITIALIZEPROC s_pSymInitialize;
static SYMFUNCTIONTABLEACCESS64PROC s_pSymFunctionTableAccess64;
static SYMGETMODULEBASE64PROC s_pSymGetModuleBase64;
static SYMGETMODULEINFO64PROC s_pSymGetModuleInfo64;
static STACKWALK64PROC s_pStackWalk64;
static SYMFROMADDRPROC s_pSymFromAddr;
static SYMCLEANUPPROC s_pSymCleanup;

static HMODULE s_hDbgHelp;
static PVOID s_pGlobalExceptionHandler;

LONG ExceptionHandler(EXCEPTION_POINTERS* pxExceptionInfo) {
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();
	
	s_pSymInitialize(hProcess, 0, TRUE);

	CONTEXT* pContext = pxExceptionInfo->ContextRecord;

	DWORD64 lExceptionAddress = (DWORD64)pxExceptionInfo->ExceptionRecord->ExceptionAddress;
	DWORD64 lModuleBaseAddress = (DWORD64)GetModuleHandle(0);

	if ((lExceptionAddress >= lModuleBaseAddress) && (lExceptionAddress < (lModuleBaseAddress + 0x1000000))) {
		STACKFRAME64 xStackFrame;
		memset(&xStackFrame, 0, sizeof(xStackFrame));
		xStackFrame.AddrPC.Offset = pContext->Rip;
		xStackFrame.AddrPC.Mode = AddrModeFlat;
		xStackFrame.AddrFrame.Offset = pContext->Rsp;
		xStackFrame.AddrFrame.Mode = AddrModeFlat;
		xStackFrame.AddrStack.Offset = pContext->Rsp;
		xStackFrame.AddrStack.Mode = AddrModeFlat;

		printf("\n");
		printf("<<<<<<<<<<<< STACK TRACE >>>>>>>>>>>>\n");
		printf("\n");

		UINT nBacktraceCount = 0;

		while ((s_pStackWalk64(IMAGE_FILE_MACHINE_AMD64, hProcess, hThread, &xStackFrame, pContext, 0, s_pSymFunctionTableAccess64, s_pSymGetModuleBase64, 0)) && (nBacktraceCount < BACKTRACE_BUFFER_SIZE)) {
			DWORD64 lAddress = xStackFrame.AddrPC.Offset;
			DWORD64 lDisplacement;

			char acSymbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(CHAR)];

			PSYMBOL_INFO pxSymbol = (PSYMBOL_INFO)acSymbolBuffer;

			pxSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			pxSymbol->MaxNameLen = MAX_SYM_NAME;

			if (s_pSymFromAddr(hProcess, lAddress, &lDisplacement, pxSymbol)) {
				printf("%s(+0x%I64x) [0x%I64x]\n", pxSymbol->Name, lDisplacement, lAddress);
			} else {
				IMAGEHLP_MODULE64 xModuleInfo;

				memset(&xModuleInfo, 0, sizeof(xModuleInfo));
	            xModuleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

	            if (s_pSymGetModuleInfo64(hProcess, lAddress, &xModuleInfo)) {
	                printf("%s(+0x%I64x) [0x%I64x]\n", xModuleInfo.ModuleName, lDisplacement, lAddress);
	            } else {
	                printf("unknown_function(+0x%I64x) [0x%I64x]\n", 0ULL, lAddress);
	            }
			}

			nBacktraceCount++;
		}

		printf("\n");

		s_pSymCleanup(hProcess);

		ExitProcess(EXIT_FAILURE);

		return EXCEPTION_EXECUTE_HANDLER;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}
#endif

#ifdef OS_LINUX
static struct sigaction s_xNewAction;
static struct sigaction s_xOldAction;

static void ExceptionHandler(int32_t nSignal) {
	UNUSED(nSignal);

	void* apBacktraceBuffer[BACKTRACE_BUFFER_SIZE];
	int32_t nBacktraceSize;
	char** ppSymbols;

	nBacktraceSize = backtrace(apBacktraceBuffer, BACKTRACE_BUFFER_SIZE);
	ppSymbols = backtrace_symbols(apBacktraceBuffer, nBacktraceSize);

	printf("\n");
	printf("<<<<<<<<<<<< STACK TRACE >>>>>>>>>>>>\n");
	printf("\n");

	for (int32_t i = 0; i < nBacktraceSize; ++i) {
		printf("%s\n", ppSymbols[i]);
	}

	printf("\n");

	exit(EXIT_FAILURE);
}
#endif

void StackTrace_Alloc(void) {
#ifdef OS_WINDOWS
	s_hDbgHelp = LoadLibrary("dbghelp.dll");

	s_pSymInitialize = (SYMINITIALIZEPROC)GetProcAddress(s_hDbgHelp, "SymInitialize");
	s_pSymFunctionTableAccess64 = (SYMFUNCTIONTABLEACCESS64PROC)GetProcAddress(s_hDbgHelp, "SymFunctionTableAccess64");
	s_pSymGetModuleBase64 = (SYMGETMODULEBASE64PROC)GetProcAddress(s_hDbgHelp, "SymGetModuleBase64");
	s_pSymGetModuleInfo64 = (SYMGETMODULEINFO64PROC)GetProcAddress(s_hDbgHelp, "SymGetModuleInfo64");
	s_pStackWalk64 = (STACKWALK64PROC)GetProcAddress(s_hDbgHelp, "StackWalk64");
	s_pSymFromAddr = (SYMFROMADDRPROC)GetProcAddress(s_hDbgHelp, "SymFromAddr");
	s_pSymCleanup = (SYMCLEANUPPROC)GetProcAddress(s_hDbgHelp, "SymCleanup");

	s_pGlobalExceptionHandler = AddVectoredExceptionHandler(1, ExceptionHandler);
#endif

#ifdef OS_LINUX
	s_xNewAction.sa_handler = ExceptionHandler;

	sigemptyset(&s_xNewAction.sa_mask);

	s_xNewAction.sa_flags = 0;

	sigaction(SIGSEGV, &s_xNewAction, &s_xOldAction);
#endif
}

void StackTrace_Free(void) {
#ifdef OS_WINDOWS
	RemoveVectoredExceptionHandler(s_pGlobalExceptionHandler);

	FreeLibrary(s_hDbgHelp);
#endif
}
