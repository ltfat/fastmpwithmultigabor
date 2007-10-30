/******************************************************************************/
/*                                                                            */
/*                            main_window.cpp                                 */
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
/* atom_factory.cpp : methods for class MainWindow        */
/*                                                        */
/**********************************************************/
#include "main_window.h"


// Constructor
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
  setupUi(this);
  guiCallBack = MP_Gui_Callback_c::get_gui_call_back();
  guiCallBack->setActivated();
  guiCallBackDemix = MP_Gui_Callback_Demix_c::get_gui_call_back();
  guiCallBackDemo = MP_Gui_Callback_Demo_c::get_gui_call_back();
  dialog  = new Dialog();
  labelOriginalSignal->setText("No wave file selected for original signal");
  labelBook->setText("No book file selected");
  labelDict->setText("No dictionary file selected");
  label_progress->setText("No decompostion");
  label_progressDemix->setText("No decompostion");
  label_progress_Demo->setText("No decompostion");
  textEditConsol->append(gplText);
  textEditConsolDemix->append(gplText);
  textEditConsolDemo->append(gplText);
  dictOpen = false;
  dictOpenDemo = false;
  dictOpenDemoDefault = false;
  dictOpenDemoCustom = false;
  stopContraintSet = false;
  connect(pushButtonStopIterate, SIGNAL(clicked()), guiCallBack, SLOT(stopIteration()), Qt::DirectConnection);
  connect(pushButtonStopIterateDemix, SIGNAL(clicked()), guiCallBackDemix, SLOT(stopIteration()), Qt::DirectConnection);
  connect(guiCallBack, SIGNAL(runningIteration(bool)), this, SLOT(iteration_running(bool)));
  connect(guiCallBackDemix, SIGNAL(runningIteration(bool)), this, SLOT(iteration_running_demix(bool)));
  connect(MP_Gui_Callback_Demo_c::get_gui_call_back(), SIGNAL(runningIteration(bool)), this, SLOT(iteration_running_demo(bool)));
  connect(guiCallBack, SIGNAL(infoMessage(char*)), this, SLOT(displayOnConsol(char*)));
  connect(guiCallBack, SIGNAL(errorMessage(char*)), this, SLOT(displayOnError(char*)));
  connect(guiCallBack, SIGNAL(warningMessage(char*)), this, SLOT(displayOnWarning(char*)));
  connect(guiCallBackDemix, SIGNAL(infoMessage(char*)), this, SLOT(displayOnConsolDemix(char*)));
  connect(guiCallBackDemix, SIGNAL(errorMessage(char*)), this, SLOT(displayOnErrorDemix(char*)));
  connect(guiCallBackDemix, SIGNAL(warningMessage(char*)), this, SLOT(displayOnWarningDemix(char*)));
  connect(guiCallBackDemo, SIGNAL(infoMessage(char*)), this, SLOT(displayOnConsolDemo(char*)));
  connect(guiCallBackDemo, SIGNAL(errorMessage(char*)), this, SLOT(displayOnErrorDemo(char*)));
  connect(guiCallBackDemo, SIGNAL(warningMessage(char*)), this, SLOT(displayOnWarningDemo(char*)));
  MP_Msg_Server_c::get_msg_server()->register_display_function("info_message_display",&MP_Gui_Callback_c::emitInfoMessage);
  MP_Msg_Server_c::get_msg_server()->register_display_function("error_message_display",&MP_Gui_Callback_c::emitErrorMessage);
  MP_Msg_Server_c::get_msg_server()->register_display_function("warning_message_display",&MP_Gui_Callback_c::emitWarningMessage);
  groupBox_27->hide();
}



// Destructor
MainWindow::~MainWindow()
{
  if (guiCallBack)
    {
      delete guiCallBack;

      guiCallBack = NULL;
    }
  if (guiCallBackDemix)
    {
      delete guiCallBackDemix;
      guiCallBackDemix = NULL;
    }
  if (guiCallBackDemo)
    {
      delete guiCallBackDemo;
      guiCallBackDemo = NULL;
    }

}

void MainWindow::displayOnConsol(char* message){
textEditConsol->append( QString(message));
textEditConsol->update(); 
}

void MainWindow::displayOnConsolDemix(char* message){
textEditConsolDemix->append( QString(message));
textEditConsolDemix->update(); 
}

void MainWindow::displayOnConsolDemo(char* message){
textEditConsolDemo->append( QString(message));
textEditConsolDemo->update(); 
}

void MainWindow::displayOnWarning(char* message){
dialog->warningMessage(QString(message));
}

