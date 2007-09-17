#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

using namespace std;

#include "ui_MPTK_GUI_APP.h"
#include "dialog.h"
#include "../core/gui_callback.h"
#include "../core/gui_callback_demix.h"
#include "gpl.h"



#include <iostream>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>


/* on hérite de la fenêtre créée dans le designer */
class MainWindow: public QMainWindow, private Ui::MainWindow {
	
	

	Q_OBJECT
	
	private:
	MP_Gui_Callback_c * guiCallBack;
	MP_Gui_Callback_Demix_c * guiCallBackDemix;
	Dialog * dialog;
	bool dictOpen;

	
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
		void on_pushButtonSaveBook_clicked();
		void on_pushButtonSaveBookDemix_clicked();
		void on_pushButtonStopIterate_clicked();
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
		public:
		void readFromStdout(QString message);
};

#endif /*MAIN_WINDOW_H_*/
