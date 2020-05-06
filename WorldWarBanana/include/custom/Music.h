#pragma once

/**
 * Ce fichier est essentiellement une copie de Sound.h.
 * Puisqu'on ne peut pas modifier les fichiers de base,
 * c'était la seule manière de rajouter un son qui peut se jouer en boucle
 * (autrement dit, de la musique).
 * 
 * Si Sound.h et Sound.cpp avaient été modifiables, il n'y aurait qu'à ajouter
 * un paramètre 'loop' dans le constructeur et appeler setMode avec 
 * FMOD_LOOP_OFF ou FMOD_LOOP_NORMAL en fonction de sa valeur.
 * C'est la seule vraie différence entre les deux paires de fichiers
 * (en plus de stop).
 */

#ifdef _WIN32
#	include <conio.h>
#	include <windows.h>
#endif

#include "fmod.hpp"
#include "fmod_errors.h"

class Music 
{
private:
	static FMOD::System*	_system;
	static FMOD::Channel*	_channel;
	static int _nmusics;	        // compte d'occurrences.

	FMOD::Sound* _music;
	void init(void);
public:
	Music(const char* file);		// charge un fichier de sons.
	~Music();					    // libère les échantillons.
	void play(float volume = 1.);	// le fait jouer.
    void stop();
};