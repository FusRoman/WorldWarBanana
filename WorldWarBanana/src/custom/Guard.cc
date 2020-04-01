#include "Guard.h"

#include "macros.h"

class Patrol: public Guard::State
{

private:
    int m_maximumDeplacementLimit;
    int m_lastDirectionUpdate;

    void updateDirection(Guard& g)
    {
        m_maximumDeplacementLimit = randomInt(60, 300);
        m_lastDirectionUpdate     = 0;
        g.m_speedX                = randomFloat(-1, 1);
        g.m_speedY                = randomFloat(-1, 1);
    }

    virtual void update(Guard& g)
    {
        if (m_lastDirectionUpdate == m_maximumDeplacementLimit)
        {
            updateDirection(g);
        }
        Labyrinthe* laby = g.getMaze();
        Vec2f       nextPosition(g._x + g.m_speedX, g._y + g.m_speedY);
        Vec2i       realPosition = laby->realToGrid(nextPosition.x, nextPosition.y);
        switch (laby->getCellType(realPosition.x, realPosition.y))
        {
        case WALL:
        case TREASURE:
            updateDirection(g);
            break;
        case CMOVER:
            CMover* moverOnNextPosition = laby->getMover(realPosition.x, realPosition.y);
            if (moverOnNextPosition != nullptr && moverOnNextPosition->id() != g.id())
            {
                updateDirection(g);
            }
            break;
        default:
            break;
        }
        g.move(g.m_speedX, g.m_speedY);
        ++m_lastDirectionUpdate;
    }

    virtual void enter(Guard& g) {}
};

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

void Guard::hit(CMover* m, int damage) { Character::hit(m, damage); }

void Guard::die(CMover* m) { rester_au_sol(); }

void Guard::fire(int angle) {}

bool Guard::process_fireball(float dx, float dy) { return false; }

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