//
// Created by Miha Oražem on 26/07/2023.
//

#include "MedicAttachmentUtil.h"
#include <codecvt>
#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "util/file/file_reader.h"
#include "util/misc/metrics.h"
#include "util/misc/uuid.h"
#include "util/net/http_body.h"
#include "util/net/http_multipart_builder.h"
#include "util/net/http_transport.h"
#include "util/net/url.h"

using namespace crashpad;
#include <archive.h>
#include <archive_entry.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <QTemporaryDir>
#include <fstream>
#include <iostream>
#include <vector>

#if BUILDFLAG(IS_WIN)
std::string convertString(std::wstring wstr) {
  int num_chars = WideCharToMultiByte(CP_UTF8,
                                      0,
                                      wstr.c_str(),
                                      static_cast<int>(wstr.length()),
                                      NULL,
                                      0,
                                      NULL,
                                      NULL);
  std::string strTo;
  if (num_chars > 0) {
    strTo.resize(num_chars);
    WideCharToMultiByte(CP_UTF8,
                        0,
                        wstr.c_str(),
                        static_cast<int>(wstr.length()),
                        &strTo[0],
                        num_chars,
                        NULL,
                        NULL);
  }
  return strTo;
}
#endif

base::FilePath toFilePath(const QString& string) {
#if BUILDFLAG(IS_WIN)
  return base::FilePath(string.toStdWString());
#else
  return base::FilePath(string.toStdString());
#endif
}

std::string fpToString(const base::FilePath& filePath) {
#if BUILDFLAG(IS_WIN)
  return convertString(filePath.value());
#else
  return filePath.value();
#endif
}


std::string logFilePath;

void setLogPath(const base::FilePath& path) {
  logFilePath = fpToString(path);
  logFilePath += "/crashpad.log";
}

bool MedicCustomLogging(logging::LogSeverity severity,
                               const char* file,
                               int line,
                               size_t message_start,
                               const std::string& str) {
  std::ofstream logfile;
  logfile.open(logFilePath, std::ios_base::app); // append to the log file
  if (!logfile) {
    return false;  // return false if unable to open the file
  }
  logfile << str;

  std::cout << str;

  logfile.close();
  return true;
}
QString fpToQString(const base::FilePath& filePath) {
  return QString::fromStdString(fpToString(filePath));
}



std::optional<XMedicProject> MedicAttachmentUtil::GetMedicProjectFromReport(
    const crashpad::CrashReportDatabase::UploadReport* report) {
  for (const auto& pair : report->GetAttachments()) {
    if (pair.first == "__xmedic_files") {
      LOG(INFO) << "Found medic files" << pair.first;

      XMedicProject project;

      const auto fileSize =  pair.second->Seek(0, SEEK_END);
      pair.second->SeekSet(0);
      auto buffer = std::unique_ptr<char[]>(new char[fileSize + 1]);
      pair.second->ReadExactly(buffer.get(), fileSize);
      pair.second->SeekSet(0);
      const auto content = QString::fromUtf8(buffer.get(), fileSize);
      if (content.isEmpty()) {
        LOG(INFO) << "Content is empty!";
        return std::nullopt;
      }

      LOG(INFO) << "Content is: " << content.toStdString();
      const auto parts = content.split(";");

      std::vector<std::string> partsVector;

      for (const auto& part : parts) {
        partsVector.push_back(part.toStdString());
      }
      LOG(INFO) << "Found " << partsVector.size() << " parts!";

      return XMedicProject{partsVector, report->uuid.ToString()};
    }
  }
  return std::nullopt;
}
