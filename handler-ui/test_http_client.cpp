//
// Created by Miha Oražem on 25/07/2023.
//

#include <iostream>

#include "base/files/file_path.h"
#include "base/logging.h"
#include "handler-ui/ui/CrashUploadProgressDialog.h"
#include "MedicAttachmentUtil.h"

#include <QApplication>
#include <QDebug>
#include <QDir>

int main(int argc, char** argv) {
  logging::LoggingSettings settings;
  settings.logging_dest =
      logging::LOG_TO_SYSTEM_DEBUG_LOG | logging::LOG_TO_STDERR;
  logging::InitLogging(settings);

  //  const char* directoryPath =
  //      "/Users/miha/.config/RealGUIDE50-DB/Storage/Chrome1676563822/Projects";
  //
  const char* directoryPath =
      "C:\\Users\\Miha\\AppData\\Roaming\\RealGUIDE50-"
      "DB\\Storage\\XX1684238711\\Projects";
  QDir dir(directoryPath);

  if (!dir.exists()) {
    qWarning() << "Directory not found: " << directoryPath;
    return 0;
  }

  // Set the file filter to the query
  QStringList fileFilter;
  fileFilter << "2ChromeCarrier_DEMO.*";
  //  fileFilter << "Final-Hutchinson-Mary-Clean\.*";
  dir.setNameFilters(fileFilter);

  QStringList fileList = dir.entryList();
  std::vector<std::string> files;
  for (const QString& fileName : fileList) {
    const auto fullPath = dir.filePath(fileName);
    files.push_back(fullPath.toStdString());
  }
//  QApplication app(argc, argv);
//  run_in_gui_thread_blocking(new QAppLambda([]() {
//    CrashUploadProgressDialog dialog;
//
//    dialog.uploadAttachmentsExec(
////        XMedicProject({{"/Users/miha/Downloads/3Diemme.zip"}, "truetest"}));
//              XMedicProject({{"C:\\Users\\Miha\\Downloads\\RealGUIDE5.3-x64-14-07-2023-Setup.exe"}, "truetest2"}));
//  }));
  //    app.exec();


}
