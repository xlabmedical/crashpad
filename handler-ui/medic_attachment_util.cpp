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

std::vector<base::FilePath> MedicAttachmentUtil::GetRGProjectFiles() {
  return {};
}

std::optional<base::FilePath> MedicAttachmentUtil::CompressRGProjectFiles(
    const std::vector<base::FilePath>& files) {
  return {};
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
