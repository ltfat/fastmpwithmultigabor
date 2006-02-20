
//---------------------------------------------------------------------------
//
// Name:        MptkGuiSettingsDialog.cpp
// Author:      Sylvestre COZIC
// Created:     31/01/2006 21:20:02
//
//---------------------------------------------------------------------------
#include "MptkGuiLimitsSettingsDialog.h"
#include <iostream>
#include <stdlib.h>


BEGIN_EVENT_TABLE(MptkGuiLimitsSettingsDialog,wxDialog)
  //EVT_COMMAND_TEXT_ENTER(MptkGuiLimitsSettingsDialog::reacTxtCtrl)
	EVT_BUTTON(wxID_OK, MptkGuiLimitsSettingsDialog::OnOK)
  	EVT_BUTTON(wxID_CANCEL, MptkGuiLimitsSettingsDialog::OnCancel)
END_EVENT_TABLE()
////Event Table End

  MptkGuiLimitsSettingsDialog::MptkGuiLimitsSettingsDialog( wxWindow *parent, int id, struct initLimitsSettingsStruct init_params) : 
		wxDialog( parent, id, _T("Limits settings"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("dialogBox") )
{
    CreateGUIControls();
    saved_values=init_params;

    tminTextCtrl->SetValue(wxString::Format("%.2f",init_params.time_min ));
    tmaxTextCtrl->SetValue(wxString::Format("%.2f",init_params.time_max ));
    
    if(isEnabledAmpFields())
      {
	ampminTextCtrl->SetValue(wxString::Format("%.2f",init_params.amp_min ));
	ampmaxTextCtrl->SetValue(wxString::Format("%.2f",init_params.amp_max ));   
      }

    if(isEnabledAmpFields())
      {
	fminTextCtrl->SetValue(wxString::Format("%.2f",init_params.freq_min ));    
	fmaxTextCtrl->SetValue(wxString::Format("%.2f",init_params.freq_max )); 
	dBminTextCtrl->SetValue(wxString::Format("%.2f",init_params.dB_min ));
	dBMaxTextCtrl->SetValue(wxString::Format("%.2f",init_params.dB_max ));
      }

    if(init_params.colormap_type==GRAY) grayRadioButton->SetValue(true);
    else if(init_params.colormap_type==COOL) coolRadiobutton->SetValue(true);
    else if(init_params.colormap_type==COPPER) copperRadioButton->SetValue(true);
    else if(init_params.colormap_type==JET) jetRadioButton->SetValue(true);
    else if(init_params.colormap_type==BONE) boneRadioButton->SetValue(true);
    else if(init_params.colormap_type==PINK) pinkRadioButton->SetValue(true);
    else if(init_params.colormap_type==HOT) hotRadioButton->SetValue(true);
    else grayRadioButton->SetValue(true);
    
}

MptkGuiLimitsSettingsDialog::~MptkGuiLimitsSettingsDialog()
{
} 

float MptkGuiLimitsSettingsDialog::getTimeMin()
{
  return (float) atof(tminTextCtrl->GetValue().GetData());
}
float MptkGuiLimitsSettingsDialog::getTimeMax()
{
  return (float) atof(tmaxTextCtrl->GetValue().GetData());
}

float MptkGuiLimitsSettingsDialog::getFrequenceMin()
{
  return (float) atof(fminTextCtrl->GetValue().GetData());
}

float MptkGuiLimitsSettingsDialog::getFrequenceMax()
{
  return (float) atof(fmaxTextCtrl->GetValue().GetData());
}

float MptkGuiLimitsSettingsDialog::getAmpMin()
{
  return (float) atof(ampminTextCtrl->GetValue().GetData());
}

float MptkGuiLimitsSettingsDialog::getAmpMax()
{
  return (float) atof(ampmaxTextCtrl->GetValue().GetData());
}

float MptkGuiLimitsSettingsDialog::getDBMin()
{
  return (float) atof(dBminTextCtrl->GetValue().GetData());
}
      
float MptkGuiLimitsSettingsDialog::getDBMax()
{
  return (float) atof(dBMaxTextCtrl->GetValue().GetData());
}

int MptkGuiLimitsSettingsDialog::getSelectedCMapType()
{
  if(grayRadioButton->GetValue()) return GRAY;
  else if(coolRadiobutton->GetValue()) return COOL;
  else if(copperRadioButton->GetValue()) return COPPER;
  else if(jetRadioButton->GetValue()) return JET;
  else if(boneRadioButton->GetValue()) return BONE;
  else if(pinkRadioButton->GetValue()) return PINK;
  else return HOT;
}

void MptkGuiLimitsSettingsDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
  //Analysis of fields value to chek correct values
  bool correct=false;

  if(!isFloat(tminTextCtrl->GetValue()))
    {
      tminTextCtrl->SetSelection(-1,-1);   
      statusStaticText->SetLabel("Time min must be a decimal number");
    }
  else if(!isFloat(tmaxTextCtrl->GetValue()))
    {
      tmaxTextCtrl->SetSelection(-1,-1);      
      statusStaticText->SetLabel("Time max must be a decimal number");
    }
  else if(getTimeMin()>=getTimeMax())
    {
      tmaxTextCtrl->SetSelection(-1,-1);      
      statusStaticText->SetLabel("Time max must be strictly higher than time min");
    }
  else correct=true;
  
  if(isEnabledFreqFields()&&correct)
    {
      correct=false;
      if(!isFloat(fminTextCtrl->GetValue()))
	{
	  fminTextCtrl->SetSelection(-1,-1);
	  statusStaticText->SetLabel("Freq. min must be a decimal number");
	}
      else if(!isFloat(fmaxTextCtrl->GetValue()))
	{
	  fmaxTextCtrl->SetSelection(-1,-1);   
	  statusStaticText->SetLabel("Freq. max must be a decimal number");
	}
      else if(getFrequenceMin()>=getFrequenceMax())
	{
	  fmaxTextCtrl->SetSelection(-1,-1);      
	  statusStaticText->SetLabel("Freq. max must be strictly higher than freq. min");
	}
      else if(!isFloat(dBminTextCtrl->GetValue()))
	{
	  dBminTextCtrl->SetSelection(-1,-1);  
	  statusStaticText->SetLabel("dB min must be a decimal number");
	}
      else if(!isFloat(dBMaxTextCtrl->GetValue()))
	{
	  dBMaxTextCtrl->SetSelection(-1,-1);
	  statusStaticText->SetLabel("dB max must be a decimal number");
	}
      else if(getDBMin()>=getDBMax())
	{
	  dBMaxTextCtrl->SetSelection(-1,-1);      
	  statusStaticText->SetLabel("Max dB value must be strictly higher than min value");
	}
      else correct=true;
    }
 
  if(isEnabledAmpFields()&&correct)
    {
      correct=false;
      if(!isFloat(ampminTextCtrl->GetValue()))
	{
	  ampminTextCtrl->SetSelection(-1,-1);
	  statusStaticText->SetLabel("Amplitude min must be a decimal number");
	}
      else if(!isFloat(ampmaxTextCtrl->GetValue()))
	{
	  ampmaxTextCtrl->SetSelection(-1,-1);
	  statusStaticText->SetLabel("Amplitude max must be a decimal number");
	}
      else if(getAmpMin()>=getAmpMax())
	{
	  ampmaxTextCtrl->SetSelection(-1,-1);      
	  statusStaticText->SetLabel("Amplitude max must be strictly higher than amplitude min");
	}
      else correct=true;
    }

  if(correct)
    {
      //Les champs sont remplis correctement. On sauve les valeurs dans la
      // structure prevue a cet effet
      saved_values.time_min=getTimeMin();
      saved_values.time_max=getTimeMax();

      if(isEnabledFreqFields()) 
	{
	  saved_values.freq_min=getFrequenceMin();
	  saved_values.freq_max=getFrequenceMax();
	  saved_values.dB_min=getDBMin();
	  saved_values.dB_max=getDBMax();
	}
     
      if(isEnabledAmpFields())
	{
	  saved_values.amp_min=getAmpMin();
	  saved_values.amp_max=getAmpMax();
	}
      
      saved_values.colormap_type=getSelectedCMapType();
      Hide();
      EndModal(wxID_OK);
    }

}

