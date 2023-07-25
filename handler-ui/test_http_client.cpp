//
// Created by Miha Oražem on 25/07/2023.
//

#include <iostream>

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
  reader->Open(base::FilePath("/Users/miha/work/tmp/projectpart.stl"));
//  http_multipart_builder.SetFileAttachment(kMinidumpKey,
//                                           "aaaa.dmp",
//                                           reader.get(),
//                                           "application/octet-stream");

  std::unique_ptr<HTTPTransport> http_transport(HTTPTransport::Create());

//  for (const auto& content_header : content_headers) {
//    http_transport->SetHeader(content_header.first, content_header.second);
//  }
  std::unique_ptr<HTTPBodyStream> stream = std::make_unique<FileReaderHTTPBodyStream>(reader.get());
  http_transport->SetBodyStream(std::move(stream));
  // TODO(mark): The timeout should be configurable by the client.
  http_transport->SetTimeout(120);
//  http_transport->

    std::string url = "http://localhost:9090/";
//  std::string url = "https://medic-ai-datasets.s3.amazonaws.com/test.txt?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAV7E76JTQOHV6IODI/20230725/eu-central-1/s3/aws4_request&X-Amz-Date=20230725T201929Z&X-Amz-Expires=3600&X-Amz-SignedHeaders=host&X-Amz-Signature=51e037d3e289bbcba7d967bda2a9f917c4c9166d1a670006a212a4e411277fb8";
// std::string url = "https://medic-ai-datasets.s3.amazonaws.com/test.txt?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAV7E76JTQOHV6IODI%2F20230725%2Feu-central-1%2Fs3%2Faws4_request&X-Amz-Date=20230725T201929Z&X-Amz-Expires=3600&X-Amz-SignedHeaders=host&X-Amz-Signature=51e037d3e289bbcba7d967bda2a9f917c4c9166d1a670006a212a4e411277fb8";
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
  http_transport->SetURL(url);

//  http_transport->SetHTTPProxy("http://localhost:9090");
  std::string response_body;
  if (!http_transport->ExecuteSynchronously(&response_body)) {
  }
  std::cout << url << std::endl;
  std::cout << response_body << std::endl;
}
