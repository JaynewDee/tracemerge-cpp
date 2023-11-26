#ifndef FRAMER_H
#define FRAMER_H
#endif

#include <string>
#include <vector>

namespace Framer
{
	using FramesVec = std::vector<std::string>;

	class Frame
	{
	public:
		std::string timeStr;
		long long sortKey;
		std::string frameStr;

		Frame(std::string timeStr, std::string frameStr)
			: timeStr(timeStr), frameStr(frameStr)
		{
			sortKey = millisFromTimeStr(timeStr);
		};
	private:
		long long millisFromTimeStr(std::string&) {
			int hours, minutes, seconds, milliseconds;
			char colon1, colon2, dot;
			std::istringstream ss(timeStr);

			ss >> hours >> colon1 >> minutes >> colon2 >> seconds >> dot >> milliseconds;

			if (ss.fail() || colon1 != ':' || colon2 != ':' || dot != '.') {
				throw std::runtime_error("Error parsing time string");
			}

			return hours * 3600000LL + minutes * 60000LL + seconds * 1000LL + milliseconds;
		};
	};

	std::string parseTimeStrFromFrame(std::string);
	std::string joinFrames(std::vector<Frame>);
	FramesVec splitInto(std::string&);
	void sortFrames(std::vector<Frame>&);

}
