#ifndef DIALOG_CCP_
#define DIALOG_CCP_

#include <QtGui>

 #include "dialog.h"

 #define MESSAGE \
     Dialog::tr("<p>Message boxes have a caption, a text, " \
                "and any number of buttons, each with standard or custom texts." \
                "<p>Click a button to close the message box. Pressing the Esc button " \
                "will activate the detected escape button (if any).")

 Dialog::Dialog(QWidget *parent)
     : QDialog(parent)
 {
     errorMessageDialog = new QErrorMessage(this);

     int frameStyle = QFrame::Sunken | QFrame::Panel;
     integerLabel = new QLabel;
     integerLabel->setFrameStyle(frameStyle);
     QPushButton *integerButton =
             new QPushButton(tr("QInputDialog::get&Integer()"));

     doubleLabel = new QLabel;
     doubleLabel->setFrameStyle(frameStyle);
     QPushButton *doubleButton =
             new QPushButton(tr("QInputDialog::get&Double()"));

     itemLabel = new QLabel;
     itemLabel->setFrameStyle(frameStyle);
     QPushButton *itemButton = new QPushButton(tr("QInputDialog::getIte&m()"));

     textLabel = new QLabel;
     textLabel->setFrameStyle(frameStyle);
     QPushButton *textButton = new QPushButton(tr("QInputDialog::get&Text()"));

     colorLabel = new QLabel;
     colorLabel->setFrameStyle(frameStyle);
     QPushButton *colorButton = new QPushButton(tr("QColorDialog::get&Color()"));

     fontLabel = new QLabel;
     fontLabel->setFrameStyle(frameStyle);
     QPushButton *fontButton = new QPushButton(tr("QFontDialog::get&Font()"));

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
     layout->addWidget(integerButton, 0, 0);
     layout->addWidget(integerLabel, 0, 1);
     layout->addWidget(doubleButton, 1, 0);
     layout->addWidget(doubleLabel, 1, 1);
     layout->addWidget(itemButton, 2, 0);
     layout->addWidget(itemLabel, 2, 1);
     layout->addWidget(textButton, 3, 0);
     layout->addWidget(textLabel, 3, 1);
     layout->addWidget(colorButton, 4, 0);
     layout->addWidget(colorLabel, 4, 1);
     layout->addWidget(fontButton, 5, 0);
     layout->addWidget(fontLabel, 5, 1);
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

 void Dialog::setInteger()
 {
     bool ok;
     int i = QInputDialog::getInteger(this, tr("QInputDialog::getInteger()"),
                                      tr("Percentage:"), 25, 0, 100, 1, &ok);
     if (ok)
         integerLabel->setText(tr("%1%").arg(i));
 }

 void Dialog::setDouble()
 {
     bool ok;
     double d = QInputDialog::getDouble(this, tr("QInputDialog::getDouble()"),
                                        tr("Amount:"), 37.56, -10000, 10000, 2, &ok);
     if (ok)
         doubleLabel->setText(QString("$%1").arg(d));
 }

 void Dialog::setItem()
 {
     QStringList items;
     items << tr("Spring") << tr("Summer") << tr("Fall") << tr("Winter");

     bool ok;
     QString item = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
                                          tr("Season:"), items, 0, false, &ok);
     if (ok && !item.isEmpty())
         itemLabel->setText(item);
 }

 void Dialog::setText()
 {
     bool ok;
     QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                          tr("User name:"), QLineEdit::Normal,
                                          QDir::home().dirName(), &ok);
     if (ok && !text.isEmpty())
         textLabel->setText(text);
 }

 void Dialog::setColor()
 {
     QColor color = QColorDialog::getColor(Qt::green, this);
     if (color.isValid()) {
         colorLabel->setText(color.name());
         colorLabel->setPalette(QPalette(color));
         colorLabel->setAutoFillBackground(true);
     }
 }

 void Dialog::setFont()
 {
     bool ok;
     QFont font = QFontDialog::getFont(&ok, QFont(fontLabel->text()), this);
     if (ok) {
         fontLabel->setText(font.key());
         fontLabel->setFont(font);
     }
 }

 void Dialog::setExistingDirectory()
 {
     QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
     if (!native->isChecked())
         options |= QFileDialog::DontUseNativeDialog;
     QString directory = QFileDialog::getExistingDirectory(this,
                                 tr("QFileDialog::getExistingDirectory()"),
                                 directoryLabel->text(),
                                 options);
     if (!directory.isEmpty())
         directoryLabel->setText(directory);
 }

QString Dialog::setOpenFileName(QString panelName, QString fileType)
 {
     QFileDialog::Options options;
     if (!native->isChecked())
         options |= QFileDialog::DontUseNativeDialog;
     QString selectedFilter;
     QString fileName = QFileDialog::getOpenFileName(this,
                                 tr(panelName.toStdString().c_str()),
                                 openFileNameLabel->text(),
                                 tr(fileType.toStdString().c_str()),
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
                                 this, tr(panelName.toStdString().c_str()),
                                 openFilesPath,
                                 tr(fileType.toStdString().c_str()),
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
                                 tr(panelName.toStdString().c_str()),
                                 saveFileNameLabel->text(),
                                 tr(fileType.toStdString().c_str()),
                                 &selectedFilter,
                                 options);
      return fileName;
 }

 void Dialog::criticalMessage()
 {
     QMessageBox::StandardButton reply;
     reply = QMessageBox::critical(this, tr("QMessageBox::critical()"),
                                     MESSAGE,
                                     QMessageBox::Abort | QMessageBox::Retry | QMessageBox::Ignore);
     if (reply == QMessageBox::Abort)
         criticalLabel->setText(tr("Abort"));
     else if (reply == QMessageBox::Retry)
         criticalLabel->setText(tr("Retry"));
     else
         criticalLabel->setText(tr("Ignore"));
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

 void Dialog::warningMessage()
 {
     QMessageBox msgBox(QMessageBox::Warning, tr("QMessageBox::warning()"),
                        MESSAGE, 0, this);
     msgBox.addButton(tr("Save &Again"), QMessageBox::AcceptRole);
     msgBox.addButton(tr("&Continue"), QMessageBox::RejectRole);
     if (msgBox.exec() == QMessageBox::AcceptRole)
         warningLabel->setText(tr("Save Again"));
     else
         warningLabel->setText(tr("Continue"));

 }

 void Dialog::errorMessage(QString message)
 {
     errorMessageDialog->showMessage(
             tr(message.toStdString().c_str()));
     errorLabel->setText(tr("If the box is unchecked, the message "
                            "won't appear again."));
 } 
 
#endif /*DIALOG_CCP_*/
