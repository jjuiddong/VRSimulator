
#include <iostream>
#include <Windows.h>
#include <map>
#include<string>
#include <fstream>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace std;

struct sWeeks
{
	string w1; // english
	string w2; // chineses
};
sWeeks g_weeks[] = {
	{ "Mon", "êÅ" },
	{ "Tue", "ûý" },
	{ "Wed", "â©" },
	{ "Thu", "ÙÊ" },
	{ "Fri", "ÐÝ" },
	{ "Sat", "÷Ï" },
	{ "Sun", "ìí" },
};
const int g_weeksSize = sizeof(g_weeks) / sizeof(sWeeks);
map<string, string> g_weeksMap;

void WriteGameResult(const string &gameName, const string &userId, const string &trackName, const int lapTime, const int lank)
{
	using namespace std;
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	if (g_weeksMap.empty())
	{
		for (int i = 0; i < g_weeksSize; ++i)
			g_weeksMap.insert({ g_weeks[i].w1, g_weeks[i].w2 });
	}

	const date curDate = second_clock::local_time().date(); // today date
	time_t time = std::time(nullptr);
	std::tm tm;
	localtime_s(&tm, &time);
	
	ofstream excelFile;
	excelFile.open("game.csv", ios_base::app);

	excelFile << std::put_time(&tm, "%Y-%m-%d %H:%M:%S"); //to_iso_extended_string(curDate);
	excelFile << "\t";
	excelFile << curDate.day_of_week();
	excelFile << "\t";
	excelFile << g_weeksMap[curDate.day_of_week().as_short_string()];
	excelFile << "\t" << gameName << "\t" << userId << "\t" << trackName << "\t" << lapTime << "\t" << lank << endl;
	excelFile.close();
}

void main()
{
	WriteGameResult("dirt3", "testId_1", "track 1", 100, 2);
	Sleep(1000);
	WriteGameResult("dirt3", "testId_1", "track 2", 1000, 2);
	Sleep(1000);
	WriteGameResult("dirt3", "testId_2", "track 1", 2000, 1);
	Sleep(1000);
	WriteGameResult("dirt3", "testId_2", "track 4", 23000, 3);

	cout << "end" << endl;
}
