#ifndef CORE_H
#define CORE_H
#endif

#include <string>
#include <cstdint>

// GLOBAL CONSTANTS
const char FRAME_DELIMITER[] = "\r\n\r\n";

// Handles variable string encodings
namespace Convert
{
	std::string utf16ToStd(const char*);
	std::wstring widen(std::string&);
	void writeUtf16(std::string&);
}

// Preserve byte-order mark
namespace BOM
{
	void remove(std::wstring&);
	void attach(std::wofstream&);
}

