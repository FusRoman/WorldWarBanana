#include "CMover.h"
#include "Labyrinthe.h"
#include "utils.h"

CMover::CMover(int x, int y, Labyrinthe* l, const char* modele, uint id):
    Mover(x, y, l, modele), m_laby(l), m_id(id)
{}

Vec2f CMover::getRealPosition() const
{
    return Vec2f(_x, _y);
}

Vec2i CMover::getGridPosition(float dx, float dy) const
{
    return Labyrinthe::realToGrid(_x + dx, _y + dy);
}

bool CMover::move(double dx, double dy) { return m_laby->move(this, dx, dy); }
