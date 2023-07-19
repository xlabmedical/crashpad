//
// Created by Miha Oražem on 19/07/2023.
//

#include "CrashUploadDialog.h"
#include <QDialogButtonBox>
#include "ui_CrashUploadDialog.h"

CrashUploadDialog::CrashUploadDialog(QWidget* parent) : QDialog(parent) {
  ui = new Ui::CrashUploadDialog;
  ui->setupUi(this);
  setWindowTitle(tr("RealGUIDE Crash detected"));
  connect(ui->btnAlways, &QAbstractButton::clicked, this, [this]() {
    done(static_cast<int>(UserSelection::Always));
  });
  connect(ui->btnNever, &QAbstractButton::clicked, this, [this]() {
    done(static_cast<int>(UserSelection::Never));
  });
  connect(ui->btnOk, &QAbstractButton::clicked, this, [this]() {
    done(static_cast<int>(UserSelection::Once));
  });

  ui->btnAlways->setFocus();
}

CrashUploadDialog::~CrashUploadDialog() {
  delete ui;
}

DialogResult CrashUploadDialog::execDialogWithResult() {
  auto result = exec();
  return DialogResult{
        static_cast<UserSelection>(result),
        ui->checkBoxProject->isChecked(),
  };
}
