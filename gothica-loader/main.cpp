#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include <fstream>

#include "options.hpp"
#include "vmp/vmp.h"
#include "xorstr.h"
#include "cheat.h"

#define xorstr XorStr

namespace license {
	static bool run = hostutils::check_actuality_license();
}

static void show_progress_bar(int time, const std::string& message, char symbol)
{
	std::string progress_bar;
	const double progress_level = 1.42;

	std::cout << message << "\n\n";

	for (double percentage = 0; percentage <= 100; percentage += progress_level)
	{
		progress_bar.insert(0, 1, symbol);
		std::cout << "\r [" << std::ceil(percentage) << '%' << "] " << progress_bar;
		std::this_thread::sleep_for(std::chrono::milliseconds(time));
	}
}

static void clipboard_hwidden(const char* text)
{
	if (OpenClipboard(0)) {
		EmptyClipboard();
		char* clip_data = (char*)(GlobalAlloc(GMEM_FIXED, MAX_PATH));
		lstrcpy(clip_data, text);
		SetClipboardData(CF_TEXT, (HANDLE)(clip_data));
		LCID* lcid = (DWORD*)(GlobalAlloc(GMEM_FIXED, sizeof(DWORD)));
		*lcid = MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_NEUTRAL), SORT_DEFAULT);
		SetClipboardData(CF_LOCALE, (HANDLE)(lcid));
		CloseClipboard();
	}
}

static void logotype() {
	std::cout << R"(
                 __  .__    .__               
    ____   _____/  |_|  |__ |__| ____ _____   
   / ___\ /  _ \   __\  |  \|  |/ ___\\__  \  
  / /_/  >  <_> )  | |   Y  \  \  \___ / __ \_
  \___  / \____/|__| |___|  /__|\___  >____  /
 /_____/                  \/        \/     \/ 
 
)";

}

int main() {

	VMProtectBeginUltra(xorstr("Main"));
	VMProtectIsDebuggerPresent(true);
	VMProtectIsVirtualMachinePresent();
	VMProtectIsValidImageCRC();

	setlocale(LC_ALL, xorstr("Russian"));

	Sleep(250);
	show_progress_bar(100, "", '*');
	system("cls");

	logotype();

	Sleep(250);

	if (license::run) {
		std::cout << "" << std::endl;

		std::cout << xorstr(" [+] sucess, license is a finded ") << std::endl;
		std::cout << xorstr(" [!] run counter-strike before opening the loader ") << std::endl;

		// unfinished 
	}
	else {
		std::cout << "" << std::endl;

		std::cout << xorstr(" [!] you dont have license") << std::endl;

		std::cout << xorstr(" [?]: eng HWID copied.") << std::endl; clipboard_hwidden(hostutils::get_serial().c_str());
		std::cout << xorstr(" [?]: ru HWID скопирован.") << std::endl; clipboard_hwidden(hostutils::get_serial().c_str());


	}

	VMProtectEnd();
}