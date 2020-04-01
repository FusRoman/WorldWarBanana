#include "FireBallDX.h"

#include "Guard.h"
#include "Labyrinthe.h"

FireBallDX::FireBallDX(Labyrinthe* laby, uint id):
    CMover(-Environnement::scale, -Environnement::scale, laby, Guard::modeles[0], id),
    m_weapon(nullptr)
{
}

void FireBallDX::setWeapon(Weapon* weapon) { m_weapon = weapon; }

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