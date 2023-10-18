//
// Created by Miha on 27/07/2023.
//

#ifndef CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H
#define CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <future>
#include "XMedicProject.h"
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
                        const QString& reportId, const QString& uploadUrl);

  void getUploadURL(const QString& archivePath, const QString& reportId);
 public:
  explicit CrashUploadProgressDialog(QWidget* parent = nullptr);
  ~CrashUploadProgressDialog() override;

  bool uploadAttachmentsExec(XMedicProject project);
  void showDialog();

 protected:
  void showEvent(QShowEvent* event) override;
 signals:
    void compressionFinished(const QString& archivePath, const QString& reportId);
    void error(const QString& error);
};

#endif  // CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H