void MainWindow::displayOnWarningDemix(char* message){
dialog->warningMessage(QString(message));
}
void MainWindow::displayOnWarningDemo(char* message){
dialog->warningMessage(QString(message));
}
void MainWindow::displayOnError(char* message){
dialog->errorMessage(QString(message));
}
void MainWindow::displayOnErrorDemix(char* message){
dialog->errorMessage(QString(message));
}
void MainWindow::displayOnErrorDemo(char* message){
dialog->errorMessage(QString(message));
}
// When user change the current tab
void MainWindow::on_tabWidget_currentChanged()
{
  if (tabWidget->currentIndex()== 1)
    {
      if (guiCallBack->getActivated()) guiCallBack->setDesactivated();
      if (guiCallBackDemo->getActivated())guiCallBackDemo->setDesactivated();
        
      guiCallBackDemix->setActivated();
      MP_Msg_Server_c::get_msg_server()->register_display_function("info_message_display",&MP_Gui_Callback_Demix_c::emitInfoMessage);
     MP_Msg_Server_c::get_msg_server()->register_display_function("error_message_display",&MP_Gui_Callback_Demix_c::emitErrorMessage);
     MP_Msg_Server_c::get_msg_server()->register_display_function("warning_message_display",&MP_Gui_Callback_Demix_c::emitWarningMessage);

      

    }
  else if (tabWidget->currentIndex()== 0)
    {
      if (guiCallBackDemix->getActivated())guiCallBackDemix->setDesactivated();
      if (guiCallBackDemo->getActivated())guiCallBackDemo->setDesactivated();
      guiCallBack->setActivated();
           MP_Msg_Server_c::get_msg_server()->register_display_function("info_message_display",&MP_Gui_Callback_c::emitInfoMessage);
     MP_Msg_Server_c::get_msg_server()->register_display_function("error_message_display",&MP_Gui_Callback_c::emitErrorMessage);
     MP_Msg_Server_c::get_msg_server()->register_display_function("warning_message_display",&MP_Gui_Callback_c::emitWarningMessage);
     
    }
  else if (tabWidget->currentIndex()== 2)
    {
      guiCallBackDemo->setActivated();
    MP_Msg_Server_c::get_msg_server()->register_display_function("info_message_display",&MP_Gui_Callback_Demo_c::emitInfoMessage);
     MP_Msg_Server_c::get_msg_server()->register_display_function("error_message_display",&MP_Gui_Callback_Demo_c::emitErrorMessage);
     MP_Msg_Server_c::get_msg_server()->register_display_function("warning_message_display",&MP_Gui_Callback_Demo_c::emitWarningMessage);
      if (guiCallBackDemix->getActivated()) guiCallBackDemix->setDesactivated();
      if (guiCallBack->getActivated()) guiCallBack->setDesactivated();

    }

}
// play in mpd tab
void MainWindow::on_btnPlay_clicked()
{
  if (NULL!=guiCallBack->signal)
    {
      std::vector<bool> * selectedChannel =  new std::vector<bool>(guiCallBack->signal->numChans, false);
      for (int i = 0 ; i<guiCallBack->signal->numChans ; i++)
        {
          (*selectedChannel)[i] = true;
        }

      if (radioButtonOri->isChecked())guiCallBack->playBaseSignal(selectedChannel,0,0);
      if (radioButtonApprox->isChecked())guiCallBack->playApproximantSignal(selectedChannel,0,0);
      if (radioButtonResi->isChecked())guiCallBack->playResidualSignal(selectedChannel,0,0);
    }
}

// Open sig in Demo tab
void MainWindow::on_btnOpenSigDemo_clicked()
{
  QString panelName = "MPTK GUI: Open Waves files";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  if (!s.isEmpty())
    {
      if (!guiCallBackDemo->coreInit())
        {
          if (guiCallBackDemo->initMpdCore(s, "")== NOTHING_OPENED) dialog->errorMessage("the file named " + s +" isn't a wave file" );
          else labelOriginalSignalDemo->setText(s);
        }
      else
        {
          if (guiCallBackDemo->initMpdCore(s, "")== NOTHING_OPENED) dialog->errorMessage("the file named " + s +" isn't a wave file" );
          else labelOriginalSignalDemo->setText(s);
          dictOpenDemoDefault = false;

        }
    }

  else dialog->errorMessage("Empty name file");


}

// Stop player
void MainWindow::on_btnStop_clicked()
{
  guiCallBack->stopPortAudioStream();
}

void MainWindow::on_btnStopDemix_clicked()
{
  guiCallBackDemix->stopPortAudioStream();
}

void MainWindow::on_btnStopDemo_clicked()
{
  guiCallBackDemo->stopPortAudioStream();

}

/* ouvrir une boîte de dialogue pour sélectionner un fichier */

