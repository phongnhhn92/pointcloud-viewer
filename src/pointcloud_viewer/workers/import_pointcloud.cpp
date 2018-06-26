#include <pointcloud_viewer/workers/import_pointcloud.hpp>
#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud/importer/abstract_importer.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>

#include <QDebug>
#include <QFileInfo>
#include <QThread>
#include <QMessageBox>
#include <QCoreApplication>
#include <QProgressDialog>
#include <QAbstractEventDispatcher>

#include <fstream>

PointCloud failed(){return PointCloud();}


PointCloud import_point_cloud(QWidget* parent, QString filepath)
{
  QFileInfo file(filepath);

  if(!file.exists())
  {
    QMessageBox::warning(parent, "Not existing file", QString("The given file <%0> does not exist!").arg(filepath));
    return failed();
  }

  const std::string filepath_std = file.absoluteFilePath().toStdString();

  if(!file.isReadable())
  {
    QMessageBox::warning(parent, "Can't existing file", QString("Could not open the file <%0> for reading.").arg(filepath));
    return failed();
  }

  const QString suffix = file.suffix();

  QSharedPointer<AbstractPointCloudImporter> importer = AbstractPointCloudImporter::importerForSuffix(suffix, filepath_std);

  if(!importer)
  {
    QMessageBox::warning(parent, "Unexpected file format", QString("Unexpected file format '%0'.").arg(suffix));
    return failed();
  }

  QProgressDialog progressDialog(QString("Importing Point Cloud Layer\n<%1>").arg(file.fileName()), "&Abort", 0, AbstractPointCloudImporter::progress_max(), parent);
  progressDialog.setWindowModality(Qt::ApplicationModal);
  progressDialog.setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);

  progressDialog.show();
  progressDialog.setDisabled(true);

  QThread thread;
  importer->moveToThread(&thread);

  bool waiting = true;

  QObject::connect(importer.data(), &AbstractPointCloudImporter::update_progress, &progressDialog, &QProgressDialog::setValue);
  QObject::connect(importer.data(), &AbstractPointCloudImporter::finished, &thread, &QThread::quit);
  QObject::connect(&progressDialog, &QProgressDialog::canceled, importer.data(), &AbstractPointCloudImporter::cancel);
  QObject::connect(&thread, &QThread::started, importer.data(), &AbstractPointCloudImporter::import);
  QObject::connect(&thread, &QThread::finished, [&waiting](){waiting=false;});


  thread.start();

  while(waiting)
    QCoreApplication::processEvents(QEventLoop::EventLoopExec | QEventLoop::DialogExec | QEventLoop::WaitForMoreEvents);

  switch(importer->state)
  {
  case AbstractPointCloudImporter::CANCELED:
    QMessageBox::warning(parent, "Importing Cancelled", QString("Importing the pointcloud file was canceled by the user."));
    return failed();
  case AbstractPointCloudImporter::RUNNING:
  case AbstractPointCloudImporter::IDLE:
    QMessageBox::warning(parent, "Unknown Error", QString("Internal error"));
    return failed();
  case AbstractPointCloudImporter::RUNTIME_ERROR:
    QMessageBox::warning(parent, "Import Error", QString("Couldn't import the file <%0>. Probably an io error or invalid file.").arg(file.fileName()));
    return failed();
  case AbstractPointCloudImporter::SUCCEEDED:
  {
    PointCloud pointcloud(std::move(importer->pointcloud));
    return pointcloud;
  }
  }

  progressDialog.hide();

  return failed();
}
