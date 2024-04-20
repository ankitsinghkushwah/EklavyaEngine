#ifndef INC_HIP_HOP_LOGGER_H
#define INC_HIP_HOP_LOGGER_H

// C++ headers
#include <cstdio>
#include <cstdarg>
#include <string>
#include "Singleton.h"

class Logger : public Singleton<Logger> {
public:
  ~Logger();
  Logger();
  void Init(const char *header);
  void LogToConsole(const char *pFormat, ...);
  void LogToFile(const char *pFormat, ...);

private:
  FILE *m_File;
};

// #define FILE_LOG

#ifdef FILE_LOG
#define LOG(format) Logger::GetInstance().LogToFile(format);
#define LOG_STRING(format) Logger::GetInstance().LogToFile((format).c_str());
#define LOG_FSTRING(format, data)                                              \
  Logger::GetInstance().LogToFile((format).c_str(), data);
#define LOG_F(format, data) Logger::GetInstance().LogToFile(format, data);
#else
#define LOG(format) Logger::GetInstance().LogToConsole(format);
#define LOG_STRING(format) Logger::GetInstance().LogToConsole((format).c_str());
#define LOG_FSTRING(format, data)                                              \
  Logger::GetInstance().LogToConsole((format).c_str(), data);
#define LOG_F(format, data) Logger::GetInstance().LogToConsole(format, data);
#endif

#endif
