#pragma once

#include <fstream>
#include <ctime>
#include <string>
#include <Windows.h>

class Debug
{
public:

  static void Log(std::string message);
  static void Log(std::string fileError, int numberLine, std::string message, HRESULT result);
  static void Log(int numberLine, std::string message, HRESULT result);
  static void Log(std::string message, HRESULT result);
  static void Log(std::string fileError, int numberLine, std::string message);

private:

  static void LogPrint(std::string fileError, int numberLine, std::string message, HRESULT result);
  static const std::string GetCurrentDay();
};