void MainWindow::on_btnOpenSig_clicked()
{
  QString panelName = "MPTK GUI: Open Waves files";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  if (!s.isEmpty())
    {
      if (guiCallBack->getBookOpen() == 1)
        {
          if (guiCallBack->initMpdCore(s, labelBook->text())== NOTHING_OPENED) dialog->errorMessage("the file named " + s +" isn't a wave file" );
          else labelOriginalSignal->setText(s);
        }
      else
        {
          if (guiCallBack->initMpdCore(s, "")== NOTHING_OPENED) dialog->errorMessage("the file named " + s +" isn't a wave file" );
          else labelOriginalSignal->setText(s);
        }
    }
  else dialog->errorMessage("Empty name file");


  return;
}

/* ouvrir une boîte de dialogue pour sélectionner un fichier */

void MainWindow::on_btnOpenDict_clicked()
{
  QString panelName = "MPTK GUI: Open dictionary";
  QString fileType ="XML Files (*.xml);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  if (guiCallBack->coreInit())
    {
      if (!s.isEmpty())
        {
          if (guiCallBack->coreInit())guiCallBack->setDictionary(s);
          labelDict->setText(s);
          dictOpen = true;
        }
      else dialog->errorMessage("Empty name file");
    }
  else dialog->errorMessage("Open a signal first");

  return;
}

void MainWindow::on_pushButtonIterateOnce_clicked()
{
  if (guiCallBack->coreInit()&&dictOpen)guiCallBack->iterateOnce();

}

void MainWindow::on_comboBoxNumIter_activated()
{ char buf[32];
  sprintf(buf, "%lu",comboBoxNumIter->currentText().toULong());
  lineEditNumIter->setText(buf);
  if (guiCallBack->coreInit())guiCallBack->setIterationNumber(comboBoxNumIter->currentText().toULong());
  if (guiCallBack->coreInit())guiCallBack->unsetSNR();
}

void MainWindow::on_comboBoxNumIterDemix_activated()
{
  if (guiCallBackDemix->coreInit())guiCallBackDemix->setIterationNumber(comboBoxNumIterDemix->currentText().toULong());
  if (guiCallBackDemix->coreInit())guiCallBackDemix->unsetSNR();
}

void MainWindow::on_comboBoxNumIterDemo_activated()
{
  if (guiCallBackDemo->coreInit())guiCallBackDemo->setIterationNumber(comboBoxNumIterDemo->currentText().toULong());
  if (guiCallBackDemo->coreInit())guiCallBackDemo->unsetSNR();
  stopContraintSet = true;
}

void MainWindow::on_comboBoxSnr_activated()
{
  char buf[32];
  sprintf(buf, "%f",comboBoxNumIter->currentText().toULong());
  lineEditSNR->setText(buf);
  if (guiCallBack->coreInit())guiCallBack->setSNR(comboBoxSnr->currentText().toDouble());
  if (guiCallBack->coreInit())guiCallBack->unsetIter();
}

void MainWindow::on_comboBoxSnrDemix_activated()
{
  if (guiCallBackDemix->coreInit())guiCallBackDemix->setSNR(comboBoxSnrDemix->currentText().toDouble());
  if (guiCallBackDemix->coreInit())guiCallBackDemix->unsetIter();
}

void MainWindow::on_comboBoxSnrDemo_activated()
{
  if (guiCallBackDemo->coreInit())guiCallBackDemo->setSNR(comboBoxSnrDemo->currentText().toDouble());
  if (guiCallBackDemo->coreInit())guiCallBackDemo->unsetIter();
  stopContraintSet = true;

}



void MainWindow::on_pushButtonIterateAll_clicked()
{ pushButtonIterateAll->setText("Iterate all");
  if (guiCallBack->coreInit()&&dictOpen)guiCallBack->iterateAll();
  else dialog->errorMessage("parameter not correctly set");
  
}

void MainWindow::on_pushButtonSaveBook_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the book";
  QString fileType ="BIN Files (*.bin);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveBook->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBack->coreInit())guiCallBack->saveBook(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_pushButtonSaveResidual_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the residual file";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveResidual->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBack->coreInit())guiCallBack->saveResidual(s);
    }
  else dialog->errorMessage("Empty name file");

}


void MainWindow::on_pushButtonSaveDecay_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the residual file";
  QString fileType ="text Files (*.txt);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveDecay->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBack->coreInit())guiCallBack->saveDecay(s);
    }
  else dialog->errorMessage("Empty name file");
}

void MainWindow::on_comboBoxNumIterSavehit_activated()
{
  guiCallBack->setSave(comboBoxNumIterSavehit->currentText().toULong(),lineEditSaveBook->displayText(),lineEditSaveResidual->displayText(),lineEditSaveDecay->displayText());

}

