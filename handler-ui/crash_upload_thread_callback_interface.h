//
// Created by Miha on 28/07/2023.
//

#ifndef CRASHPAD_CRASH_UPLOAD_THREAD_CALLBACK_INTERFACE_H
#define CRASHPAD_CRASH_UPLOAD_THREAD_CALLBACK_INTERFACE_H
#include "client/crash_report_database.h"
namespace crashpad {
class CrashUploadThreadCallbackInterface {
public:
 virtual bool hasUploadConsent() = 0;
 virtual bool onBeforeUploadReport(CrashReportDatabase::UploadReport* report) = 0;
 virtual void onUploadReportDone(CrashReportDatabase::UploadReport* report) = 0;
};
}  // namespace crashpad

#endif  // CRASHPAD_CRASH_UPLOAD_THREAD_CALLBACK_INTERFACE_H
