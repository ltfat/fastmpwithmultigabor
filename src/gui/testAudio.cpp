# include "MptkGuiAudio.h"
# include <iostream>
# include <vector>
using namespace std;

int main(int argc, char ** argv) {
MptkGuiAudio * audio;
MP_Signal_c * sig;
int milieu;
sig = MP_Signal_c::init(argv[1]);
cout<<"signal charge \n";flush(cout);
std::vector<bool> vect=* new vector<bool>(2,false);
vect[0]=true;
audio= new MptkGuiAudio(NULL, sig);
milieu=audio->getEnd()/2;
cout<<"lecture du signal complet \n";flush(cout);
audio->play();
Pa_Sleep(20 *1000);
audio->pause();
Pa_Sleep(2 *1000);
audio->play();
Pa_Sleep(20 *1000);
cout<<"lecture des canaux de la liste \n";flush(cout);
audio->playSelected(&vect);
Pa_Sleep(20 *1000);
audio->pause();
Pa_Sleep(2 *1000);
audio->play();
Pa_Sleep(20 *1000);
cout<<"lecture de la moitie du signal complet on commence au milieu \n";flush(cout);
audio->play(milieu,audio->getEnd());
Pa_Sleep(20 *1000);
audio->pause();
Pa_Sleep(2 *1000);
audio->play();
Pa_Sleep(20 *1000);
cout<<"lecture de la moitie des canaux de la liste on commence au milieu \n";flush(cout);
audio->playSelected(&vect, milieu,audio->getEnd());
Pa_Sleep(20 *1000);
audio->pause();
Pa_Sleep(2 *1000);
audio->play();
Pa_Sleep(20 *1000);
}
