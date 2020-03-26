#include "Character.h"

Character::Character(int x, int y, Labyrinthe* l, const char* modele, uint id) :
    CMover(x, y, l, modele, id), m_pv(100), m_weapon(this) {} 


void Character::hurt(CMover* m, int damage)
{
    if (damage >= 0)
    {
        play(m_damage_hit);
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
        if (m_pv - damage >= 100)
        {
            m_pv = 100;
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

void Character::fire(int angle_vertical)
{
    m_weapon.fire(_fb, angle_vertical);
}