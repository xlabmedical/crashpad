//
// Created by Miha Oražem on 25/07/2023.
//

#include <iostream>

#include "base/files/file_path.h"
#include "base/logging.h"
#include "medic_attachment_util.h"

#include <QDebug>
#include <QDir>

int main() {
  logging::LoggingSettings settings;
  settings.logging_dest =
      logging::LOG_TO_SYSTEM_DEBUG_LOG | logging::LOG_TO_STDERR;
  logging::InitLogging(settings);

  const char* directoryPath =
      "/Users/miha/.config/RealGUIDE50-DB/Storage/Chrome1676563822/Projects";
  QDir dir(directoryPath);

  if (!dir.exists()) {
    qWarning() << "Directory not found: " << directoryPath;
    return 0;
  }

  // Set the file filter to the query
  QStringList fileFilter;
  fileFilter << "2ChromeCarrier_DEMO\\.*";
  dir.setNameFilters(fileFilter);

  QStringList fileList = dir.entryList();
  std::vector<std::string> files;
  for (const QString& fileName : fileList) {
    const auto fullPath = dir.filePath(fileName);
    files.push_back(fullPath.toStdString());
  }

  const auto compressedFile =
      MedicAttachmentUtil::CompressRGProjectFiles(files);

  if (compressedFile.has_value()) {
    MedicAttachmentUtil::UploadRGProjectFile(
        "ed1c3e26-b029-4d29-b6b3-a84b3e573a37",
        base::FilePath(compressedFile.value()));
  }
}
