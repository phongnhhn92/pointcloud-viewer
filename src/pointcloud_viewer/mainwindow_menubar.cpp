#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud_viewer/workers/import_pointcloud.hpp>
#include <pointcloud/importer/abstract_importer.hpp>

#include <QMenuBar>

void MainWindow::initMenuBar()
{
  QMenuBar* menuBar = new QMenuBar;
  menuBar->setVisible(true);
  setMenuBar(menuBar);

  QMenu* menu_project = menuBar->addMenu("&Project");
  QAction* import_pointcloud_layers = menu_project->addAction("&Import Pointcloud");

  QMenu* menu_flythrough = menuBar->addMenu("&Flythrough");
  QAction* action_flythrough_insert_keypoint = menu_flythrough->addAction("&Insert Keypoint");

  action_flythrough_insert_keypoint->setShortcut(QKeySequence(Qt::Key_I));
  connect(action_flythrough_insert_keypoint, &QAction::triggered, this, &MainWindow::insertKeypoint);

  QMenu* menu_view = menuBar->addMenu("&View");
  QMenu* menu_view_navigation = menu_view->addMenu("&Navigation");
  QAction* action_view_navigation_fps = menu_view_navigation->addAction("&First Person Navigation");

  action_view_navigation_fps->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F));
  connect(action_view_navigation_fps, &QAction::triggered, &viewport.navigation, &Navigation::startFpsNavigation);

  import_pointcloud_layers->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
  connect(import_pointcloud_layers, &QAction::triggered, this, &MainWindow::importPointcloudLayer);

  QMenu* menu_view_application = menuBar->addMenu("&Application");
  QAction* about_action = menu_view_application->addAction("&About");
  connect(about_action, &QAction::triggered, this, &MainWindow::openAboutDialog);

}

void MainWindow::insertKeypoint()
{
  const int position_after_last = std::numeric_limits<int>::max();
  int position = position_after_last;

  flythrough.insert_keypoint(viewport.navigation.camera.frame, position);
}

#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPlainTextEdit>
#include <QApplication>
#include <QScrollBar>

void MainWindow::importPointcloudLayer()
{
  QString file_to_import = QFileDialog::getOpenFileName(this,
                                                         "Select pointcloud to import",
                                                         ".",
                                                         AbstractPointCloudImporter::allSupportedFiletypes());

  if(file_to_import.isEmpty())
    return;

  viewport.load_point_cloud(import_point_cloud(this, file_to_import));
}

extern const QString pcl_notes;
extern const QString pcl_license;

void MainWindow::openAboutDialog()
{
  QDialog aboutDialog;
  aboutDialog.setModal(true);
  aboutDialog.setSizeGripEnabled(true);

  QVBoxLayout* layout = new QVBoxLayout;
  aboutDialog.setLayout(layout);

  auto add_note = [&](QString info, QString details){
    QLabel* label = new QLabel(info);
    QPlainTextEdit* plain_text_edit = new QPlainTextEdit(details);

    plain_text_edit->setWordWrapMode(QTextOption::NoWrap);
    plain_text_edit->setReadOnly(true);

    layout->addWidget(label);
    layout->addWidget(plain_text_edit);
    layout->addSpacing(22);
  };

  layout->addWidget(new QLabel("Dependencies:"));
  layout->addSpacing(22);

  add_note(pcl_notes, pcl_license);

  aboutDialog.exec();
}

const QString pcl_notes = "The Point Cloud Library (PCL) is used for:\n"
                          "- Loading PLY files";
const QString pcl_license = "Software License Agreement (BSD License)\n"
                            "\n"
                            "Point Cloud Library (PCL) - www.pointclouds.org\n"
                            "Copyright (c) 2009-2012, Willow Garage, Inc.\n"
                            "Copyright (c) 2012-, Open Perception, Inc.\n"
                            "Copyright (c) XXX, respective authors.\n"
                            "\n"
                            "All rights reserved.\n"
                            "\n"
                            "Redistribution and use in source and binary forms, with or without\n"
                            "modification, are permitted provided that the following conditions\n"
                            "are met: \n"
                            "\n"
                            " * Redistributions of source code must retain the above copyright\n"
                            "   notice, this list of conditions and the following disclaimer.\n"
                            " * Redistributions in binary form must reproduce the above\n"
                            "   copyright notice, this list of conditions and the following\n"
                            "   disclaimer in the documentation and/or other materials provided\n"
                            "   with the distribution.\n"
                            " * Neither the name of the copyright holder(s) nor the names of its\n"
                            "   contributors may be used to endorse or promote products derived\n"
                            "   from this software without specific prior written permission.\n"
                            "\n"
                            "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n"
                            "\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n"
                            "LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS\n"
                            "FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE\n"
                            "COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,\n"
                            "INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,\n"
                            "BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n"
                            "LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n"
                            "CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n"
                            "LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN\n"
                            "ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n"
                            "POSSIBILITY OF SUCH DAMAGE.\n";