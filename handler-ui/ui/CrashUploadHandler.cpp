//
// Created by Miha Oražem on 28/07/2023.
//

#include "CrashUploadHandler.h"
#include <QSettings>
#include <future>
#include "CrashUploadDialog.h"
#include "CrashUploadProgressDialog.h"

#ifdef Q_OS_WIN
#include <QtPlatformHeaders/QWindowsWindowFunctions>
#endif

Q_DECLARE_METATYPE(UploadConsent);
Q_DECLARE_METATYPE(UploadConsentType);
const QString UPLOAD_CONSENT_KEY = "UploadConsent";
const QString UPLOAD_CONSENT_TYPE = "UploadConsentType";

CrashUploadHandler::CrashUploadHandler(xQt::callback_dispatcher* pDispatcher,
                                       crashpad::CrashReportDatabase* database,
                                       const QString& pDatabasePath)
    : mpDispatcher(pDispatcher),
      mpDatabase(database),
      mDatabasePath(pDatabasePath) {
#ifdef Q_OS_WIN  // this is Windows specific code, not portable
  QWindowsWindowFunctions::setWindowActivationBehavior(
      QWindowsWindowFunctions::AlwaysActivateWindow);
#endif
}

bool CrashUploadHandler::hasUploadConsent() {
  if (mUploadConsent == UploadConsent::NotAsked ||
      mUploadConsent == UploadConsent::GrantedOnce) {
    std::promise<DialogResult> promise;

    mpDispatcher->dispatch([this, &promise]() {
      LOG(INFO) << "Showing question dialog";
      CrashUploadDialog dialog;
      auto result = dialog.execDialogWithResult();
      promise.set_value(result);
    });

    const auto result = promise.get_future().get();
    LOG(INFO) << "Got dialog result";

    mUploadConsent = UploadConsent::NotAsked;

    if (result.selection == UserSelection::No) {
      mUploadConsent = UploadConsent::Denied;
    }

    if (result.selection == UserSelection::Once) {
      mUploadConsent = UploadConsent::GrantedOnce;
    }
    mUploadConsentType = UploadConsentType::Basic;
    if (result.uploadProjectFiles) {
      mUploadConsentType = UploadConsentType::WithProject;
    }

  }

  LOG(INFO) << "Upload consent: " << static_cast<int>(mUploadConsent);
  LOG(INFO) << "Upload consent type: " << static_cast<int>(mUploadConsentType);

  return mUploadConsent == UploadConsent::GrantedOnce;
}

bool CrashUploadHandler::onBeforeUploadReport(
    const crashpad::CrashReportDatabase::UploadReport* report) {
  mpDispatcher->dispatch([this]() {
    mpProgressDialog = std::make_shared<CrashUploadProgressDialog>();
    mpProgressDialog->showDialog();
  });
  return true;
}

void CrashUploadHandler::onUploadReportDone(
    const crashpad::CrashReportDatabase::UploadReport* report) {
  if (mUploadConsentType == UploadConsentType::WithProject) {
    const auto project = MedicAttachmentUtil::GetMedicProjectFromReport(report);
    LOG(INFO) << "Project: " << project.has_value();
    if (project && mpProgressDialog) {
      std::promise<bool> promise;
      mpDispatcher->dispatch([this, report, project, &promise]() {
        mpProgressDialog->uploadAttachmentsExec(project.value());
        promise.set_value(true);
      });
      const auto value = promise.get_future().get();
      LOG(INFO) << "Waiting for upload to finish" << value;
    }
  }
  if (mpProgressDialog) {
    mpDispatcher->dispatch([this]() {
      mpProgressDialog->close();
    });
  }
  mpProgressDialog.reset();
}

