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
    connect(this, &CrashUploadProgressDialog::compressionFinished, this, [=](const QString& archivePath, const QString& reportId) {
      uploadAttachment(archivePath, reportId);
    });
}

CrashUploadProgressDialog::~CrashUploadProgressDialog() {
  delete ui;
}

void CrashUploadProgressDialog::uploadAttachment(const QString& archivePath,
                                                 const QString& reportId) {
//  QUrl url("http://localhost:9090/" + reportId);
  QUrl url(QString("https://crash.medicteam.io/upload-item/%1").arg(reportId));
  QNetworkRequest request(url);

  request.setRawHeader("Medic-Secret", "0e992b12-1192-4a65-a0c0-b4461d28d12f");

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
    qDebug() << "Progress" << bytesSent << bytesTotal;
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesSent);
    int percentage = (bytesSent * 100) / bytesTotal;
      ui->labelAction->setText(QString("Uploading crash report...(%1%)").arg(percentage));
  });
    ui->labelAction->setText("Uploading crash report...");
}


void CrashUploadProgressDialog::uploadAttachmentsExec(XMedicProject project) {
    this->projectToUpload = std::move(project);
    ui->labelAction->setText("Preparing crash report...");
    actionFutures.push_back(std::async(std::launch::async, [&]() {
      const auto response = MedicAttachmentUtil::CompressRGProjectFiles(projectToUpload.files);
        if (response.has_value()) {
          emit compressionFinished(response.value(), QString::fromStdString(projectToUpload.report_uuid));
        }
    }));

    exec();
}
