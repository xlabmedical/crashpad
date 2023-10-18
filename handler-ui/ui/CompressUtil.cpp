//
// Created by Miha Oražem on 18. 10. 23.
//

#include "CompressUtil.h"
#include <archive.h>
#include <archive_entry.h>
#include <QFileInfo>
#include <QTemporaryDir>
#include <fstream>
#include <iostream>
#include <vector>

#include "Logging.h"

bool addFileToArchive(const std::string& filePath, struct archive* archive) {
  std::ifstream file(filePath, std::ios::binary);

  if (!file) {
    std::cerr << "Could not open file: " << filePath << std::endl;
    return false;
  }

  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> fileData(size);

  if (!file.read(fileData.data(), size)) {
    std::cerr << "Could not read file: " << filePath << std::endl;
    return false;
  }

  QFileInfo fileInfo(filePath.c_str());

  struct archive_entry* entry = archive_entry_new();
  archive_entry_set_pathname(entry, fileInfo.fileName().toStdString().c_str());
  archive_entry_set_size(entry, size);
  archive_entry_set_filetype(entry, AE_IFREG);
  archive_entry_set_perm(entry, 0644);
  archive_write_header(archive, entry);
  archive_write_data(archive, fileData.data(), size);
  archive_entry_free(entry);

  return true;
}

std::optional<QString> CompressUtil::CompressRGProjectFiles(
    const std::vector<std::string>& files) {
  QTemporaryDir tempDir;
  tempDir.setAutoRemove(false);
  auto filePath = tempDir.filePath("archive.zip");
  const auto filePathString = filePath.toStdString();
  int status = 0;

  struct archive* archive = archive_write_new();

  status = archive_write_set_format_zip(archive);
  if (status != ARCHIVE_OK) {
    LOG(WARNING) << "Error setting archive format: " << status;
    return std::nullopt;
  }
  status = archive_write_open_filename(archive, filePathString.c_str());
  if (status != ARCHIVE_OK) {
    LOG(WARNING) << "Error opening archive: " << status;
    return std::nullopt;
  }

  for (const auto& item : files) {
    status = addFileToArchive(item, archive);
    if (!status) {
      LOG(WARNING) << "Error adding file to archive: " << status;
      return std::nullopt;
    }
  }

  status = archive_write_close(archive);
  if (status != ARCHIVE_OK) {
    LOG(WARNING) << "Error closing archive: " << status;
    return std::nullopt;
  }
  archive_write_free(archive);
  LOG(INFO) << "Archive created: " << filePath.toStdString().c_str();
  return filePath;
}
