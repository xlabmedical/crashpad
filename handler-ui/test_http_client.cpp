//
// Created by Miha Oražem on 25/07/2023.
//

#include <iostream>

#include "base/files/file_path.h"
#include "base/logging.h"
#include "medic_attachment_util.h"
int main() {
  logging::LoggingSettings settings;
  settings.logging_dest =
      logging::LOG_TO_SYSTEM_DEBUG_LOG | logging::LOG_TO_STDERR;
  logging::InitLogging(settings);
  MedicAttachmentUtil::UploadRGProjectFile(
      "ed1c3e26-b029-4d29-b6b3-a84b3e573a36",
      base::FilePath("/Users/miha/Downloads/512MB.zip"));
}
