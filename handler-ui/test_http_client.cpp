//
// Created by Miha Oražem on 25/07/2023.
//

#include <iostream>

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

int main() {

  logging::LoggingSettings settings;
  settings.logging_dest = logging::LOG_TO_SYSTEM_DEBUG_LOG | logging::LOG_TO_STDERR;
  logging::InitLogging(settings);
  HTTPMultipartBuilder http_multipart_builder;
  http_multipart_builder.SetGzipEnabled(true);

  static constexpr char kMinidumpKey[] = "upload_file_minidump";
  const std::map<std::string, std::string> parameters = {
      std::make_pair("prod", "Crashpad"),
  };
  for (const auto& kv : parameters) {
    if (kv.first == kMinidumpKey) {
    } else {
      http_multipart_builder.SetFormData(kv.first, kv.second);
    }
  }

//  for (const auto& it : report->GetAttachments()) {
//    http_multipart_builder.SetFileAttachment(
//        it.first, it.first, it.second, "application/octet-stream");
//  }

  std::unique_ptr<FileReader> reader = std::make_unique<FileReader>();
  reader->Open(base::FilePath(L"C:\\work\\build.bat"));
  http_multipart_builder.SetFileAttachment(kMinidumpKey,
                                           "aaaa.dmp",
                                           reader.get(),
                                           "application/octet-stream");

  std::unique_ptr<HTTPTransport> http_transport(HTTPTransport::Create());

  HTTPHeaders content_headers;
  http_multipart_builder.PopulateContentHeaders(&content_headers);
  for (const auto& content_header : content_headers) {
    http_transport->SetHeader(content_header.first, content_header.second);
  }
  http_transport->SetHeader("Medic-Secret", "0e992b12-1192-4a65-a0c0-b4461d28d12f");
  std::unique_ptr<HTTPBodyStream> stream = std::make_unique<FileReaderHTTPBodyStream>(reader.get());
  http_transport->SetBodyStream(std::move(stream));
  // TODO(mark): The timeout should be configurable by the client.
  http_transport->SetTimeout(120);

 std::string url = "http://dali:8000/upload-item/test";
  http_transport->SetMethod("PUT");
    // Add parameters to the URL which identify the client to the server.
    static constexpr struct {
      const char* key;
      const char* url_field_name;
    } kURLParameterMappings[] = {
        {"prod", "product"},
        {"ver", "version"},
        {"guid", "guid"},
    };

    for (const auto& parameter_mapping : kURLParameterMappings) {
      const auto it = parameters.find(parameter_mapping.key);
      if (it != parameters.end()) {
//        url.append(
//            base::StringPrintf("%c%s=%s",
//                               url.find('?') == std::string::npos ? '?' : '&',
//                               parameter_mapping.url_field_name,
//                               URLEncode(it->second).c_str()));
      }
    }

    http_transport->SetBodyStream(http_multipart_builder.GetBodyStream());
    http_transport->SetURL(url);

  std::string response_body;
  if (!http_transport->ExecuteSynchronously(&response_body)) {
      std::cout << "error" << std::endl;
  }
  std::cout << url << std::endl;
  std::cout << response_body << std::endl;
}
