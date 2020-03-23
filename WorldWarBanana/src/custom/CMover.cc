#include "CMover.h"
#include "utils.h"
#include "Labyrinthe.h"

CMover::CMover(int x, int y, Labyrinthe* l, const char* modele, uint id):
    Mover   (x, y, l, modele),
    m_laby  (l),
    m_id    (id)
{}

bool CMover::move(double dx, double dy)
{
    return m_laby->move(this, dx, dy);
}

void CMover::hurt(CMover* m, int damage)
{
    if (damage >= 0)
    {
        play(m_damage_hit);
        if (m_pv - damage <= 0)
        {
            m_pv = 0;
            die();
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
