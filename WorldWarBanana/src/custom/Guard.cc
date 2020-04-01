#include "Guard.h"

#include "macros.h"

class Attack: public Guard::State
{
private:
public:
    Attack(Guard* g);
    virtual void update() override;
    virtual void enter() override;
};

class Pursuit: public Guard::State
{
private:
public:
    Pursuit(Guard* g);
    virtual void update() override;
    virtual void enter() override;
};

class Patrol: public Guard::State
{

private:
    int  m_maximumDeplacementLimit;
    int  m_lastDirectionUpdate;
    void updateDirection();

public:
    Patrol(Guard* g);
    virtual void update() override;
    virtual void enter() override;
};

class Dead: public Guard::State
{
public:
    Dead(Guard* g);
    virtual void update() override;
    virtual void enter() override;
};

/****** Implémentation de Attack *******/
Attack::Attack(Guard* g): State(g) {}

void Attack::update() {
    if(!m_guard->canSeeHunter()){
        m_guard->setState(new Pursuit(m_guard));
    }
    else {
        if(m_guard->m_guardWeapon->canFire()){
            m_guard->m_guardWeapon->fire(0);
        }
    }
}
void Attack::enter() {}


/****** Implémentation de Pursuit *******/
Pursuit::Pursuit(Guard* g) : State(g){}

void Pursuit::update(){}
void Pursuit::enter(){}


/****** Implémentation de Patrol *******/
void Patrol::updateDirection()
{
    m_maximumDeplacementLimit              = randomInt(60, 300);
    m_lastDirectionUpdate                  = 0;
    std::pair<Vec2f, float> newSpeedVector = randomVector();
    m_guard->m_speedX                      = newSpeedVector.first.x;
    m_guard->m_speedY                      = newSpeedVector.first.y;
    m_guard->_angle                        = radiansToDegrees(newSpeedVector.second);
}

Patrol::Patrol(Guard* g): State(g) {}

void Patrol::update()
{
    if (m_guard->canSeeHunter())
    {
        m_guard->setState(new Attack(m_guard));
    }
    else if (m_lastDirectionUpdate == m_maximumDeplacementLimit)
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

void Patrol::enter() {}


/****** Implémentation de Dead *******/
Dead::Dead(Guard* g): State(g) {}

void Dead::update() {}
void Dead::enter()
{
    m_guard->rester_au_sol();
    m_guard->getMaze()->free(m_guard);
}




Sound* Guard::damage_hit = new Sound("sons/roblox_hit.wav");
Sound* Guard::heal_sound = new Sound("sons/heal_sound.wav");

const std::vector<const char*> Guard::modeles({"drfreak", "Marvin", "Potator", "garde", "Droid",
                                               "Lezard", "Samourai", "Serpent", "Squelette",
                                               "Blade"});

Guard::Guard(Labyrinthe* l, const char* modele, uint id):
    Character(120, 80, l, modele, id), m_speedX(1), m_speedY(1), m_vision(5*Environnement::scale)
{
    m_damage_hit = damage_hit;
    m_heal_sound = heal_sound;
    m_state      = new Patrol(this);
    m_guardWeapon = new Weapon(this, "Holy Banana", 10, 60, Weapon::infiniteReach);
}

Guard::Guard(Labyrinthe* l, int modele, uint id):
    Character(120, 80, l, modeles.at(modele), id), m_speedX(1), m_speedY(1), m_vision(5*Environnement::scale)
{
    m_damage_hit = damage_hit;
    m_heal_sound = heal_sound;
    m_state      = new Patrol(this);
    m_guardWeapon = new Weapon(this, "Holy Banana", 10, 60, Weapon::infiniteReach);
}

void Guard::hit(CMover* m, int damage) { Character::hit(m, damage); }

void Guard::die(CMover* m) { setState(new Dead(this)); }

void Guard::fire(int angle) {}

bool Guard::process_fireball(float dx, float dy) { return false; }

void Guard::setState(State* state)
{
    //(facultatif: m_state->quit();)
    delete m_state;
    m_state = state;
    state->enter();
}

bool Guard::canSeeHunter()
{
    Hunter* h = getMaze()->getHunter();
    Vec2f   diffHunterGuard(h->_x - _x, h->_y - _y);
    return diffHunterGuard.norm() < m_vision;
}

void Guard::update()
{
    Character::update();
    m_state->update();
}