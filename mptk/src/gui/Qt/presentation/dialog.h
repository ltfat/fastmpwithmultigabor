/******************************************************************************/
/*                                                                            */
/*                               dialog.h                                     */
/*                                                                            */
/*                        Matching Pursuit Library                            */
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

/****************************************/
/*                                      */
/* DEFINITION OF THE DIALOG CLASS       */
/*                                      */
/****************************************/

#ifndef DIALOG_H_
#define DIALOG_H_

#include <QDialog>

/***********************/
/* PRE DECLARATIONS    */
/***********************/
class QCheckBox;
class QLabel;
class QErrorMessage;

/**
 * \brief
 * Dialog class offers GUI utilities to dialog with the user
 */
class Dialog : public QDialog
  {
    Q_OBJECT

    /********/
    /* DATA */
    /********/
  private:
    QCheckBox *native;
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

    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
    /**  \brief Constructor */
  public:
    Dialog(QWidget *parent = 0);
    /**  \brief Destructor */
    virtual ~Dialog();

    /***********/
    /* SLOTS   */
    /***********/
  public slots:
    /** \brief Slot to display a panel in order to find file name to open
    *  \param panelName: name of the panel to display
    *  \param fileType: the file type to open in string
    *  \return QSting : the name of the found file
    */
    QString setOpenFileName(QString panelName, QString fileType);
    /** \brief Slot to display a panel in order to find multiple files names to open
    *  \param panelName: name of the panel to display
    *  \param fileType: the file type to open in string
    *  \return QSting : the name of the found file
    */
    QStringList setOpenFileNames(QString panelName, QString fileType);
    /** \brief Slot to display a panel in order to find file name for saving
    *  \param panelName: name of the panel to display
    *  \param fileType: the file type to open in string
    *  \return QSting : the name of the found file
    */
    QString setSaveFileName(QString panelName, QString fileType);
    /** \brief Slot to display a critical Message
    *  \param message: message to display
    */
    int criticalMessage(QString message);
    /** \brief Slot to display a information Message
    *  \param message: message to display
    */
    void informationMessage(QString message);
    /** \brief Slot to display a question Message
    *  \param message: message to display
    */
    int questionMessage(QString message);
    /** \brief Slot to display a warning Message
    *  \param message: message to display
    */
    void warningMessage(QString message);
    /** \brief Slot to display a error Message
    *  \param message: message to display
    */
    void errorMessage(QString message);

  };

#endif /*DIALOG_H_*/
