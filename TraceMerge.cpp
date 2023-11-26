/////////////////////////////////////////////////////////////////
/*
	::: Author:       | Joshua Newell Diehl
	::: VERSION:      | 0.1.0
	::: Last update:  | 11-25-2023 Establish core functionality
*/
/////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstring>
#include <fstream>
#include <codecvt>
#include <sstream>
#include <numeric>
#include <iterator>
#include <algorithm>

// Local declarations
#include "Core.h"
#include "Framer.h"

void BOM::remove(std::wstring& wstr)
{
	if (wstr.size() >= 1 && wstr[0] == 0xFEFF)
	{
		wstr = wstr.substr(1);
	}
}

void BOM::attach(std::wofstream& wof)
{
	wof.put(0xFEFF);
}

std::string Convert::utf16ToStd(const char* filepath)
{
	auto codec = new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>;

	// init `wide-file` stream
	std::wifstream wif(filepath, std::ios::binary);

	// imbue stream with codec specifics
	wif.imbue(std::locale(wif.getloc(), codec));

	// init wide string stream
	std::wstringstream wss{};

	// Read the content of the file into the stream 
	wss << wif.rdbuf();

	// convert widestream into widestring
	std::wstring wideStr = wss.str();
	BOM::remove(wideStr);

	// Convert wide string into narrow string
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

	return converter.to_bytes(wideStr);
}

// Converts std string result back to wide string for writing
std::wstring Convert::widen(std::string& narrowStr)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(narrowStr);
}

// Splits string of merged files into individual frames
Framer::FramesVec Framer::splitInto(std::string& str)
{
	Framer::FramesVec result;
	size_t start = 0;
	size_t end = str.find(FRAME_DELIMITER);

	while (end != std::string::npos) {
		result.push_back(str.substr(start, end - start));
		start = end + std::strlen(FRAME_DELIMITER);
		end = str.find(FRAME_DELIMITER, start);
	}

	result.push_back(str.substr(start));

	return result;
}

// Sorts instances of Frame class by sortKey
void Framer::sortFrames(std::vector<Frame>& frames)
{
	auto compare = [](const Frame& a, const Frame& b)
		{
			// sortKey is # of millis in 00:00:00.00 formatted time
			return a.sortKey < b.sortKey;
		};

	std::sort(frames.begin(), frames.end(), compare);
}

// Rejoin individual frames with appropriate tokens/delimiter 
std::string Framer::joinFrames(std::vector<Frame> frames)
{
	std::string result{ "- " + frames[0].timeStr + frames[0].frameStr };

	for (int i{ 1 }; i < frames.size(); ++i)
	{
		Frame f = frames[i];
		result += (FRAME_DELIMITER + ("- " + f.timeStr + f.frameStr));
	}

	return result;
}

// Widen std string result, reattach BOM and write to file
void Convert::writeUtf16(std::string& result)
{
	std::wofstream outputFile("merged.txt", std::ios::binary);

	if (!outputFile.is_open()) {
		std::cerr << "Error opening the file." << std::endl;
		return;
	}

	outputFile.write(Convert::widen(result).c_str(), result.size());
	BOM::attach(outputFile);

	outputFile.close();
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Received too few arguments...\n";
		return 1;
	}

	const char* cmd = argv[1];

	if (std::strcmp(cmd, "merge") == 0)
	{
		std::cout << "Received `merge` command.  Gathering files ... \n";

		std::string all{};

		for (int i{ 2 }; i < argc; ++i)
		{
			all += Convert::utf16ToStd(argv[i]);
		}

		std::cout << "Processing frames ... \n";

		Framer::FramesVec frameStrs{ Framer::splitInto(all) };

		std::vector<Framer::Frame> frames{};

		for (size_t i{ 0 }; i < frameStrs.size(); ++i)
		{
			if (frameStrs[i].empty()) break;

			std::string timeStr = frameStrs[i].substr(2, 11);
			std::string frameStr = frameStrs[i].substr(13);

			auto frame = Framer::Frame(timeStr, frameStr);

			frames.emplace_back(frame);
		}

		Framer::sortFrames(frames);

		std::string result = Framer::joinFrames(frames);

		std::cout << "Frames merged and sorted.\n";
		std::cout << "Writing results to merged.txt in working directory ... \n";
		Convert::writeUtf16(result);

		std::cout << "Tracemerge complete.\n";
	}

	return 0;
}