//
// Created by Miha Oražem on 26/07/2023.
//

#ifndef CRASHPAD_MEDIC_ATTACHMENT_UTIL_H
#define CRASHPAD_MEDIC_ATTACHMENT_UTIL_H

#include <optional>
#include <vector>
#include "base/files/file_path.h"
#include "client/crash_report_database.h"
#include "base/logging.h"

#include <QString>

struct XMedicProject {
  std::vector<std::string> files;
  std::string report_uuid;
};

void setLogPath(const base::FilePath& path);

bool MedicCustomLogging(logging::LogSeverity severity,
                               const char* file,
                               int line,
                               size_t message_start,
                               const std::string& str);
namespace MedicAttachmentUtil {
std::optional<XMedicProject> GetMedicProjectFromReport(
    const crashpad::CrashReportDatabase::UploadReport* report);
std::optional<QString> CompressRGProjectFiles(
    const std::vector<std::string>& files);
};  // namespace MedicAttachmentUtil

#endif  // CRASHPAD_MEDIC_ATTACHMENT_UTIL_H
