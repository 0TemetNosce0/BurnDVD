// ---------------------------------------------------------------------------

#ifndef DllInterfaceH
#define DllInterfaceH
// ---------------------------------------------------------------------------
#include <windows.h>
#include <assert.h>
#include <stdio.h>

#ifndef _DEBUG
#define NDEBUG
#endif

#if defined(UNICODE) || defined (_UNICODE)
#define _LoadLibrary LoadLibraryW
#define _sprintf swprintf
#else //_UNICODE
#define _LoadLibrary LoadLibrary
#define _sprintf sprintf
#endif //_UNICODE

class TDllInterface {
private:
	HINSTANCE HInstance;

public:
	TDllInterface() {
		HInstance = 0;
	}

	~TDllInterface() {
		FreeDll();
	}

	bool LoadDll(const TCHAR* dllPath) {
		FreeDll();
		HInstance = _LoadLibrary(dllPath);
		if (HInstance) {
			return true;
		}
		else {
			DWORD dwMessageID = ::GetLastError();
			TCHAR *szMessage = NULL;
			TCHAR szTitle[32];
			memset(szTitle, 0, sizeof(TCHAR) * 32);

			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwMessageID, 0,
				 (TCHAR*)&szMessage, 0, NULL);
			_sprintf(szTitle, L"Error(%d)", dwMessageID);
			::MessageBox(NULL, szMessage, szTitle, MB_OK);
		}
		return false;
	}

	void FreeDll() {
		if (HInstance) {
			FreeLibrary(HInstance);
			HInstance = 0;
		}
	}

	template<class T>
	T GetFunction(const char* functionName) {
		assert(HInstance != 0);

		T fun = reinterpret_cast<T>(GetProcAddress(HInstance, functionName));
		if (fun == 0)
			throw functionName;
		return fun;
	}
};

#endif
