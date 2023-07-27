//
// Created by Miha on 27/07/2023.
//

#ifndef CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H
#define CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <future>
#include "../medic_attachment_util.h"
namespace Ui {
  class CrashUploadProgressDialog;
}

class CrashUploadProgressDialog : public QDialog {
  Q_OBJECT
 private:
  QNetworkAccessManager manager;
  std::vector<std::future<void>> actionFutures;
  Ui::CrashUploadProgressDialog* ui;
  XMedicProject projectToUpload;

  void uploadAttachment(const QString& archivePath,
                        const std::string& reportId);

 public:
  explicit CrashUploadProgressDialog(QWidget* parent = nullptr);
  ~CrashUploadProgressDialog() override;

  void uploadAttachmentsExec(XMedicProject project);
};

#endif  // CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H
