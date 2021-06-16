#pragma once
#include <iostream>
#include <windows.h>
#include <WinUser.h>
#include <WinBase.h>
#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"wininet.lib")
#pragma comment(lib, "Advapi32.lib")
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <WinInet.h>
#include <iomanip>
#include <cstdio>

namespace hostutils {
	std::string get_hwid();
	std::string get_username(bool User);
	std::string string_hex(const std::string input);
	DWORD volume_id();
	std::string get_serial_key();
	std::string get_hash_serial_key();
	std::string get_serial();
	std::string base64_encode(char const* bytes_to_encode, unsigned int in_len);
	std::string get_serial_64();
	std::string get_url_data(std::string url);
	std::string get_url_data_scpt(std::string url);
	bool check_actuality_license();
	std::string get_days_user();
	std::string get_version_this_dll();
	std::string detect();
	std::string check_files();
}