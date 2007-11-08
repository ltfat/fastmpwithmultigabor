/******************************************************************************/
/*                                                                            */
/*                             main_window.h                                  */
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

/******************************************/
/*                                        */
/* DEFINITION OF THE GUI MAINWINDOW CLASS */
/*                                        */
/******************************************/
#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

using namespace std;

#include "ui_MPTK_GUI_APP.h"
#include <QMetaType>
#include "dialog.h"
#include "../core/gui_callback.h"
#include "../core/gui_callback_demix.h"
#include "../core/gui_callback_demo.h"
#include "../core/gui_callback_abstract.h"
#include "../core/gui_callback_reconstruction.h"
#include "gpl.h"
#include <unistd.h>



#include <iostream>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
/**
 * \brief MainWindow is a class that provides the GUI for MPTK library
 * \note inherit from the window defined in designer Ui::MainWindow and QMainWindow
 */
class MainWindow: public QMainWindow, private Ui::MainWindow
  {
    Q_OBJECT
    // Q_DECLARE_METATYPE(QMetaType::QTextCursor)
    /********/
    /* DATA */
    /********/
  private:
    /**  \brief Pointer on a gui callback for mpd  */
    MP_Gui_Callback_c * guiCallBack;
    /**  \brief Pointer on a gui callback for mpd demix  */
    MP_Gui_Callback_Demix_c * guiCallBackDemix;
    /**  \brief Pointer on a gui callback for mpd demo  */
    MP_Gui_Callback_Demo_c *guiCallBackDemo;
    /**  \brief Pointer on a gui callback for mpr */
    MP_Gui_Callback_Reconstruct_c *guiCallBackReconstruct;
    /**  \brief Pointer on map vector to create the customs blocks   */
    vector<map< string, string, mp_ltstring>*> * customBlockMapVector;
    /**  \brief Pointer on a dialog class to show message ti the user   */
    Dialog * dialog;
    /**  \brief Boolean to check if a dict is open   */
    bool dictOpen;
    /**  \brief Boolean to check if a dict is open for demo  */
    bool dictOpenDemo;
    /**  \brief Boolean to check if a default dict is open for demo   */
    bool dictOpenDemoDefault;
    /**  \brief Boolean to check if a custom dict is created for demo   */
    bool dictOpenDemoCustom;
    /**  \brief Boolean to indicate if the stop contrait has been set   */
    bool stopContraintSet;


    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/

  public:
    /**  \brief Constructor */
    MainWindow(QWidget *parent = 0);
    /**  \brief Destructor */
    virtual ~MainWindow();
    /***********/
    /* SLOTS   */
    /***********/

  private:
    /**  \brief A slot is called when a signal connected to it is emitted.
     *  Slots are normal C++ functions and can be called normally;
     *  their only special feature is that signals can be connected to them. 
     * A slot's arguments cannot have default values, and, like signals,
     *  it is rarely wise to use your own custom types for slot arguments.   */
  private slots:
    /**  \brief A slot */
    void on_btnPlay_clicked();
    /**  \brief A slot */
    void on_btnPlayDemix_clicked();
    /**  \brief A slot */
    void on_btnStop_clicked();
    /**  \brief A slot */
    void on_btnOpenSig_clicked();
    /**  \brief A slot */
    void on_btnOpenSigRecons_clicked();
    /**  \brief A slot */
    void on_btnOpenDict_clicked();
    /**  \brief A slot */
    void on_btnOpenbook_clicked();
    /**  \brief A slot */
    void on_pushButtonIterateOnce_clicked();
    /**  \brief A slot */
    void on_pushButtonIterateAll_clicked();
    /**  \brief A slot */
    void on_pushButtonreconSignal_clicked();
    /**  \brief A slot */
    void on_comboBoxNumIter_activated();
    /**  \brief A slot */
    void on_comboBoxNumIterDemix_activated();
    /**  \brief A slot */
    void on_comboBoxSnr_activated();
    /**  \brief A slot */
    void on_comboBoxSnrDemix_activated();
    /**  \brief A slot */
    void on_comboBoxSnrDemo_activated();
    /**  \brief A slot */
    void on_comboBoxNumIterDemo_activated();
    /**  \brief A slot */
    void on_pushButtonSaveBook_clicked();
    /**  \brief A slot */
    void on_pushButtonSaveBookDemix_clicked();
    /**  \brief A slot */
    void on_pushButtonSaveResidual_clicked();
    /**  \brief A slot */
    void on_pushButtonSaveResidualDemix_clicked();
    /**  \brief A slot */
    void on_pushButtonSaveDecayDemix_clicked();
    /**  \brief A slot */
    void on_pushButtonSaveDecay_clicked();
    /**  \brief A slot */
    void on_comboBoxNumIterSavehit_activated();
    /**  \brief A slot */
    void on_comboBoxNumIterSavehitDemix_activated();
    /**  \brief A slot */
    void on_pushButtonSaveApprox_clicked();
    /**  \brief A slot */
    void on_pushButtonSaveApproxDemix_clicked();
    /**  \brief A slot */
    void on_pushButtonSaveReconstruc_clicked();
    /**  \brief A slot */
    void on_pushButtonSaveApproxRecons_clicked();
    /**  \brief A slot */
    void on_tabWidget_currentChanged();
    /**  \brief A slot */
    void on_btnOpenMixer_clicked();
    /**  \brief A slot */
    void on_btnOpenSigDemix_clicked();
    /**  \brief A slot */
    void on_btnOpenDictDemix_clicked();
    /**  \brief A slot */
    void on_pushButtonIterateAllDemix_clicked();
    /**  \brief A slot */
    void on_radioButtonVerbose_toggled();
    /**  \brief A slot */
    void on_radioButtonVerboseDemix_toggled();
    /**  \brief A slot */
    void on_pushButtonSaveSequenceDemix_clicked();
    /**  \brief A slot */
    void on_btnOpenDefaultSig_clicked();
    /**  \brief A slot */
    void on_btnValidateDefautlDict_clicked();
    /**  \brief A slot */
    void on_btnLauchDemo_clicked();
    /**  \brief A slot */
    void on_btnPlayDemo_clicked();
    /**  \brief A slot */
    void on_btnStopDemo_clicked();
    /**  \brief A slot */
    void on_btnStopRecons_clicked();
    /**  \brief A slot */
    void on_btnStopDemix_clicked();
    /**  \brief A slot */
    void on_btnPlayRecons_clicked();
    /**  \brief A slot */
    void on_btnOpenSigDemo_clicked();
    /**  \brief A slot */
    void on_btnOpenDictDemo_clicked();
    /**  \brief A slot */
    void on_btnDecomposeDemo_clicked();
    /**  \brief A slot */
    void on_btnValidateCustomDict_clicked();
    /**  \brief A slot */
    void on_btnSaveCustomDict_clicked();
    /**  \brief A slot */
    void on_lineEditCustomBlock1WindowLen_textEdited();
    /**  \brief A slot */
    void on_lineEditCustomBlock1FftSize_textEdited();
    /**  \brief A slot */
    void on_lineEditCustomBlock2WindowLen_textEdited();
    /**  \brief A slot */
    void on_lineEditCustomBlock2FftSize_textEdited();
    /**  \brief A slot */
    void on_lineEditCustomBlock1WindowLenSec_textEdited();
    /**  \brief A slot */
    void on_lineEditCustomBlock2WindowLenSec_textEdited();
    /**  \brief A slot */
    void on_pushButtonStopIterate_clicked();
    /**  \brief A slot */
    void on_btnOpenDefaultMixerDemix_clicked();
    /**  \brief A slot */
    void on_lineEditNumIter_textEdited();
    /**  \brief A slot */
    void on_lineEditSNR_textEdited();
    /**  \brief A slot */
    void on_btnOpenDefaultSigDemix_clicked();
    
  public slots:
    /**  \brief A slot */
    void iteration_running(bool status);
    /**  \brief A slot */
    void iteration_running_demix(bool status);
    /**  \brief A slot */
    void iteration_running_demo(bool status);
     /**  \brief A slot */
    void reconstruction_running(bool status);
    /**  \brief A slot */
    void displayOnConsol(char* message);
    /**  \brief A slot */
    void displayOnWarning(char* message);
    /**  \brief A slot */
    void displayOnError(char* message);
    /**  \brief A slot */
    void displayOnConsolDemix(char* message);
    /**  \brief A slot */
    void displayOnConsolDemo(char* message);
    /**  \brief A slot */
    void displayOnWarningDemo(char* message);
    /**  \brief A slot */
    void displayOnWarningDemix(char* message);
    /**  \brief A slot */
    void displayOnErrorDemix(char* message);
    /**  \brief A slot */
    void displayOnErrorDemo(char* message);
    /**  \brief A slot */
    void displayOnConsolRecons(char* message);
    /**  \brief A slot */
    void displayOnWarningRecons(char* message);
    /**  \brief A slot */
    void displayOnErrorRecons(char* message);

  };

#endif /*MAIN_WINDOW_H_*/
