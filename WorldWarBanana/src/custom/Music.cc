/*****************************************************************

  Copyright (c) 2010-2011 by Patrick Amar - pa@lri.fr

    http://www.lri.fr/~pa

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "Music.h"

int	Music::_nmusics = 0;
FMOD::System*	Music::_system = 0;
FMOD::Channel*	Music::_channel = 0;

void Music::init (void)
{
	unsigned int	version;

	if (FMOD_OK != FMOD::System_Create(&_system) ||
		FMOD_OK != _system->getVersion (&version))
	{
		fprintf(stderr, "Erreur FMOD.\n");
		return;
	}
	if (version < FMOD_VERSION) {
		fprintf(stderr, "Version FMOD trop ancienne.\n");
		return;
	}
	if (FMOD_OK != _system -> init (32, FMOD_INIT_NORMAL, 0))
	{
		fprintf(stderr, "Erreur pilote de sortie (ALSA) FMOD.\n");
		return;
	}
}

// Charge une nouvelle musique
Music::Music(const char* file)
{
	if (_nmusics++ == 0)	// une occurrence de plus.
		init();			    // initialiser fmod la première fois.

	if (FMOD_OK != _system->createSound(file, FMOD_SOFTWARE, 0, &_music) ||
		FMOD_OK != _music->setMode(FMOD_LOOP_NORMAL))
    {
		fprintf(stderr, "Erreur FMOD: %s.\n", file);
    }
}

// Détruit une musique
Music::~Music()
{
	_music->release ();
	// un son de moins.
	if (--_nmusics == 0) {
		// libérer le système de son.
		_system->close ();
		_system->release ();
	}
}

// Joue une musique
void Music::play(float volume)
{
	(void) _system->playSound(FMOD_CHANNEL_FREE, _music, 0, &_channel);
	_channel->setVolume(volume);
	_channel->setPan(0.);
	_system->update();
}

void Music::stop()
{
    _channel->stop();
}