void MptkGuiLimitsSettingsDialog::setTimeMin(float new_tmin)
{
  tminTextCtrl->SetValue(wxString::Format("%f",new_tmin));
  saved_values.time_min=new_tmin;
}

void MptkGuiLimitsSettingsDialog::setTimeMax(float new_tmax)
{
  tmaxTextCtrl->SetValue(wxString::Format("%f",new_tmax));
  saved_values.time_max=new_tmax;
}

void MptkGuiLimitsSettingsDialog::setFrequenceMin(float new_fmin)
{
   if(isEnabledFreqFields())
    {
      fminTextCtrl->SetValue(wxString::Format("%f",new_fmin));
      saved_values.freq_min=new_fmin;
    }
}

void MptkGuiLimitsSettingsDialog::setFrequenceMax(float new_fmax)
{
  if(isEnabledFreqFields())
    {
      fmaxTextCtrl->SetValue(wxString::Format("%f",new_fmax));
      saved_values.freq_max=new_fmax;
    }
}

void MptkGuiLimitsSettingsDialog::setAmpMin(float new_ampmin)
{
  if(isEnabledAmpFields())
    {
      ampminTextCtrl->SetValue(wxString::Format("%f",new_ampmin));
      saved_values.amp_min=new_ampmin;
    }
}

