//
// Created by Miha on 27/07/2023.
//

// You may need to build the project (run Qt uic code generator) to get
// "ui_CrashUploadProgressDialog.h" resolved

#include "CrashUploadProgressDialog.h"

#include <QFile>
#include <QHttpPart>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <thread>
#include <utility>
#include "ui_CrashUploadProgressDialog.h"
#include "QNetworkReply"
CrashUploadProgressDialog::CrashUploadProgressDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::CrashUploadProgressDialog) {
  ui->setupUi(this);

    setWindowTitle(tr("RealGUIDE Crash Report"));
    ui->progressBar->setRange(0, 0);
    ui->progressBar->setValue(0);
}

CrashUploadProgressDialog::~CrashUploadProgressDialog() {
  delete ui;
}

void CrashUploadProgressDialog::uploadAttachment(const QString& archivePath,
                                                 const std::string& reportId) {
  QUrl url("http://localhost:9090/" + QString::fromStdString(reportId));
  QNetworkRequest request(url);

  auto *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  QHttpPart filePart;
  filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
  const auto dispositionHeader = QString(R"(form-data; name="project_file"; filename="archive.zip")");
  filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(dispositionHeader));

  auto *file = new QFile(archivePath);
  file->open(QIODevice::ReadOnly);
  filePart.setBodyDevice(file);
  file->setParent(multiPart);

  multiPart->append(filePart);

  QNetworkReply *reply = manager.post(request, multiPart);
  multiPart->setParent(reply); // delete the multiPart with the reply
  connect(reply, &QNetworkReply::finished, this, [=]() {
    if (reply->error() == QNetworkReply::NoError) {
      qDebug() << "Success" << reply->readAll();
    } else {
      qDebug() << "Failure" << reply->errorString();
    }
    reply->deleteLater();
    accept();
  });
  connect(reply, &QNetworkReply::uploadProgress, this, [=](qint64 bytesSent, qint64 bytesTotal) {
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesSent);
  });

  ui->labelAction->setText("Uploading crash report...");
}


void CrashUploadProgressDialog::uploadAttachmentsExec(XMedicProject project) {
    this->projectToUpload = std::move(project);
    ui->labelAction->setText("Preparing crash report...");
    actionFutures.push_back(std::async(std::launch::async, [&]() {
      const auto response = MedicAttachmentUtil::CompressRGProjectFiles(projectToUpload.files);
        if (response.has_value()) {
          uploadAttachment(QString::fromStdString(response.value().value()), projectToUpload.report_uuid);
        }
    }));

    exec();
}
