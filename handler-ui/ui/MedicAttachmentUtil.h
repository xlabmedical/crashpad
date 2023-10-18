//
// Created by Miha Oražem on 26/07/2023.
//

#ifndef CRASHPAD_MEDICATTACHMENTUTIL_H
#define CRASHPAD_MEDICATTACHMENTUTIL_H

#include <optional>
#include <vector>
#include "base/files/file_path.h"
#include "client/crash_report_database.h"
#include "base/logging.h"

#include <QString>

#include "XMedicProject.h"

void setLogPath(const base::FilePath& path);
std::string fpToString(const base::FilePath& filePath);
QString fpToQString(const base::FilePath& filePath);

bool MedicCustomLogging(logging::LogSeverity severity,
                               const char* file,
                               int line,
                               size_t message_start,
                               const std::string& str);
namespace MedicAttachmentUtil {
std::optional<XMedicProject> GetMedicProjectFromReport(
    const crashpad::CrashReportDatabase::UploadReport* report);
};  // namespace MedicAttachmentUtil

#endif  // CRASHPAD_MEDICATTACHMENTUTIL_H