void MptkGuiLimitsSettingsDialog::setAmpMax(float new_ampmax)
{
  if(isEnabledAmpFields())
    {
      ampmaxTextCtrl->SetValue(wxString::Format("%f",new_ampmax));
      saved_values.freq_max=new_ampmax;
    }
}

void MptkGuiLimitsSettingsDialog::setDBMin(float new_dBmin)
{
  if(isEnabledFreqFields())
    {
      dBminTextCtrl->SetValue(wxString::Format("%f",new_dBmin));
      saved_values.dB_min=new_dBmin;
    }
}

void MptkGuiLimitsSettingsDialog::setDBMax(float new_dBmax)
{
  if(isEnabledFreqFields())
    {
      dBMaxTextCtrl->SetValue(wxString::Format("%f",new_dBmax));
      saved_values.dB_max=new_dBmax;
    }
}

void MptkGuiLimitsSettingsDialog::setSelectedCMapType(int new_cmap_type)
{
  saved_values.colormap_type=new_cmap_type;
  
  grayRadioButton->SetValue(false);
  coolRadiobutton->SetValue(false);
  copperRadioButton->SetValue(false);
  jetRadioButton->SetValue(false);
  boneRadioButton->SetValue(false);
  pinkRadioButton->SetValue(false);
  hotRadioButton->SetValue(false);

  if(new_cmap_type==GRAY) grayRadioButton->SetValue(true);
  else if(new_cmap_type==COOL) coolRadiobutton->SetValue(true);
  else if(new_cmap_type==COPPER) copperRadioButton->SetValue(true);
  else if(new_cmap_type==JET) jetRadioButton->SetValue(true);
  else if(new_cmap_type==BONE) boneRadioButton->SetValue(true);
  else if(new_cmap_type==PINK) pinkRadioButton->SetValue(true);
  else if(new_cmap_type==HOT) hotRadioButton->SetValue(true);
  else grayRadioButton->SetValue(true);
}

void MptkGuiLimitsSettingsDialog::setAllParams(struct initLimitsSettingsStruct * params)
{
  setTimeMin(params->time_min);
  setTimeMax(params->time_max);
  setAmpMin(params->amp_min);
  setAmpMax(params->amp_max);
  setFrequenceMin(params->freq_min);
  setFrequenceMax(params->freq_max);
  setDBMin(params->dB_min);
  setDBMax(params->dB_max );
  setSelectedCMapType(params->colormap_type);
}

void MptkGuiLimitsSettingsDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    Hide();
    EndModal(wxID_CANCEL);
}

struct initLimitsSettingsStruct * MptkGuiLimitsSettingsDialog::getSavedValues()
{
  return &saved_values;
}

