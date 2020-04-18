#include "FireBallDX.h"

#include "Guard.h"
#include "Labyrinthe.h"

uint FireBallDX::m_tick = 0;

FireBallDX::FireBallDX(Labyrinthe* laby, uint id):
    CMover(-Environnement::scale, -Environnement::scale, laby, Guard::modeles[0], id),
    m_weapon(nullptr)
{
}

void FireBallDX::setWeapon(Weapon* weapon) { m_weapon = weapon; }

void FireBallDX::update()
{
    if (m_id == 1)
    {
        ++m_tick;

        // Mise à jour des gardes
        float dmax = (float) m_laby->getMaxDistance();
        float threshold = 10.;
        float sum = 0.;
        for (uint i = 0; i < m_laby->getNbGuards(); ++i)
        {
            Guard* guard = m_laby->getGuard(i);
            if (guard->isDead())
            {
                continue;
            }

            Vec2i p = Labyrinthe::realToGrid(guard->_x, guard->_y);
            float di = (float) m_laby->distanceFromTreasure(p.x, p.y);
            sum += dmax / di;
            guard->affectToDefense(sum <= threshold);
        }
    }
}

bool FireBallDX::block() const { return false; }

void FireBallDX::fire(int angle_vertical) {}

bool FireBallDX::process_fireball(float dx, float dy)
{
    if (m_weapon)
    {
        return m_weapon->process_fireball(_fb, dx, dy);
    }
    else
    {
        WARNING("FireBallDX::process_fireball called when unexpected");
        return true;
    }
}

void FireBallDX::hit(CMover*, int) {}