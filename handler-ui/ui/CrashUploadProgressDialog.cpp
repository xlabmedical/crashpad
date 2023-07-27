//
// Created by Miha on 27/07/2023.
//

// You may need to build the project (run Qt uic code generator) to get
// "ui_CrashUploadProgressDialog.h" resolved

#include "CrashUploadProgressDialog.h"
#include "ui_CrashUploadProgressDialog.h"

CrashUploadProgressDialog::CrashUploadProgressDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::CrashUploadProgressDialog) {
  ui->setupUi(this);

    setWindowTitle(tr("RealGUIDE Crash Report"));
    ui->progressBar->setRange(0, 0);
    ui->progressBar->setValue(0);

}

void CrashUploadProgressDialog::uploadAttachmentsExec() {



}

CrashUploadProgressDialog::~CrashUploadProgressDialog() {
  delete ui;
}
