#include "Hunter.h"

#include <string.h>

#include "FireBallDX.h"
#include "Labyrinthe.h"

Sound* Hunter::_hunter_hit = new Sound("sons/hunter_hit.wav");
Sound* Hunter::heal_sound  = new Sound("sons/heal_sound.wav");

/*
 *	Constructeur.
 */

Hunter::Hunter(Labyrinthe* l, uint id): Character(l, 0, id, 100)
{
    m_damage_hit = _hunter_hit;
    m_heal_sound = heal_sound;
    message("PV: %d", m_pv);
}

void Hunter::update() {}

void Hunter::die(CMover* m) { m_laby->partie_terminee(false); }

void Hunter::hit(CMover* m, int damage)
{
    Character::hit(m, damage);
    if (m)
    {
        message("PV: %d (last shot by guard %d)", m_pv, m->id());
    }
    else
    {
        message("PV: %d", m_pv);
    }
    
}
