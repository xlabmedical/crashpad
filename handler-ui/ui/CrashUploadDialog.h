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
  Never = 0,
  Once,
  Always,
};

struct DialogResult {
  UserSelection selection;
  bool uploadProjectFiles;
};


class CrashUploadDialog : public QDialog {
  Q_OBJECT
 public:
  explicit CrashUploadDialog(QWidget* parent = nullptr);
  ~CrashUploadDialog() override;
  DialogResult execDialogWithResult();
 private:
  Ui::CrashUploadDialog* ui;
};

#endif  // CRASHPAD_CRASHUPLOADDIALOG_H
