//---------------------------------------------------------------------------
//
// Name:        LIMITSETTINGSDlg.h
// Author:      Administrator
// Created:     31/01/2006 21:20:02
//
//---------------------------------------------------------------------------
#ifndef __MPTKGUILIMITSSETTINGS_H_
#define __MPTKGUILIMITSSETTINGS_H_

#include <wx/wxprec.h>
#include <wx/wx.h>

//Do not add custom headers.
//wx-dvcpp designer will remove them
////Header Include Start
#include <wx/button.h>
#include <wx/radiobut.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include "MptkGuiColormaps.h"
////Header Include End

#include <wx/dialog.h>

/** \brief This structure contains fiels for all values set in this dialog box*/ 
struct initLimitsSettingsStruct
{
  float time_min;
  float time_max;
  float freq_min;
  float freq_max;
  float amp_min;
  float amp_max;
  float dB_min;
  float dB_max;
  int colormap_type;
  float freq_bound;
  float time_bound;
};

/** \brief this dialog box is used to adjust settings for the displayed views.
 time, amplitude, frequency, intensity  bounds and colormap type can be set here*/
class MptkGuiLimitsSettingsDialog : public wxDialog
{
public:
  /** \brief Construcor
      \param init_params structure containing initial values for the fields
    */
  MptkGuiLimitsSettingsDialog( wxWindow *parent, int id, struct initLimitsSettingsStruct init_params );
    virtual ~MptkGuiLimitsSettingsDialog();
    
    /** \brief Function called when the user presses OK button. Values of editable text fields are parsed
	and checked correct.If it is correct, settings are saved and can be obtained with getSavedValues()*/
    void OnOK(wxCommandEvent& event); 

    /** \brief Function called when the user presses Cancel button. Simply hides the dialog box (without
	destroying it)*/
    void OnCancel(wxCommandEvent& WXUNUSED(event));

    /** \brief Returns the current value of  min time field of the dialog box. This value might be 
	inconsistent if  the text field does not contain a decimal value*/
    float getTimeMin(); 

    /** \brief Returns the current value of max time field of the dialog box. This value might be 
	inconsistent if  the text field does not contain a decimal value*/
    float getTimeMax();

    /** \brief Returns the current value of min frequency field of the dialog box. This value might be 
	inconsistent if  the text field does not contain a decimal value*/
    float getFrequenceMin();

    /** \brief Returns the current value of max frequency field of the dialog box. This value might be 
	inconsistent if  the text field does not contain a decimal value*/ 
    float getFrequenceMax();

    /** \brief Returns the current value of min amplitude field of the dialog box. This value might be 
	inconsistent if  the text field does not contain a decimal value*/ 
    float getAmpMin();

    /** \brief Returns the current value of max amplitude field of the dialog box. This value might be 
	inconsistent if  the text field does not contain a decimal value*/ 
    float getAmpMax();

    /** \brief Returns the current value of min TF view range field of the dialog box. This value might be 
	inconsistent if  the text field does not contain a decimal value*/   
    float getDBMin();

    /** \brief Returns the current value of max TF view range field of the dialog box. This value might be 
	inconsistent if  the text field does not contain a decimal value*/ 
    float getDBMax();

    /** \brief Returns the currently selected colormap type (COOL,COPPER,HOT,JET,GRAY,PINK or BONE)*/
    int getSelectedCMapType();

    /** \brief Tells if min and max amplitude fields are active. It will be true if there is a signal
	view currently displayed in the GUI*/
    bool isEnabledAmpFields();
    
    /** \brief Tells if frequency and TF view range fields are active.It will be true if there
	is a TF view currently displayed in the GUI*/
    bool isEnabledFreqFields();
    
    /** \brief Returns last saved values, in a structure identical to the structure given to the constructor.
        These values have been checked correct*/ 
    struct initLimitsSettingsStruct * getSavedValues();
    
    /** \brief Sets the value in min time field*/
    void setTimeMin(float new_tmin);

    /** \brief Sets the value in max time field*/
    void setTimeMax(float new_tmax);

    /** \brief Sets the value in min frequency field*/
    void setFrequenceMin(float new_fmin);

    /** \brief Sets the value in max frequency field*/
    void setFrequenceMax(float new_fmax);

    /** \brief Sets the value in min amplitude field*/
    void setAmpMin(float new_ampmin);

    /** \brief Sets the value in max amplitude field*/
    void setAmpMax(float new_ampmax);

    /** \brief Sets the value in min TF view range field*/
    void setDBMin(float new_dBmin);

    /** \brief Sets the value in max TF view range field*/
    void setDBMax(float new_dBmax);

    /** \brief Sets the selected colormap type
        \param new_cmap_type Must be one of the colormap type
       constants defined in MptkGuiColormaps (GRAY,JET,HOT,BONE,PINK,COPPER,COOL) */
    void setSelectedCMapType(int new_cmap_type);

