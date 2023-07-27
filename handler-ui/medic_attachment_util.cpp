//
// Created by Miha Oražem on 26/07/2023.
//

#include "medic_attachment_util.h"
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
  archive_entry_set_pathname(entry, fileInfo.baseName().toStdString().c_str());
  archive_entry_set_size(entry, size);
  archive_entry_set_filetype(entry, AE_IFREG);
  archive_entry_set_perm(entry, 0644);
  archive_write_header(archive, entry);
  archive_write_data(archive, fileData.data(), size);
  archive_entry_free(entry);

  return true;
}

std::vector<base::FilePath> MedicAttachmentUtil::GetRGProjectFiles() {
  return {};
}

std::optional<std::string> MedicAttachmentUtil::CompressRGProjectFiles(
    const std::vector<std::string>& files) {
  QTemporaryDir tempDir;
  tempDir.setAutoRemove(false);
  const auto filePath = tempDir.filePath("archive.tar.xz");

  int status = 0;

  struct archive* archive = archive_write_new();

  status = archive_write_add_filter_xz(archive);
  if(status != ARCHIVE_OK) {
    return std::nullopt;
  }
  status = archive_write_set_options(archive, "threads=0, preset=2");
  if(status != ARCHIVE_OK) {
    return std::nullopt;
  }
  status = archive_write_set_format_pax_restricted(archive);
  if(status != ARCHIVE_OK) {
    return std::nullopt;
  }
  status = archive_write_open_filename(archive, filePath.toStdString().c_str());
  if(status != ARCHIVE_OK) {
    return std::nullopt;
  }

  for (const auto& item : files) {
    bool status = addFileToArchive(item, archive);
    if(!status) {
      return std::nullopt;
    }
  }

  status = archive_write_close(archive);
  if(status != ARCHIVE_OK) {
    return std::nullopt;
  }
  archive_write_free(archive);

  return filePath.toStdString();
}

bool MedicAttachmentUtil::UploadRGProjectFile(std::string report_id,
                                              base::FilePath file) {
  HTTPMultipartBuilder http_multipart_builder;
  http_multipart_builder.SetGzipEnabled(false);

  std::unique_ptr<FileReader> reader = std::make_unique<FileReader>();
  if (!reader->Open(file)) {
    LOG(WARNING) << "Error opening file!" << file.value();
  }
  http_multipart_builder.SetFileAttachment("project_file",
                                           file.BaseName().value(),
                                           reader.get(),
                                           "application/octet-stream");

  std::unique_ptr<HTTPTransport> http_transport(HTTPTransport::Create());

  HTTPHeaders content_headers;
  http_multipart_builder.PopulateContentHeaders(&content_headers);
  for (const auto& content_header : content_headers) {
    http_transport->SetHeader(content_header.first, content_header.second);
  }
  http_transport->SetHeader("Medic-Secret",
                            "0e992b12-1192-4a65-a0c0-b4461d28d12f");

  std::unique_ptr<HTTPBodyStream> stream =
      std::make_unique<FileReaderHTTPBodyStream>(reader.get());

  http_transport->SetBodyStream(std::move(stream));
  http_transport->SetTimeout(300);

  std::string url = "https://crash.medicteam.io/upload-item/" + report_id;
  http_transport->SetBodyStream(http_multipart_builder.GetBodyStream());
  http_transport->SetURL(url);

  std::string response_body;
  if (!http_transport->ExecuteSynchronously(&response_body)) {
    LOG(WARNING) << "Failed to upload file to " << url
                 << " with error: " << response_body;
    return false;
  }
  return true;
}
