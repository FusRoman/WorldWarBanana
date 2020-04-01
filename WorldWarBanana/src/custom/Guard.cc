#include "Guard.h"

#include "macros.h"

class Patrol: public Guard::State
{

private:
    int m_maximumDeplacementLimit;
    int m_lastDirectionUpdate;

    void updateDirection()
    {
        m_maximumDeplacementLimit              = randomInt(60, 300);
        m_lastDirectionUpdate                  = 0;
        std::pair<Vec2f, float> newSpeedVector = randomVector();
        m_guard->m_speedX                      = newSpeedVector.first.x;
        m_guard->m_speedY                      = newSpeedVector.first.y;
        m_guard->_angle                        = radiansToDegrees(newSpeedVector.second);
    }

public:
    Patrol(Guard* g): State(g) {}

    virtual void update()
    {
        if (m_lastDirectionUpdate == m_maximumDeplacementLimit)
        {
            updateDirection();
        }
        Labyrinthe* laby = m_guard->getMaze();
        Vec2f       nextPosition(m_guard->_x + m_guard->m_speedX, m_guard->_y + m_guard->m_speedY);
        Vec2i       realPosition = laby->realToGrid(nextPosition.x, nextPosition.y);
        switch (laby->getCellType(realPosition.x, realPosition.y))
        {
        case WALL:
        case TREASURE:
            updateDirection();
            break;
        case CMOVER:
        {
            CMover* moverOnNextPosition = laby->getMover(realPosition.x, realPosition.y);
            if (moverOnNextPosition != nullptr && moverOnNextPosition->id() != m_guard->id())
            {
                updateDirection();
            }
            break;
        }
        default:
            break;
        }
        m_guard->move(m_guard->m_speedX, m_guard->m_speedY);
        ++m_lastDirectionUpdate;
    }

    virtual void enter() {}
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
    m_state      = new Patrol(this);
}

Guard::Guard(Labyrinthe* l, int modele, uint id):
    Character(120, 80, l, modeles.at(modele), id), m_speedX(1), m_speedY(1)
{
    m_damage_hit = damage_hit;
    m_heal_sound = heal_sound;
    m_state      = new Patrol(this);
}

void Guard::hit(CMover* m, int damage) { Character::hit(m, damage); }

void Guard::die(CMover* m) { rester_au_sol(); }

void Guard::fire(int angle) {}

bool Guard::process_fireball(float dx, float dy) { return false; }

void Guard::setState(State* state)
{
    //(facultatif: m_state->quit();)
    m_state = state;
    state->enter();
}

void Guard::update()
{
    Character::update();
    m_state->update();
}