#include "Hunter.h"

#include <string.h>

#include "Labyrinthe.h"
#include "FireBallDX.h"

Sound* Hunter::_hunter_hit  = new Sound("sons/hunter_hit.wav");
Sound* Hunter::heal_sound = new Sound("sons/heal_sound.wav");

/*
 *	Constructeur.
 */

Hunter::Hunter(Labyrinthe* l, uint id):
    Character(100, 80, l, 0, id)
{
    m_damage_hit = _hunter_hit;
    m_heal_sound = heal_sound;
}

/*
 *	Clic droit: par d�faut fait tomber le premier gardien.
 *
 *	Inutile dans le vrai jeu, mais c'est juste pour montrer
 *	une utilisation des fonctions � tomber � et � rester_au_sol �
 */

void Hunter::right_click(bool shift, bool control)
{
    if (shift)
        _l->_guards[1]->rester_au_sol();
    else
        _l->_guards[1]->tomber();
}

void Hunter::die(CMover* m)
{
    partie_terminee(false);
}
