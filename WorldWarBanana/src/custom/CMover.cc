#include "CMover.h"

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