void MainWindow::on_comboBoxNumIterSavehitDemix_activated()
{
  guiCallBackDemix->setSave(comboBoxNumIterSavehitDemix->currentText().toULong(),lineEditSaveBookDemix->displayText(),lineEditSaveResidualDemix->displayText(),lineEditSaveDecayDemix->displayText(), lineEditSaveSequenceDemix->displayText());
}
void MainWindow::on_pushButtonSaveApprox_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the approximant file";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveApproximant->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBack->coreInit())guiCallBack->saveApproximant(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_pushButtonSaveApproxDemix_clicked()
{

  QString panelName = "MPTK GUI: type a name for saving the approximant file";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveApproximantDemix->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBackDemix->coreInit())guiCallBackDemix->saveApprox(s);
    }
  else dialog->errorMessage("Empty name file");
}

void MainWindow::on_btnOpenbook_clicked()
{
  QString panelName = "MPTK GUI: Choose a book to open";
  QString fileType ="BIN Files (*.bin);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  int answer =0;
  if (!s.isEmpty())
    {
      if (guiCallBack->getSignalOpen()==0) answer = dialog->questionMessage("Do you want to rebuild the book and substract atom from original signal?");

      if (answer == 2) dialog->errorMessage("Operation canceled");
      else if (answer == 0) guiCallBack->openBook(s);
      else
        {
          if ( answer == 1)
            {
              guiCallBack->openBook(s);
              guiCallBack->subAddBook();
            }

        }

      labelBook->setText(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_pushButtonSaveBookDemix_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the book";
  QString fileType ="BIN Files (*.bin);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveBookDemix->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBackDemix->coreInit())guiCallBackDemix->saveBook(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_btnOpenSigDemix_clicked()
{
  QString panelName = "MPTK GUI: Open Waves files";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  if (guiCallBackDemix->mixer)
    {
      if (!s.isEmpty())
        {
          if (! guiCallBackDemix->openSignal(s) == SIGNAL_OPENED) dialog->errorMessage("Failed to open original signal file");
          guiCallBackDemix->setBookArray();
          guiCallBackDemix->initMpdDemixCore();
          guiCallBackDemix->plugApprox();
          labelOriginalSignalDemix->setText(s);
        }
      else dialog->errorMessage("Empty name file");
    }
  else dialog->errorMessage("Open mixer file first");

  return;
}

void MainWindow::on_btnOpenMixer_clicked()
{
  QString panelName = "MPTK GUI: Choose a mixer file to open";
  QString fileType ="Text Files (*.txt);;XML Files (*.xml);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  if (!s.isEmpty())
    {
      if (guiCallBackDemix->openMixer(s))
        {
          labelMixer->setText(s);
          char buf[3];
          sprintf(buf, "%d", guiCallBackDemix->mixer->numSources);
          labelBookMixeeNbrSources->setText(buf);
        }
      else dialog->errorMessage("Cannot open mixer file");

    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_btnOpenDictDemix_clicked()
{
  QString panelName = "MPTK GUI: Choose a dictionary for each sources";
  QString fileType ="XML Files (*.xml);;All Files (*)";
  QStringList files = dialog->setOpenFileNames(panelName,fileType);
  if (!guiCallBackDemix->mixer) dialog->errorMessage("Open a mixer file first");
  else
    {
      if (files.count() >0 && files.count()==1)
        {
          for (unsigned int i =0; i< guiCallBackDemix->mixer->numSources;i++)
            guiCallBackDemix->addDictToArray(files.at(0),i);
        }
      else
        {
          if (files.count() >0 && files.count()== guiCallBackDemix->mixer->numSources )
            {
              for (unsigned int i =0; i< guiCallBackDemix->mixer->numSources;i++)
                guiCallBackDemix->addDictToArray(files.at(i),i);
            }
          else
            {
              dialog->errorMessage("Please select the same number of dictionary files than the number of sources");
              return;
            }
        }
      guiCallBackDemix->setDictArray();
      labelDictDemix->setText(files.join("/n"));
    }
}

void MainWindow::on_pushButtonIterateAllDemix_clicked()
{
  if (guiCallBackDemix->coreInit()&& guiCallBackDemix->getBookOpen()==BOOK_OPENED)guiCallBackDemix->iterateAll();
}

void MainWindow::on_btnPlayDemix_clicked()
{
  if (NULL!=guiCallBackDemix->signal)
    {
      std::vector<bool> * selectedChannel =  new std::vector<bool>(guiCallBackDemix->signal->numChans, false);
      for (int i = 0 ; i<guiCallBackDemix->signal->numChans ; i++)
        {
          (*selectedChannel)[i] = true;
        }

      if (radioButtonOriDemix->isChecked())guiCallBackDemix->playBaseSignal(selectedChannel,0,0);
      if (radioButtonResiDemix->isChecked())guiCallBackDemix->playResidualSignal(selectedChannel,0,0);
    }

}

void MainWindow::on_pushButtonSaveResidualDemix_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the residual file";
  QString fileType ="Wave Files (*.wav);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  lineEditSaveResidualDemix->setText(s);
  if (!s.isEmpty())
    {
      if (guiCallBackDemix->coreInit())guiCallBackDemix->saveResidual(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_pushButtonSaveDecayDemix_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the decay file";
  QString fileType ="text Files (*.txt);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );

  if (!s.isEmpty())
    {
      if (guiCallBackDemix->coreInit())guiCallBackDemix->saveDecay(s);
      lineEditSaveDecayDemix->setText(s);
    }
  else dialog->errorMessage("Empty name file");

}

void MainWindow::on_pushButtonSaveSequenceDemix_clicked()
{
  QString panelName = "MPTK GUI: type a name for saving the decay file";
  QString fileType ="text Files (*.txt);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  if (!s.isEmpty())lineEditSaveSequenceDemix->setText(s);

}
void MainWindow::on_radioButtonVerbose_toggled()
{
  if (radioButtonVerbose->isChecked ())
    {
      guiCallBack->setVerbose();
      dialog->errorMessage("set verbose");
    }
  else guiCallBack->unSetVerbose();
}

void MainWindow::on_radioButtonVerboseDemix_toggled()
{
  if (radioButtonVerboseDemix->isChecked()) guiCallBackDemix->setVerbose();
  else guiCallBackDemix->unSetVerbose();
}

void MainWindow::on_btnOpenDefaultSig_clicked()
{
  std::string strAppDirectory;
  if (!guiCallBackDemo->coreInit())
    {
#ifdef __WIN32__
      char szAppPath[MAX_PATH] = "";

      GetModuleFileName(NULL, szAppPath, MAX_PATH);

// Extract directory
      strAppDirectory = szAppPath;

      strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind("\\"));
      strAppDirectory += "\\glockenspiel.wav";
      FILE *fp = fopen (strAppDirectory.c_str(), "r");
      if (fp == NULL)
        {
          /* no files*/
          dialog->errorMessage("Cannot open signal file\n Please open a signal file");
        }
      else
        {
          /* files */
          fclose(fp);
          labelOriginalSignalDemo->setText(QString(strAppDirectory.c_str()));
          guiCallBackDemo->initMpdCore(QString(strAppDirectory.c_str()), "");
        }

#else
      char path[2048];
      getcwd(path, 2004);
      strAppDirectory = path;
      strAppDirectory += "/glockenspiel.wav";
      FILE *fp = fopen (strAppDirectory.c_str(), "r");
      if (fp == NULL)
        {
          /* no files*/
          dialog->errorMessage("Cannot open signal file\n Please open a signal file");
        }
      else
        {
          /* files */
          fclose(fp);
          labelOriginalSignalDemo->setText(QString(strAppDirectory.c_str()));
          guiCallBackDemo->initMpdCore(QString(strAppDirectory.c_str()), "");
        }
#endif /* WIN32 */
    }
}

void MainWindow::on_btnValidateDefautlDict_clicked()
{
  std::string strAppDirectory;
  char buf[32];
  char buf1[32];
  char buf2[32];
  char buf3[32];
  if (guiCallBackDemo->coreInit() && !dictOpenDemo)
    {
#ifdef __WIN32__
      char szAppPath[MAX_PATH] = "";

      GetModuleFileName(NULL, szAppPath, MAX_PATH);

      /* Extract directory*/
      strAppDirectory = szAppPath;

      strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind("\\"));
      strAppDirectory += "\\dic_gabor_two_scales.xml";
      FILE *fp = fopen (strAppDirectory.c_str(), "r");
      if (fp == NULL)
        {
          /* no files*/
          dialog->errorMessage("Cannot open dictionary file\n Please open a dictionary or use the custom dictionary");
        }
      else
        {
          /* files */
          fclose(fp);
          if (guiCallBackDemo->coreInit())labelDictDemixDemo->setText(QString(strAppDirectory.c_str()));
          if (guiCallBackDemo->coreInit())guiCallBackDemo->setDictionary(QString(strAppDirectory.c_str()));
          guiCallBackDemo->getDictFilterlengths(2);
          sprintf(buf, "%lu",guiCallBackDemo->dictFilterLengthsVector->at(0));
          labeltransientsize->setText(buf);
          sprintf(buf2, "%f",1000.0*guiCallBackDemo->dictFilterLengthsVector->at(0)/guiCallBackDemo->getSignalSampleRate());
          labeltransientsizemilliseconds->setText(buf2);
          sprintf(buf1, "%lu",guiCallBackDemo->dictFilterLengthsVector->at(1));
          labeltonalsize->setText(buf1);
          sprintf(buf3, "%f",1000.0*guiCallBackDemo->dictFilterLengthsVector->at(1)/guiCallBackDemo->getSignalSampleRate());
          labeltonalsizemilliseconds->setText(buf3);
          dictOpenDemoDefault = true;
          groupBox_19->hide();
        }

#else
      char path[2048];
      getcwd(path, 2004);
      strAppDirectory = path;
      strAppDirectory += "/dic_gabor_two_scales.xml";
      FILE *fp = fopen (strAppDirectory.c_str(), "r");
      if (fp == NULL)
        {
          /* no files*/
          dialog->errorMessage("Cannot open dictionary file\n Please open a dictionary or use the custom dictionary");
        }
      else
        {
          /* files */
          fclose(fp);
          if (guiCallBackDemo->coreInit())labelDictDemixDemo->setText(QString(strAppDirectory.c_str()));
          if (guiCallBackDemo->coreInit())guiCallBackDemo->setDictionary(QString(strAppDirectory.c_str()));
          guiCallBackDemo->getDictFilterlengths(2);
          sprintf(buf, "%lu",guiCallBackDemo->dictFilterLengthsVector->at(0));
          labeltransientsize->setText(buf);
          sprintf(buf2, "%f",1000.0*guiCallBackDemo->dictFilterLengthsVector->at(0)/guiCallBackDemo->getSignalSampleRate());
          labeltransientsizemilliseconds->setText(buf2);
          sprintf(buf1, "%lu",guiCallBackDemo->dictFilterLengthsVector->at(1));
          labeltonalsize->setText(buf1);
          sprintf(buf3, "%f",1000.0*guiCallBackDemo->dictFilterLengthsVector->at(1)/guiCallBackDemo->getSignalSampleRate());
          labeltonalsizemilliseconds->setText(buf3);
          dictOpenDemoDefault = true;
          groupBox_19->hide();
        }
#endif /* WIN32 */

    }
}

void MainWindow::on_btnOpenDefaultMixerDemix_clicked(){
  std::string strAppDirectory;

#ifdef __WIN32__
      char szAppPath[MAX_PATH] = "";

      GetModuleFileName(NULL, szAppPath, MAX_PATH);

      /* Extract directory*/
      strAppDirectory = szAppPath;

      strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind("\\"));
      strAppDirectory += "\\mix_58_mixer.txt";
      FILE *fp = fopen (strAppDirectory.c_str(), "r");
      if (fp == NULL)
        {
          /* no files*/
          dialog->errorMessage("Cannot open dictionary file\n Please open a dictionary or use the custom dictionary");
        }
      else
        {
          /* files */
          fclose(fp);
      if (guiCallBackDemix->openMixer(QString(strAppDirectory.c_str())))
        {
          labelMixer->setText(strAppDirectory.c_str());
          char buf[3];
          sprintf(buf, "%d", guiCallBackDemix->mixer->numSources);
          labelBookMixeeNbrSources->setText(buf);
        }
      else dialog->errorMessage("Cannot open mixer file");
        }

#else
      char path[2048];
      getcwd(path, 2004);
      strAppDirectory = path;
      strAppDirectory += "/mix_58_mixer.txt";
      FILE *fp = fopen (strAppDirectory.c_str(), "r");
      if (fp == NULL)
        {
          /* no files*/
          dialog->errorMessage("Cannot open dictionary file\n Please open a dictionary or use the custom dictionary");
        }
      else
        {
          /* files */
          fclose(fp);
           if (guiCallBackDemix->openMixer(QString(strAppDirectory.c_str())))
        {
          labelMixer->setText(strAppDirectory.c_str());
          char buf[3];
          sprintf(buf, "%d", guiCallBackDemix->mixer->numSources);
          labelBookMixeeNbrSources->setText(buf);
        }
      else dialog->errorMessage("Cannot open mixer file");
        }
#endif /* WIN32 */

    

}

void MainWindow::on_btnValidateCustomDict_clicked()
{
  map<string, string, mp_ltstring>* parameterCustomBlock1 = new map<string, string, mp_ltstring>();
  map<string, string, mp_ltstring>* parameterCustomBlock2 = new map<string, string, mp_ltstring>();
  char buf[32];
  char buf1[32];
  char buf2[32];
  char buf3[32];
  if (guiCallBackDemo->coreInit() && !dictOpenDemo)
    {

      MP_Block_Factory_c::get_block_factory()->get_block_default_map(comboBoxBlock1Type->currentText().toStdString().c_str())(parameterCustomBlock1);

      if (lineEditCustomBlock1WindowLen->text().toInt()> 0)
        {
          (*parameterCustomBlock1)["windowLen"] = lineEditCustomBlock1WindowLen->text().toStdString();

        }
      if (lineEditCustomBlock1FftSize->text().toInt()> 0 && lineEditCustomBlock1FftSize->text().toInt()%2 ==0)
        {
          (*parameterCustomBlock1)["fftSize"] = lineEditCustomBlock1FftSize->text().toStdString();
        }

      MP_Block_Factory_c::get_block_factory()->get_block_default_map(comboBoxBlock2Type->currentText().toStdString().c_str())(parameterCustomBlock2);
      if (lineEditCustomBlock2WindowLen->text().toInt()> 0)
        {
          (*parameterCustomBlock2)["windowLen"] = lineEditCustomBlock2WindowLen->text().toStdString();

        }
      if (lineEditCustomBlock2FftSize->text().toInt()> 0 && lineEditCustomBlock2FftSize->text().toInt()%2 ==0)
        {
          (*parameterCustomBlock2)["fftSize"] = lineEditCustomBlock2FftSize->text().toStdString();
        }

      guiCallBackDemo->initDictionary();
      guiCallBackDemo->addCustomBlockToDictionary(parameterCustomBlock1);
      guiCallBackDemo->addCustomBlockToDictionary(parameterCustomBlock2);
       guiCallBackDemo->getDictFilterlengths(2);
          sprintf(buf, "%lu",guiCallBackDemo->dictFilterLengthsVector->at(0));
          labeltransientsize->setText(buf);
          sprintf(buf2, "%f",1000.0*guiCallBackDemo->dictFilterLengthsVector->at(0)/guiCallBackDemo->getSignalSampleRate());
          labeltransientsizemilliseconds->setText(buf2);
          sprintf(buf1, "%lu",guiCallBackDemo->dictFilterLengthsVector->at(1));
          labeltonalsize->setText(buf1);
          sprintf(buf3, "%f",1000.0*guiCallBackDemo->dictFilterLengthsVector->at(1)/guiCallBackDemo->getSignalSampleRate());
          labeltonalsizemilliseconds->setText(buf3);
      dictOpenDemoCustom = true;
      groupBox_22->hide();
    }
}

void MainWindow::on_btnLauchDemo_clicked()
{
  if (guiCallBackDemo->coreInit() && (dictOpenDemoDefault||dictOpenDemo||dictOpenDemoCustom))
    {
      if (!stopContraintSet)guiCallBackDemo->setIterationNumber(comboBoxNumIterDemo->currentText().toULong());
      guiCallBackDemo->iterateAll();
    }
  else dialog->errorMessage("parameter not correctly set");


}

void MainWindow::on_btnPlayDemo_clicked()
{

  if (NULL!=guiCallBackDemo->signal)
    {
      std::vector<bool> * selectedChannel =  new std::vector<bool>(guiCallBackDemo->signal->numChans, false);
      for (int i = 0 ; i<guiCallBackDemo->signal->numChans ; i++)
        {
          (*selectedChannel)[i] = true;
        }

      if (radioButtonOriDemo->isChecked())guiCallBackDemo->playBaseSignal(selectedChannel,0,0);
      if (radioButtonTransiDemo->isChecked())guiCallBackDemo->playTransientSignal(selectedChannel,0,0);
      if (radioButtonOtherDemo->isChecked())guiCallBackDemo->playOtherSignal(selectedChannel,0,0);
      if (radioButtonResiDemo->isChecked())guiCallBackDemo->playResidualSignal(selectedChannel,0,0);
    }

}



void MainWindow::on_btnOpenDictDemo_clicked()
{
  QString panelName = "MPTK GUI: Open dictionary";
  QString fileType ="XML Files (*.xml);;All Files (*)";
  QString s =dialog->setOpenFileName(panelName, fileType );
  char buf[32];
  char buf1[32];
  char buf2[32];
  char buf3[32];
  if (guiCallBackDemo->coreInit() && !dictOpenDemoDefault)
    {
      if (!s.isEmpty())
        {
          if (guiCallBackDemo->coreInit())guiCallBackDemo->setDictionary(s);
          labelDictDemixDemo->setText(s);
          guiCallBackDemo->getDictFilterlengths(2);
          sprintf(buf, "%lu",guiCallBackDemo->dictFilterLengthsVector->at(0));
          labeltransientsize->setText(buf);
          sprintf(buf2, "%f",1000.0*guiCallBackDemo->dictFilterLengthsVector->at(0)/guiCallBackDemo->getSignalSampleRate());
          labeltransientsizemilliseconds->setText(buf2);
          sprintf(buf1, "%lu",guiCallBackDemo->dictFilterLengthsVector->at(1));
          labeltonalsize->setText(buf1);
          sprintf(buf3, "%f",1000.0*guiCallBackDemo->dictFilterLengthsVector->at(1)/guiCallBackDemo->getSignalSampleRate());
          labeltonalsizemilliseconds->setText(buf3);
          dictOpenDemo = true;
        }
      else dialog->errorMessage("Empty name file");
    }
  else dialog->errorMessage("Open a signal first");
}

void MainWindow::on_btnSaveCustomDict_clicked()
{
  QString panelName = "MPTK GUI: Save Cutom dictionary";
  QString fileType ="XML Files (*.xml);;All Files (*)";
  QString s =dialog->setSaveFileName(panelName, fileType );
  if (dictOpenDemoCustom)
    {
      if (!s.isEmpty())
        {
          if (guiCallBackDemo->coreInit())guiCallBackDemo->saveDictionary(s);
          labelDictDemixDemo->setText(s);
        }
      else dialog->errorMessage("Empty name file");
    }
  else dialog->errorMessage("Validate a custom dictionary first");
}

void MainWindow::on_lineEditCustomBlock1WindowLen_textEdited()
{
  char buf[32];
  sprintf(buf, "%f",1000.0*lineEditCustomBlock1WindowLen->text().toULong()/guiCallBackDemo->getSignalSampleRate());
  lineEditCustomBlock1WindowLenSec->setText(buf);

}
void MainWindow::on_lineEditCustomBlock1FftSize_textEdited()
{
  char buf[32];
  sprintf(buf, "%f",1000.0*lineEditCustomBlock1FftSize->text().toULong()/guiCallBackDemo->getSignalSampleRate());
  lineEditCustomBlock1FftSizeSec->setText(buf);

}
void MainWindow::on_lineEditNumIter_textEdited(){
	if (guiCallBack->coreInit())guiCallBack->setIterationNumber(lineEditNumIter->text().toULong());
    if (guiCallBack->coreInit())guiCallBack->unsetSNR();

}
void MainWindow::on_lineEditSNR_textEdited(){
	if (guiCallBack->coreInit())guiCallBack->setSNR(lineEditSNR->text().toDouble());
    if (guiCallBack->coreInit())guiCallBack->unsetIter();

}

void MainWindow::on_lineEditCustomBlock2WindowLen_textEdited()
{
  char buf[32];
  sprintf(buf, "%f",1000.0*lineEditCustomBlock2WindowLen->text().toULong()/guiCallBackDemo->getSignalSampleRate());
  lineEditCustomBlock2WindowLenSec->setText(buf);

}
void MainWindow::on_lineEditCustomBlock2FftSize_textEdited()
{
  char buf[32];
  sprintf(buf, "%f",1000.0*lineEditCustomBlock2FftSize->text().toULong()/guiCallBackDemo->getSignalSampleRate());
  lineEditCustomBlock2FftSizeSec->setText(buf);

}

void MainWindow::on_lineEditCustomBlock1WindowLenSec_textEdited()
{
  char buf[32];
  sprintf(buf, "%f",lineEditCustomBlock1WindowLenSec->text().toULong()*guiCallBackDemo->getSignalSampleRate()/1000.0);
  lineEditCustomBlock1WindowLen->setText(buf);
}

void MainWindow::on_lineEditCustomBlock2WindowLenSec_textEdited()
{
  char buf[32];
  sprintf(buf, "%f",lineEditCustomBlock2WindowLenSec->text().toULong()*guiCallBackDemo->getSignalSampleRate()/1000.0);
  lineEditCustomBlock2WindowLen->setText(buf);
}

void MainWindow::iteration_running(bool status)
{
  if (status) label_progress->setText("<font color=\"#FF0000\">Decomposition in progress</font>");
  else
    {
      label_progress->setText("<font color=green>Decomposition ended with success</font>");
      textEditConsol->update();
      char buf[32];
      sprintf(buf, "%lu",guiCallBack->getNumIter());
      labelNumIter->setText(buf);
    }
}    
  void MainWindow::iteration_running_demo(bool status)
  {
    if (status)
      {
        label_progress_Demo->setText("<font color=\"#FF0000\">Decomposition in progress</font>");
      }
    else
      {
        label_progress_Demo->setText("<font color=green>Decomposition ended with success</font>");
        textEditConsolDemo->update();
        groupBox_27->show();
      }

  }
  void MainWindow::iteration_running_demix(bool status)
  {
    if (status)label_progressDemix->setText("<font color=\"#FF0000\">Decomposition in progress</font>");
    else
      {
        label_progressDemix->setText("<font color=green>Decomposition ended with success</font>");
        textEditConsolDemix->update();
      }
  }
  void MainWindow::on_btnDecomposeDemo_clicked()
  {
    
   if (guiCallBackDemo->dictFilterLengthsVector->at(1) - guiCallBackDemo->dictFilterLengthsVector->at(0)> 0){   
       guiCallBackDemo->separate((guiCallBackDemo->dictFilterLengthsVector->at(1) - guiCallBackDemo->dictFilterLengthsVector->at(0))/2);       
  }
    else guiCallBackDemo->separate(200);
    textEditConsolDemo->update();
  }

  void MainWindow::on_pushButtonStopIterate_clicked(){
  	char buf[32];
    sprintf(buf, "%lu",guiCallBack->getNumIter());
    labelNumIter->setText(buf);
    pushButtonIterateAll->setText("Resume");

  
  }
