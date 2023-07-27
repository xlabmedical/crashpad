//
// Created by Miha on 27/07/2023.
//

#ifndef CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H
#define CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class CrashUploadProgressDialog;
}

class CrashUploadProgressDialog : public QDialog {
  Q_OBJECT

 public:
  explicit CrashUploadProgressDialog(QWidget* parent = nullptr);
  ~CrashUploadProgressDialog() override;

  void uploadAttachmentsExec();
 private:
  Ui::CrashUploadProgressDialog* ui;
};

#endif  // CRASHPAD_CRASHUPLOADPROGRESSDIALOG_H