void MptkGuiLimitsSettingsDialog::setEnabledAmpFields(bool value)
{
  if(!value)ampminTextCtrl->Clear();
  ampminTextCtrl->SetEditable(value);
  if(!value)ampmaxTextCtrl->Clear();
  ampmaxTextCtrl->SetEditable(value);
}

void MptkGuiLimitsSettingsDialog::setEnabledFreqFields(bool value)
{
  if(!value) fminTextCtrl->Clear();
  fminTextCtrl->SetEditable(value);
  if(!value) fmaxTextCtrl->Clear();
  fmaxTextCtrl->SetEditable(value);
  if(!value) dBminTextCtrl->Clear();
  dBminTextCtrl->SetEditable(value);
  if(!value) dBMaxTextCtrl->Clear();
  dBMaxTextCtrl->SetEditable(value);
}

bool MptkGuiLimitsSettingsDialog::isEnabledAmpFields()
{
  return ampminTextCtrl->IsEditable();
}

bool MptkGuiLimitsSettingsDialog::isEnabledFreqFields()	
{
  return fminTextCtrl->IsEditable();
}

void MptkGuiLimitsSettingsDialog::CreateGUIControls(void)
{
    //Do not add custom Code here
    //wx-devcpp designer will remove them.
    //Add the custom code before or after the Blocks
    ////GUI Items Creation Start

	this->SetSize(120,32,290,477);
	this->SetTitle(wxT("limits settings"));
	this->Center();
	this->SetIcon(wxNullIcon);
	
	statusStaticText = new wxStaticText(this, ID_WXSTATUSSTATICTEXT, wxT(""), wxPoint(25,450), wxSize(250,23), 0, wxT("statusStaticText"));

	cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxPoint(175,400), wxSize(75,30), 0, wxDefaultValidator, wxT("cancelButton"));

	okButton = new wxButton(this, wxID_OK, wxT("OK"), wxPoint(48,400), wxSize(65,30), 0, wxDefaultValidator, wxT("okButton"));

	cmapStaticText = new wxStaticText(this, ID_CMAPSTATICTEXT, wxT("colormap:"), wxPoint(25,272), wxSize(55,17), 0, wxT("cmapStaticText"));

	jetRadioButton = new wxRadioButton(this, ID_JETRADIOBUTTON, wxT("jet"), wxPoint(175,294), wxSize(62,23), 0, wxDefaultValidator, wxT("jetRadioButton"));

	pinkRadioButton = new wxRadioButton(this, ID_PINKRADIOBUTTON, wxT("pink"), wxPoint(100,350), wxSize(62,23), 0, wxDefaultValidator, wxT("pinkRadioButton"));

	boneRadioButton = new wxRadioButton(this, ID_BONERADIOBUTTON, wxT("bone"), wxPoint(100,322), wxSize(62,23), 0, wxDefaultValidator, wxT("boneRadioButton"));

	hotRadioButton = new wxRadioButton(this, ID_HOTRADIOBUTTON, wxT("hot"), wxPoint(100,294), wxSize(62,23), 0, wxDefaultValidator, wxT("hotRadioButton"));

	copperRadioButton = new wxRadioButton(this, ID_COPPERRADIOBUTTON, wxT("copper"), wxPoint(25,350), wxSize(62,23), 0, wxDefaultValidator, wxT("copperRadioButton"));

	coolRadiobutton = new wxRadioButton(this, ID_COOLRADIOBUTTON, wxT("cool"), wxPoint(25,322), wxSize(62,23), 0, wxDefaultValidator, wxT("coolRadiobutton"));

	grayRadioButton = new wxRadioButton(this, ID_GRAYRADIOBUTTON, wxT("gray"), wxPoint(25,294), wxSize(62,23), 0, wxDefaultValidator, wxT("grayRadioButton"));

	dBmaxStaticText2 = new wxStaticText(this, ID_DBMAXSTATICTEXT2, wxT("dB"), wxPoint(247,244), wxSize(17,17), 0, wxT("dBmaxStaticText2"));

	dBminStaticText2 = new wxStaticText(this, ID_DBMINSTATICTEXT2, wxT("dB"), wxPoint(106,244), wxSize(17,17), 0, wxT("dBminStaticText2"));

	dBmaxStaticText1 = new wxStaticText(this, ID_DBMAXSTATICTEXT, wxT("max"), wxPoint(157,244), wxSize(25,17), 0, wxT("dBmaxStaticText1"));

	dBMaxTextCtrl = new wxTextCtrl(this, ID_DBMAXTEXTCTRL, wxT(""), wxPoint(189,241), wxSize(52,22), 0, wxDefaultValidator, wxT("dBMaxTextCtrl"));
	//dBMaxTextCtrl->SetMaxLength(0);

	dBminTextCtrl = new wxTextCtrl(this, ID_DBMINTEXTCTRL, wxT(""), wxPoint(48,241), wxSize(52,22), 0, wxDefaultValidator, wxT("dBminTextCtrl"));
	//dBminTextCtrl->SetMaxLength(0);

	dBminStaticText1 = new wxStaticText(this, ID_DBMINSTATICTEXT1, wxT("min"), wxPoint(25,244), wxSize(21,17), 0, wxT("dBminStaticText1"));

	tfRangeStaticBox = new wxStaticBox(this, ID_TFRANGESTATICBOX, wxT("TF view range"), wxPoint(8,217), wxSize(269,160));

	ampmaxTextCtrl = new wxTextCtrl(this, ID_AMPMAXTEXTCTRL, wxT(""), wxPoint(189,172), wxSize(52,22), 0, wxDefaultValidator, wxT("ampmaxTextCtrl"));
	//ampmaxTextCtrl->SetMaxLength(0);

	ampminTextCtrl = new wxTextCtrl(this, ID_AMPMINTEXTCTRL, wxT(""), wxPoint(48,172), wxSize(52,22), 0, wxDefaultValidator, wxT("ampminTextCtrl"));
	//ampminTextCtrl->SetMaxLength(0);

	ampmaxStaticText1 = new wxStaticText(this, ID_AMPMAXSTATICTEXT1, wxT("max"), wxPoint(157,175), wxSize(25,17), 0, wxT("ampMaxStaticText1"));

	ampminStatictext1 = new wxStaticText(this, ID_AMPMINSTATICTEXT1, wxT("min"), wxPoint(25,175), wxSize(21,17), 0, wxT("ampminStatictext1"));

	ampStaticBox = new wxStaticBox(this, ID_AMPLSTATICBOX, wxT("Signal amplitude"), wxPoint(8,152), wxSize(269,50));

	fmaxStaticText2 = new wxStaticText(this, ID_EMAXSTATICTEXT2, wxT("Hz"), wxPoint(247,104), wxSize(17,17), 0, wxT("emaxStaticText2"));

	fminStaticText2 = new wxStaticText(this, ID_EMINSTATICTEXT2, wxT("Hz"), wxPoint(106,104), wxSize(17,17), 0, wxT("eminStaticText2"));

	fmaxStaticText1 = new wxStaticText(this, ID_EMAXSTATICTEXT1, wxT("max"), wxPoint(157,105), wxSize(25,17), 0, wxT("emaxStaticText1"));

	fmaxTextCtrl = new wxTextCtrl(this, ID_EMAXTEXTCTRL, wxT(""), wxPoint(189,101), wxSize(52,22), 0, wxDefaultValidator, wxT("emaxTextCtrl"));
	//fmaxTextCtrl->SetMaxLength(0);

	fminStaticText1 = new wxStaticText(this, ID_EMINSTATICTEXT1, wxT("min"), wxPoint(25,105), wxSize(21,17), 0, wxT("eminStaticText1"));

	fminTextCtrl = new wxTextCtrl(this, ID_EMINTEXTCTRL, wxT(""), wxPoint(48,101), wxSize(52,22), 0, wxDefaultValidator, wxT("eminTextCtrl"));
	//fminTextCtrl->SetMaxLength(0);

	freqStaticBox = new wxStaticBox(this, ID_FREQSTATICBOX, wxT("Frequency"), wxPoint(8,80), wxSize(269,50));

	tmaxStaticText2 = new wxStaticText(this, ID_TMAXSTATICTEXT2, wxT("s"), wxPoint(247,31), wxSize(9,17), 0, wxT("tmaxStaticText2"));

	tminStaticText2 = new wxStaticText(this, ID_TMINSTATICTEXT2, wxT("s"), wxPoint(106,31), wxSize(9,17), 0, wxT("tminStaticText2"));

	tminTextCtrl = new wxTextCtrl(this, ID_TMINTEXTCTRL, wxT(""), wxPoint(48,28), wxSize(52,22), 0, wxDefaultValidator, wxT("tminTextCtrl"));
	//tminTextCtrl->SetMaxLength(0);

	//tmaxTextCtrl = new wxTextCtrl(this, wxID_ANY);
	
	//tmaxTextCtrl->XYToPosition(189,28);
	tmaxTextCtrl = new wxTextCtrl(this, ID_TMAXTEXTCTRL, wxT(""), wxPoint(189,28), wxSize(52,22));
	//tmaxTextCtrl->SetMaxLength(0);

	tmaxStaticText1 = new wxStaticText(this, ID_TMAXSTATICTEXT1, wxT("max"), wxPoint(157,32), wxSize(25,17), 0, wxT("tmaxStaticText1"));

	tminStaticText1 = new wxStaticText(this, ID_TMINSTATICTEXT1, wxT("min"), wxPoint(25,31), wxSize(21,17), 0, wxT("tminStaticText1"));

	timeStaticBox = new wxStaticBox(this, ID_TIMESTATICBOX, wxT("Time"), wxPoint(8,8), wxSize(269,50));
    ////GUI Items Creation End
}

