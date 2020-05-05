#include "Character.h"

#include "macros.h"

Character::Character(Labyrinthe* l, const char* modele, uint id, int maxpvs):
    CMover(0, 0, l, modele, id), m_pv(maxpvs), m_maxpvs(maxpvs), m_weapon(this)
{
}

void Character::hit(CMover* m, int damage)
{
    if (damage >= 0)
    {
        if (m_pv > 0)
        {
            play(m_damage_hit);
        }
        if (m_pv - damage <= 0)
        {
            m_pv = 0;
            die(m);
        }
        else
        {
            m_pv -= damage;
        }
    }
    else
    {
        play(m_heal_sound);
        if (m_pv - damage >= m_maxpvs)
        {
            m_pv = m_maxpvs;
        }
        else
        {
            m_pv -= damage;
        }
    }
}

bool Character::process_fireball(float dx, float dy)
{
    return m_weapon.process_fireball(_fb, dx, dy);
}

void Character::fire(int angle_vertical) { m_weapon.fire(angle_vertical); }

bool Character::block() const { return !isDead(); }