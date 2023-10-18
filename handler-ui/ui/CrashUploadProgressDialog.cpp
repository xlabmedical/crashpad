//
// Created by Miha on 27/07/2023.
//

// You may need to build the project (run Qt uic code generator) to get
// "ui_CrashUploadProgressDialog.h" resolved

#include "CrashUploadProgressDialog.h"

#include <QFile>
#include <QHttpPart>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <thread>
#include <utility>
#include "CompressUtil.h"
#include "QNetworkReply"
#include "ui_CrashUploadProgressDialog.h"
CrashUploadProgressDialog::CrashUploadProgressDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::CrashUploadProgressDialog) {
  ui->setupUi(this);

  setWindowTitle(tr("RealGUIDE Crash Report"));
  ui->progressBar->setRange(0, 0);
  ui->progressBar->setValue(0);
  connect(this,
          &CrashUploadProgressDialog::compressionFinished,
          this,
          [=](const QString& archivePath, const QString& reportId) {
            uploadAttachment(archivePath, reportId);
          });
  connect(
      this, &CrashUploadProgressDialog::error, this, [=](const QString& error) {
        QMessageBox::critical(this, "Error", error);
        reject();
      });
  setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint);
  ui->btnCancel->setEnabled(false);
}

CrashUploadProgressDialog::~CrashUploadProgressDialog() {
  delete ui;
}

void CrashUploadProgressDialog::uploadAttachment(const QString& archivePath,
                                                 const QString& reportId) {
  QUrl url(QString("https://crash.medicteam.io/upload-item/%1").arg(reportId));
  QNetworkRequest request(url);

  request.setRawHeader("Medic-Secret", "0e992b12-1192-4a65-a0c0-b4461d28d12f");

  auto* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  QHttpPart filePart;
  filePart.setHeader(QNetworkRequest::ContentTypeHeader,
                     QVariant("application/octet-stream"));
  const auto dispositionHeader =
      QString(R"(form-data; name="project_file"; filename="archive.zip")");
  filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                     QVariant(dispositionHeader));

  auto* file = new QFile(archivePath);
  file->open(QIODevice::ReadOnly);
  filePart.setBodyDevice(file);
  file->setParent(multiPart);

  multiPart->append(filePart);

  QNetworkReply* reply = manager.post(request, multiPart);
  multiPart->setParent(reply);  // delete the multiPart with the reply
  connect(reply, &QNetworkReply::finished, this, [=]() {
    if (reply->error() == QNetworkReply::NoError) {
      qDebug() << "Success" << reply->readAll();
    } else {
      qDebug() << "Failure" << reply->errorString();
    }
    file->close();
    QFile::remove(archivePath);
    accept();
  });
  connect(reply,
          &QNetworkReply::uploadProgress,
          this,
          [=](qint64 bytesSent, qint64 bytesTotal) {
            if (bytesTotal == 0) {
              return;
            }
            ui->progressBar->setMaximum(bytesTotal);
            ui->progressBar->setValue(bytesSent);
            int percentage = (bytesSent * 100) / bytesTotal;
            ui->labelAction->setText(
                QString("Uploading crash report...(%1%)").arg(percentage));
          });
  ui->labelAction->setText("Uploading crash report...");
  connect(ui->btnCancel, &QPushButton::clicked, this, [=]() {
    if (reply->isRunning()) {
      reply->abort();
    }
    qDebug() << "Aborted";
    reject();
  });
  ui->btnCancel->setEnabled(true);
}

bool CrashUploadProgressDialog::uploadAttachmentsExec(XMedicProject project) {
  this->projectToUpload = std::move(project);
  actionFutures.push_back(std::async(std::launch::async, [&]() {
    const auto response =
        CompressUtil::CompressRGProjectFiles(projectToUpload.files);
    if (response.has_value()) {
      emit compressionFinished(
          response.value(),
          QString::fromStdString(projectToUpload.report_uuid));
    } else {
      emit error(
          "Error compressing project files. The project will not be included.");
    }
  }));
  return exec() == QDialog::Accepted;
}
void CrashUploadProgressDialog::showEvent(QShowEvent* event) {
  QDialog::showEvent(event);
  setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
  raise();
  activateWindow();
}

void CrashUploadProgressDialog::showDialog() {
  ui->labelAction->setText("Preparing crash report...");
  open();
}
