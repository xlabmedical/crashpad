//
// Created by Miha on 27/07/2023.
//

#ifndef CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H
#define CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <future>
#include "MedicAttachmentUtil.h"
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
                        const QString& reportId);

 public:
  explicit CrashUploadProgressDialog(QWidget* parent = nullptr);
  ~CrashUploadProgressDialog() override;

  void uploadAttachmentsExec(XMedicProject project);
signals:
    void compressionFinished(const QString& archivePath, const QString& reportId);
};

#endif  // CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H
