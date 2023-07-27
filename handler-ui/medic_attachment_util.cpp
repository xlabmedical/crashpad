//
// Created by Miha Oražem on 26/07/2023.
//

#include "medic_attachment_util.h"
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

std::optional<base::FilePath> MedicAttachmentUtil::CompressRGProjectFiles(
    const std::vector<std::string>& files) {
  QTemporaryDir tempDir;
  tempDir.setAutoRemove(false);
  const auto filePath = tempDir.filePath("archive.zip");
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
  LOG(INFO) << "Archive created: " << filePath.toStdString();
  return toFilePath(filePath);
}
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>

void UploadRGProjectFile(std::string report_id,
                         QString filePath) {
  QUrl url("https://crash.medicteam.io/upload-item/" + QString::fromStdString(report_id));
  QNetworkRequest request(url);

  QNetworkAccessManager manager;

  auto *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  QHttpPart filePart;
  filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
  const auto dispositionHeader = QString(R"(form-data; name="project_file"; filename="archive.zip")");
  filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(dispositionHeader));

  auto *file = new QFile(filePath);
  file->open(QIODevice::ReadOnly);
  filePart.setBodyDevice(file);
  file->setParent(multiPart);

  multiPart->append(filePart);

  QNetworkReply *reply = manager.post(request, multiPart);
  multiPart->setParent(reply); // delete the multiPart with the reply

//  reply->
  // here connect signals etc.
}
bool MedicAttachmentUtil::UploadRGProjectFile(std::string report_id,
                                              base::FilePath file) {
  HTTPMultipartBuilder http_multipart_builder;
  http_multipart_builder.SetGzipEnabled(false);

  std::unique_ptr<FileReader> reader = std::make_unique<FileReader>();

  if (!reader->Open(file)) {
    LOG(WARNING) << "Error opening file!" << fpToString(file);
  }

  http_multipart_builder.SetFileAttachment("project_file",
                                           fpToString(file.BaseName()),
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
  LOG(INFO) << "Uploading file to: " << url;
  std::string response_body;
  if (!http_transport->ExecuteSynchronously(&response_body)) {
    LOG(WARNING) << "Failed to upload file to " << url
                 << " with error: " << response_body;
    return false;
  }
  LOG(INFO) << "File uploaded success" << url;
  return true;
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

      return XMedicProject{partsVector};
    }
  }
  return std::nullopt;
}
