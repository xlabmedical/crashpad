//
// Created by Miha on 27/07/2023.
//

#include <memory>
#include "base/logging.h"
#include "crash_report_upload_thread.h"
#include "medic_attachment_util.h"
#include "util/thread/stoppable.h"

using namespace crashpad;
class ScopedStoppable {
 public:
  ScopedStoppable() = default;

  ScopedStoppable(const ScopedStoppable&) = delete;
  ScopedStoppable& operator=(const ScopedStoppable&) = delete;

  ~ScopedStoppable() {
    if (stoppable_) {
      stoppable_->Stop();
    }
  }

  void Reset(Stoppable* stoppable) { stoppable_.reset(stoppable); }

  Stoppable* Get() { return stoppable_.get(); }

 private:
  std::unique_ptr<Stoppable> stoppable_;
};
int main() {
  const auto url = "https://o4505199589326848.ingest.sentry.io:443/api/4505560620990464";

  logging::LoggingSettings settings;
  settings.logging_dest = logging::LOG_TO_SYSTEM_DEBUG_LOG;
  logging::InitLogging(settings);
  const base::FilePath& path = base::FilePath(
      L"C:\\Users\\Miha\\AppData\\Roaming\\RealGUIDE50\\crash-db");
  setLogPath(path);
  logging::SetLogMessageHandler(&MedicCustomLogging);
  std::unique_ptr<CrashReportDatabase> database(
      CrashReportDatabase::Initialize(path));
  if (!database) {
    return 0;
  }

  ScopedStoppable upload_thread;
    CrashReportUploadThread::Options upload_thread_options;
    upload_thread_options.identify_client_via_url = false;
    upload_thread_options.rate_limit = false;
    upload_thread_options.upload_gzip = false;
    upload_thread_options.watch_pending_reports = true;

    CrashReportUploadThread* stoppable = new CrashReportUploadThread(
        database.get(),
        url,
        "",
        upload_thread_options,
        CrashReportUploadThread::ProcessPendingReportsObservationCallback());
    upload_thread.Reset(stoppable);
    upload_thread.Get()->Start();

    auto uuid = crashpad::UUID();
    uuid.InitializeFromString("5a604df2-fa15-41ae-9665-aad933406719");

    std::unique_ptr<const CrashReportDatabase::UploadReport> upload_report;
    CrashReportDatabase::OperationStatus status =
      database->GetReportForUploading(uuid, &upload_report);
    stoppable->ReportPending(uuid);


    while(true){

    }
}
