//
// Created by Miha Oražem on 26/07/2023.
//

#ifndef CRASHPAD_MEDIC_ATTACHMENT_UTIL_H
#define CRASHPAD_MEDIC_ATTACHMENT_UTIL_H

#include <vector>
#include <optional>
#include "base/files/file_path.h"

std::wstring convertWString(std::string str);
namespace MedicAttachmentUtil {

  std::vector<base::FilePath> GetRGProjectFiles();
  std::optional<std::string> CompressRGProjectFiles(const std::vector<std::string>& files);
  bool UploadRGProjectFile(std::string report_id, base::FilePath file);
};

#endif  // CRASHPAD_MEDIC_ATTACHMENT_UTIL_H