    /** \brief Sets min and max amplitude fields active or inactive*/
    void setEnabledAmpFields(bool value);

    /** \brief Sets frequency and TF view range fields active or inactive*/
    void setEnabledFreqFields(bool value);
    
    /** Sets the values of every field in the dialog box through a structure*/
    void setAllParams(struct initLimitsSettingsStruct * init_params);
    
private:
  ////GUI Control Declaration Start
    wxStaticText * statusStaticText;
    wxButton *cancelButton;
    wxButton *okButton;
    wxStaticText *cmapStaticText;
    wxRadioButton *jetRadioButton;
    wxRadioButton *pinkRadioButton;
    wxRadioButton *boneRadioButton;
    wxRadioButton *hotRadioButton;
    wxRadioButton *copperRadioButton;
    wxRadioButton *coolRadiobutton;
    wxRadioButton *grayRadioButton;
    wxStaticText *dBmaxStaticText2;
    wxStaticText *dBminStaticText2;
    wxStaticText *dBmaxStaticText1;
    wxTextCtrl *dBMaxTextCtrl;
    wxTextCtrl *dBminTextCtrl;
    wxStaticText *dBminStaticText1;
    wxStaticBox *tfRangeStaticBox;
    wxTextCtrl *ampmaxTextCtrl;
    wxTextCtrl *ampminTextCtrl;
    wxStaticText *ampmaxStaticText1;
    wxStaticText *ampminStatictext1;
    wxStaticBox *ampStaticBox;
    wxStaticText *fmaxStaticText2;
    wxStaticText *fminStaticText2;
    wxStaticText *fmaxStaticText1;
    wxTextCtrl *fmaxTextCtrl;
    wxStaticText *fminStaticText1;
    wxTextCtrl *fminTextCtrl;
    wxStaticBox *freqStaticBox;
    wxStaticText *tmaxStaticText2;
    wxStaticText *tminStaticText2;
    wxTextCtrl *tminTextCtrl;
    wxTextCtrl *tmaxTextCtrl;
    wxStaticText *tmaxStaticText1;
    wxStaticText *tminStaticText1;
    wxStaticBox *timeStaticBox;
  ////GUI Control Declaration End
private:

    //Note: if you receive any error with these enums, then you need to
    //change your old form code that are based on the #define control ids.
    //#defines may replace a numeric value for the enums names.
    //Try copy pasting the below block in your old Form header Files.
	enum {
////GUI Enum Control ID Start
ID_WXSTATUSSTATICTEXT=1071,
ID_WXCANCELBUTTON = 1070,
ID_WXOKBUTTON = 1069,
ID_CMAPSTATICTEXT = 1068,
ID_JETRADIOBUTTON = 1066,
ID_PINKRADIOBUTTON = 1065,
ID_BONERADIOBUTTON = 1064,
ID_HOTRADIOBUTTON = 1063,
ID_COPPERRADIOBUTTON = 1062,
ID_COOLRADIOBUTTON = 1061,
ID_GRAYRADIOBUTTON = 1060,
ID_DBMAXSTATICTEXT2 = 1059,
ID_DBMINSTATICTEXT2 = 1058,
ID_DBMAXSTATICTEXT = 1057,
ID_DBMAXTEXTCTRL = 1056,
ID_DBMINTEXTCTRL = 1055,
ID_DBMINSTATICTEXT1 = 1054,
ID_TFRANGESTATICBOX = 1052,
ID_AMPMAXTEXTCTRL = 1051,
ID_AMPMINTEXTCTRL = 1050,
ID_AMPMAXSTATICTEXT1 = 1049,
ID_AMPMINSTATICTEXT1 = 1048,
ID_AMPLSTATICBOX = 1047,
ID_EMAXSTATICTEXT2 = 1046,
ID_EMINSTATICTEXT2 = 1045,
ID_EMAXSTATICTEXT1 = 1044,
ID_EMAXTEXTCTRL = 1043,
ID_EMINSTATICTEXT1 = 1042,
ID_EMINTEXTCTRL = 1041,
ID_FREQSTATICBOX = 1034,
ID_TMAXSTATICTEXT2 = 1011,
ID_TMINSTATICTEXT2 = 1010,
ID_TMINTEXTCTRL = 1009,
ID_TMAXTEXTCTRL = 1008,
ID_TMAXSTATICTEXT1 = 1006,
ID_TMINSTATICTEXT1 = 1005,
ID_TIMESTATICBOX = 1002,
////GUI Enum Control ID End
   ID_DUMMY_VALUE_ //Dont Delete this DummyValue
   }; //End of Enum
private:  

    bool isFloat(wxString string);
    struct initLimitsSettingsStruct saved_values;
    DECLARE_EVENT_TABLE()

public:
    void CreateGUIControls(void);
};

#endif
