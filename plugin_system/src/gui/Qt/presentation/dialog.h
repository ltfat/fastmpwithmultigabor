#ifndef DIALOG_H_
#define DIALOG_H_

#include <QDialog>

 class QCheckBox;
 class QLabel;
 class QErrorMessage;

 class Dialog : public QDialog
 {
     Q_OBJECT

 public:
     Dialog(QWidget *parent = 0);

 public slots:
     void setInteger();
     void setDouble();
     void setItem();
     void setText();
     void setColor();
     void setFont();
     void setExistingDirectory();
     QString setOpenFileName(QString panelName, QString fileType);
     QStringList setOpenFileNames(QString panelName, QString fileType);
     QString setSaveFileName(QString panelName, QString fileType);
     void criticalMessage();
     void informationMessage(QString message);
     int questionMessage(QString message);
     void warningMessage();
     void errorMessage(QString message);

 private:
     QCheckBox *native;
     QLabel *integerLabel;
     QLabel *doubleLabel;
     QLabel *itemLabel;
     QLabel *textLabel;
     QLabel *colorLabel;
     QLabel *fontLabel;
     QLabel *directoryLabel;
     QLabel *openFileNameLabel;
     QLabel *openFileNamesLabel;
     QLabel *saveFileNameLabel;
     QLabel *criticalLabel;
     QLabel *informationLabel;
     QLabel *questionLabel;
     QLabel *warningLabel;
     QLabel *errorLabel;
     QErrorMessage *errorMessageDialog;

     QString openFilesPath;
 };
 
#endif /*DIALOG_H_*/
