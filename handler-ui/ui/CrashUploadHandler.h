//
// Created by Miha Oražem on 28/07/2023.
//

#ifndef CRASHPAD_CRASHUPLOADHANDLER_H
#define CRASHPAD_CRASHUPLOADHANDLER_H
#include <QSettings>
#include "CrashUploadProgressDialog.h"
#include "crash_upload_thread_callback_interface.h"
#include "xQt/callback_dispatcher.hpp"

enum class UploadConsent {
  NotAsked = -1,
  Denied,
  GrantedOnce,
  Granted,
};

enum class UploadConsentType {
  Basic = 0,
  WithProject,
};

class CrashUploadHandler : public crashpad::CrashUploadThreadCallbackInterface {
 private:
  std::unique_ptr<QSettings> mpSettings;
  xQt::callback_dispatcher* mpDispatcher;
  crashpad::CrashReportDatabase* mpDatabase;
  QString mDatabasePath;

  UploadConsent mUploadConsent;
  UploadConsentType mUploadConsentType;

  std::shared_ptr<CrashUploadProgressDialog> mpProgressDialog;
 public:
  explicit CrashUploadHandler(xQt::callback_dispatcher* pDispatcher,
                              crashpad::CrashReportDatabase* database, const QString& pDatabasePath);
  void reloadSettings();
  bool hasUploadConsent() override;
  bool onBeforeUploadReport(
      const crashpad::CrashReportDatabase::UploadReport* report) override;
  void onUploadReportDone(
      const crashpad::CrashReportDatabase::UploadReport* report) override;
};

#endif  // CRASHPAD_CRASHUPLOADHANDLER_H
