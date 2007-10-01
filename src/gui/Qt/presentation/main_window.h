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
#include "dialog.h"
#include "../core/gui_callback.h"
#include "../core/gui_callback_demix.h"
#include "../core/gui_callback_demo.h"
#include "gpl.h"
#include <unistd.h>



#include <iostream>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>


/* Class MainWindow inherit from the window defined in designer */
class MainWindow: public QMainWindow, private Ui::MainWindow {
	Q_OBJECT
	
	private:
	MP_Gui_Callback_c * guiCallBack;
	MP_Gui_Callback_Demix_c * guiCallBackDemix;
	MP_Gui_Callback_Demo_c *guiCallBackDemo;
	Dialog * dialog;
	bool dictOpen;
	bool dictOpenDemo;
    bool dictOpenDemoDefault;
    bool stopContraintSet;
	
	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();
	public slots:
		void reader_thread_update();
		void reader_thread_ok();
		void reader_thread_exception();
	private:
	private slots:
		void on_btnPlay_clicked();
		void on_btnPlayDemix_clicked();
		void on_btnStop_clicked();
		void on_btnOpenSig_clicked();
		void on_btnOpenDict_clicked();
		void on_btnOpenbook_clicked();
		void on_pushButtonIterateOnce_clicked();
		void on_pushButtonIterateAll_clicked();
		void on_comboBoxNumIter_activated();
		void on_comboBoxNumIterDemix_activated();
		void on_comboBoxSnr_activated();
		void on_comboBoxSnrDemix_activated();
		void on_comboBoxSnrDemo_activated();
		void on_comboBoxNumIterDemo_activated();
		void on_pushButtonSaveBook_clicked();
		void on_pushButtonSaveBookDemix_clicked();
		void on_pushButtonSaveResidual_clicked();
		void on_pushButtonSaveResidualDemix_clicked();
		void on_pushButtonSaveDecayDemix_clicked();
		void on_pushButtonSaveDecay_clicked();
		void on_comboBoxNumIterSavehit_activated();
		void on_comboBoxNumIterSavehitDemix_activated();
		void on_pushButtonSaveApprox_clicked();
		void on_pushButtonSaveApproxDemix_clicked();
		void on_tabWidget_currentChanged();
		void on_btnOpenMixer_clicked();
		void on_btnOpenSigDemix_clicked();
		void on_btnOpenDictDemix_clicked();
		void on_pushButtonIterateAllDemix_clicked();
		void on_radioButtonVerbose_toggled();
		void on_radioButtonVerboseDemix_toggled();
		void on_pushButtonSaveSequenceDemix_clicked();
		void on_btnOpenDefaultSig_clicked();
		void on_btnValidateDefautlDict_clicked();
		void on_btnLauchDemo_clicked();
		void on_btnPlayDemo_clicked();
		void on_btnStopDemo_clicked();
		void on_btnStopDemix_clicked();
		void on_btnOpenSigDemo_clicked();
		void on_btnOpenDictDemo_clicked();
		void on_horizontalScrollBarDemo_valueChanged();
		public:
		void readFromStdout(QString message);
};

#endif /*MAIN_WINDOW_H_*/
