/******************************************************************************/
/*                                                                            */
/*                               dialog.cpp                                   */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/*                                                                            */
/* Roy Benjamin                                               Mon Feb 21 2007 */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/*  Copyright (C) 2005 IRISA                                                  */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or             */
/*  modify it under the terms of the GNU General Public License               */
/*  as published by the Free Software Foundation; either version 2            */
/*  of the License, or (at your option) any later version.                    */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place - Suite 330,                            */
/*  Boston, MA  02111-1307, USA.                                              */
/*                                                                            */
/******************************************************************************/

/**********************************************************/
/*                                                        */
/* dialog.cpp : methods for class MainWindow              */
/*                                                        */
/**********************************************************/

#ifndef DIALOG_CCP_
#define DIALOG_CCP_

#include <QtGui>

#include "dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
  errorMessageDialog = new QErrorMessage(this);

  int frameStyle = QFrame::Sunken | QFrame::Panel;

  directoryLabel = new QLabel;
  directoryLabel->setFrameStyle(frameStyle);
  QPushButton *directoryButton =
    new QPushButton(tr("QFileDialog::getE&xistingDirectory()"));

  openFileNameLabel = new QLabel;
  openFileNameLabel->setFrameStyle(frameStyle);
  QPushButton *openFileNameButton =
    new QPushButton(tr("QFileDialog::get&OpenFileName()"));

  openFileNamesLabel = new QLabel;
  openFileNamesLabel->setFrameStyle(frameStyle);
  QPushButton *openFileNamesButton =
    new QPushButton(tr("QFileDialog::&getOpenFileNames()"));

  saveFileNameLabel = new QLabel;
  saveFileNameLabel->setFrameStyle(frameStyle);
  QPushButton *saveFileNameButton =
    new QPushButton(tr("QFileDialog::get&SaveFileName()"));

  criticalLabel = new QLabel;
  criticalLabel->setFrameStyle(frameStyle);
  QPushButton *criticalButton =
    new QPushButton(tr("QMessageBox::critica&l()"));

  informationLabel = new QLabel;
  informationLabel->setFrameStyle(frameStyle);
  QPushButton *informationButton =
    new QPushButton(tr("QMessageBox::i&nformation()"));

  questionLabel = new QLabel;
  questionLabel->setFrameStyle(frameStyle);
  QPushButton *questionButton =
    new QPushButton(tr("QMessageBox::&question()"));

  warningLabel = new QLabel;
  warningLabel->setFrameStyle(frameStyle);
  QPushButton *warningButton = new QPushButton(tr("QMessageBox::&warning()"));

  errorLabel = new QLabel;
  errorLabel->setFrameStyle(frameStyle);
  QPushButton *errorButton =
    new QPushButton(tr("QErrorMessage::show&M&essage()"));

  native = new QCheckBox(this);
  native->setText("Use native file dialog.");
  native->setChecked(true);
#ifndef Q_WS_WIN
#ifndef Q_OS_MAC
  native->hide();
#endif
#endif
  QGridLayout *layout = new QGridLayout;
  layout->setColumnStretch(1, 1);
  layout->setColumnMinimumWidth(1, 250);
  layout->addWidget(directoryButton, 6, 0);
  layout->addWidget(directoryLabel, 6, 1);
  layout->addWidget(openFileNameButton, 7, 0);
  layout->addWidget(openFileNameLabel, 7, 1);
  layout->addWidget(openFileNamesButton, 8, 0);
  layout->addWidget(openFileNamesLabel, 8, 1);
  layout->addWidget(saveFileNameButton, 9, 0);
  layout->addWidget(saveFileNameLabel, 9, 1);
  layout->addWidget(criticalButton, 10, 0);
  layout->addWidget(criticalLabel, 10, 1);
  layout->addWidget(informationButton, 11, 0);
  layout->addWidget(informationLabel, 11, 1);
  layout->addWidget(questionButton, 12, 0);
  layout->addWidget(questionLabel, 12, 1);
  layout->addWidget(warningButton, 13, 0);
  layout->addWidget(warningLabel, 13, 1);
  layout->addWidget(errorButton, 14, 0);
  layout->addWidget(errorLabel, 14, 1);
  layout->addWidget(native, 15, 0);
  setLayout(layout);

  setWindowTitle(tr("Standard Dialogs"));
}
Dialog::~Dialog()
{}

QString Dialog::setOpenFileName(QString panelName, QString fileType)
{
  QFileDialog::Options options;
  if (!native->isChecked())
    options |= QFileDialog::DontUseNativeDialog;
  QString selectedFilter;
  QString fileName = QFileDialog::getOpenFileName(this,
                     tr(panelName.toAscii().constData()),
                     openFileNameLabel->text(),
                     tr(fileType.toAscii().constData()),
                     &selectedFilter,
                     options);
  return fileName;

}

QStringList Dialog::setOpenFileNames(QString panelName, QString fileType)
{
  QFileDialog::Options options;
  if (!native->isChecked())
    options |= QFileDialog::DontUseNativeDialog;
  QString selectedFilter;
  QStringList files = QFileDialog::getOpenFileNames(
                        this, tr(panelName.toAscii().constData()),
                        openFilesPath,
                        tr(fileType.toAscii().constData()),
                        &selectedFilter,
                        options);
  return files;
}

QString Dialog::setSaveFileName(QString panelName, QString fileType)
{
  QFileDialog::Options options;
  if (!native->isChecked())
    options |= QFileDialog::DontUseNativeDialog;
  QString selectedFilter;
  QString fileName = QFileDialog::getSaveFileName(this,
                     tr(panelName.toAscii().constData()),
                     saveFileNameLabel->text(),
                     tr(fileType.toAscii().constData()),
                     &selectedFilter,
                     options);
  return fileName;
}

int Dialog::criticalMessage(QString message)
{
  QMessageBox::StandardButton reply;
  reply = QMessageBox::critical(this, tr("MPTK GUI"),
                                message,
                                QMessageBox::Abort | QMessageBox::Retry | QMessageBox::Ignore);
  if (reply == QMessageBox::Abort)
    return 1;
  else if (reply == QMessageBox::Retry)
    return 2;
  else
    return 3;
}

void Dialog::informationMessage(QString message)
{
  QMessageBox::StandardButton reply;
  reply = QMessageBox::information(this, tr("MPTK GUI"), message);
  if (reply == QMessageBox::Ok)
    informationLabel->setText(tr("OK"));
  else
    informationLabel->setText(tr("Escape"));
}

int Dialog::questionMessage(QString message)
{
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, tr("MPTK GUI"),
                                message,
                                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  if (reply == QMessageBox::Yes)
    return 1;
  else if (reply == QMessageBox::No)
    return 0;
  else if (reply == QMessageBox::Cancel)
    return 2;
}

void Dialog::warningMessage(QString message)
{
  QMessageBox msgBox(QMessageBox::Warning, tr("MPTK GUI"),
                     message, 0, this);
  msgBox.addButton(tr("&Continue"), QMessageBox::AcceptRole);
  msgBox.addButton(tr("&Stop"), QMessageBox::RejectRole);
  if (msgBox.exec() == QMessageBox::AcceptRole)
    warningLabel->setText(tr("Continue"));
  else
    warningLabel->setText(tr("Stop"));

}

void Dialog::errorMessage(QString message)
{
  errorMessageDialog->showMessage(
    tr(message.toAscii().constData()));
  errorLabel->setText(tr("If the box is unchecked, the message "
                         "won't appear again."));
}

#endif /*DIALOG_CCP_*/
