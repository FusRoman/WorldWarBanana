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

void CMover::hurt(Mover* m, int damage)
{
    if (damage >= 0)
    {
        play(m_damage_hit);
        if (pv - damage <= 0)
        {
            pv = 0;
            die();
        }
        else
        {
            pv = pv - damage;
        }
    }
    else
    {
        play(m_heal_sound);
        if (pv - damage >= 100)
        {
            pv = 100;
        }
        else
        {
            pv = pv - damage;
        }
    }
}
