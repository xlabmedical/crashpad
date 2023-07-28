//
// Created by Miha Oražem on 28/07/2023.
//

#include "CrashUploadHandler.h"
#include <QSettings>
#include <future>
#include "CrashUploadDialog.h"
#include "CrashUploadProgressDialog.h"
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
  mpSettings = std::make_unique<QSettings>(mDatabasePath + "/settings.ini",
                                           QSettings::IniFormat);
  mUploadConsent =
      mpSettings
          ->value(UPLOAD_CONSENT_KEY, static_cast<int>(UploadConsent::NotAsked))
          .value<UploadConsent>();
  mUploadConsentType = mpSettings
                           ->value(UPLOAD_CONSENT_KEY,
                                   static_cast<int>(UploadConsentType::Basic))
                           .value<UploadConsentType>();
}

bool CrashUploadHandler::hasUploadConsent() {
  if (mUploadConsent == UploadConsent::NotAsked ||
      mUploadConsent == UploadConsent::GrantedOnce) {
    std::promise<DialogResult> promise;

    mpDispatcher->dispatch([this, &promise]() {
      CrashUploadDialog dialog;
      promise.set_value(dialog.execDialogWithResult());
    });

    const auto result = promise.get_future().get();

    mUploadConsent = UploadConsent::NotAsked;

    if (result.selection == UserSelection::Never) {
      mUploadConsent = UploadConsent::Denied;
    }

    if (result.selection == UserSelection::Once) {
      mUploadConsent = UploadConsent::GrantedOnce;
    }

    if (result.selection == UserSelection::Always) {
      mUploadConsent = UploadConsent::Granted;
    }
    mUploadConsentType = UploadConsentType::Basic;
    if (result.uploadProjectFiles) {
      mUploadConsentType = UploadConsentType::WithProject;
    }

    mpSettings->setValue(UPLOAD_CONSENT_KEY, static_cast<int>(mUploadConsent));
    mpSettings->setValue(UPLOAD_CONSENT_TYPE,
                         static_cast<int>(mUploadConsentType));
  }

  return mUploadConsent == UploadConsent::Granted ||
         mUploadConsent == UploadConsent::GrantedOnce;
}

bool CrashUploadHandler::onBeforeUploadReport(
    const crashpad::CrashReportDatabase::UploadReport* report) {
  if (mUploadConsentType == UploadConsentType::WithProject) {
    const auto project = MedicAttachmentUtil::GetMedicProjectFromReport(report);
    if (project) {
      std::promise<bool> promise;
      mpDispatcher->dispatch([this, report, project, &promise]() {
        CrashUploadProgressDialog dialog;
        dialog.uploadAttachmentsExec(project.value());
        promise.set_value(true);
      });
      return promise.get_future().get();
    }
  }
  return false;
}

void CrashUploadHandler::onUploadReportDone(
    const crashpad::CrashReportDatabase::UploadReport* report) {}
