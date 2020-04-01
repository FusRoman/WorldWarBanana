#include "Guard.h"

#include "macros.h"

Sound* Guard::damage_hit = new Sound("sons/roblox_hit.wav");
Sound* Guard::heal_sound = new Sound("sons/heal_sound.wav");

const std::vector<const char*> Guard::modeles({"drfreak", "Marvin", "Potator", "garde", "Droid",
                                               "Lezard", "Samourai", "Serpent", "Squelette",
                                               "Blade"});

Guard::Guard(Labyrinthe* l, const char* modele, uint id):
    Character(120, 80, l, modele, id), m_speedX(1), m_speedY(1)
{
    m_damage_hit = damage_hit;
    m_heal_sound = heal_sound;
}

Guard::Guard(Labyrinthe* l, int modele, uint id):
    Character(120, 80, l, modeles.at(modele), id), m_speedX(1), m_speedY(1)
{
    m_damage_hit = damage_hit;
    m_heal_sound = heal_sound;
}

void Guard::hit(CMover* m, int damage)
{
    Character::hit(m, damage);
    message("gardiens : %d, pv : %d", m_id, m_pv);
    if (damage > 0)
    {
        // tomber();
    }
}

void Guard::die(CMover* m) { rester_au_sol(); }

void Guard::update()
{
    Character::update();
    if (m_pv > 0)
    {
        Labyrinthe* laby         = getMaze();
        Vec2i       nextPosition = laby->realToGrid(_x + m_speedX, _y);
        switch (laby->getCellType(nextPosition.x, nextPosition.y))
        {
        case WALL:
        case TREASURE:
            if (m_speedX > 0)
            {
                _angle = 90;
            }
            else
            {
                _angle = 270;
            }
            m_speedX *= -1;
            break;
        default:
            break;
        }
        move(m_speedX, 0);
    }
}