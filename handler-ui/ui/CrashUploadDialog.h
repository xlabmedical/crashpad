//
// Created by Miha Oražem on 19/07/2023.
//

#ifndef CRASHPAD_CRASHUPLOADDIALOG_H
#define CRASHPAD_CRASHUPLOADDIALOG_H

#include <QDialog>

namespace Ui {
  class CrashUploadDialog;
}

enum class UserSelection {
  No = 0,
  Once,
};

struct DialogResult {
  UserSelection selection;
  bool uploadProjectFiles;
};


class CrashUploadDialog : public QDialog {
  Q_OBJECT
 protected:
  void showEvent(QShowEvent* event) override;

 public:
  explicit CrashUploadDialog(QWidget* parent = nullptr);
  ~CrashUploadDialog() override;
  DialogResult execDialogWithResult();
 private:
  Ui::CrashUploadDialog* ui;
};

#endif  // CRASHPAD_CRASHUPLOADDIALOG_H
