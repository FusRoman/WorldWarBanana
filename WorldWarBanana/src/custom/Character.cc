#include "Character.h"

Character::Character(int x, int y, Labyrinthe* l, const char* modele, uint id) :
    CMover(x, y, l, modele, id), m_pv(100) {} 


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