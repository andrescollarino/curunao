#include "Debug.h"



void Debug::Log(std::string message)
{
  LogPrint("", __LINE__, message, NULL);
}


void Debug::Log(std::string fileError, int numberLine, std::string message)
{
  LogPrint(fileError, numberLine, message, NULL);
}


void Debug::Log(std::string fileError, int numberLine, std::string message, HRESULT result)
{
  LogPrint(fileError, numberLine, message, result);
}

void Debug::Log(int numberLine, std::string message, HRESULT result)
{
  LogPrint("", numberLine, message, result);
}

void Debug::Log(std::string message, HRESULT result)
{
  LogPrint("", __LINE__, message, result);
}

void Debug::LogPrint(std::string fileError, int numberLine, std::string message, HRESULT result)
{
  std::ofstream file;

  file.open("GameLog.txt", std::ios::app);

  file << GetCurrentDay() << " " << fileError << "(" << numberLine << ") : " << message << std::hex << result << std::endl;

  file.close();
}


const std::string Debug::GetCurrentDay()
{
  time_t now = time(0);
  struct tm  tstruct;

  localtime_s(&tstruct, &now);

  std::string result =
    std::to_string(tstruct.tm_mday) + "/" +
    std::to_string(tstruct.tm_mon) + "/" +
    std::to_string(tstruct.tm_year) + "  " +
    std::to_string(tstruct.tm_hour) + ":" +
    std::to_string(tstruct.tm_min);


  return result;
}