/**
 * Cette fonction rend vrai si la chaine passee en parametre est une representation correcte de nombre decimal signe.
 */
bool MptkGuiLimitsSettingsDialog::isFloat(wxString string)
{
  bool res=false;
  bool point=false;
  int i=0;
  if(string.Length()>0)
    {
      if((string.GetChar(0)=='0')
	 ||(string.GetChar(0)=='1')
	 ||(string.GetChar(0)=='2')
	 ||(string.GetChar(0)=='3')
	 ||(string.GetChar(0)=='4')
	 ||(string.GetChar(0)=='5')
	 ||(string.GetChar(0)=='6')
	 ||(string.GetChar(0)=='7')
	 ||(string.GetChar(0)=='8')
	 ||(string.GetChar(0)=='9')
	 ||(string.GetChar(0)=='-')
	 )
	{
	  if(string.GetChar(0)=='-')
	    {
	      if(string.Length()>1)
		{
		  if((string.GetChar(1)=='0')
		     ||(string.GetChar(1)=='1')
		     ||(string.GetChar(1)=='2')
		     ||(string.GetChar(1)=='3')
		     ||(string.GetChar(1)=='4')
		     ||(string.GetChar(1)=='5')
		     ||(string.GetChar(1)=='6')
		     ||(string.GetChar(1)=='7')
		     ||(string.GetChar(1)=='8')
		     ||(string.GetChar(1)=='9'))
		    {
		      i=2;
		      res=true;
		    }
		}   
	    }
	  else
	    {
	      i=1;
	      res=true;
	    }
	}
      
      while(i<(int)string.Length()&&res)
	{
	  if(string.GetChar(i)=='.')
	    {
	      if(point) res=false;
	      else point=true;
	    }
	  else
	    {
	      if(!((string.GetChar(i)=='0')
		   ||(string.GetChar(i)=='1')
		   ||(string.GetChar(i)=='2')
		   ||(string.GetChar(i)=='3')
		   ||(string.GetChar(i)=='4')
		   ||(string.GetChar(i)=='5')
		   ||(string.GetChar(i)=='6')
		   ||(string.GetChar(i)=='7')
		   ||(string.GetChar(i)=='8')
		   ||(string.GetChar(i)=='9')
		   ))
		{
		  res=false;
		}
	    }
	  i++;
	}
    }
  return res;
}


