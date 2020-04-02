#include "Guard.h"

#include "macros.h"

class Walking: public Guard::State
{
private:
    int  m_duration;
    int  m_lastDirectionUpdate;

    void updateDirection();

protected:
    virtual std::pair<Vec2f, float> newDirection() = 0;

public:
    Walking(Guard* g);
    virtual void update() override;
};

class Defense: public Walking
{
private:
public:
    Defense(Guard* g);
    virtual std::pair<Vec2f, float> newDirection() override;
    virtual void enter() override;
};

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

class Patrol: public Walking
{
public:
    Patrol(Guard* g);
    virtual std::pair<Vec2f, float> newDirection() override;
    virtual void enter() override;
};

class Dead: public Guard::State
{
public:
    Dead(Guard* g);
    virtual void update() override;
    virtual void enter() override;
};


/**************************************************************************************************
 * 
 * Walking
 * 
 *************************************************************************************************/

Walking::Walking(Guard* g): State(g) {}

void Walking::updateDirection()
{
    m_duration = randomInt(60, 300);
    m_lastDirectionUpdate = 0;
    auto _new = newDirection();
    m_guard->face(_new.first, _new.second);
}

void Walking::update()
{
    if (m_guard->canSeeHunter(false))
    {
        m_guard->setState(new Attack(m_guard));
    }
    else if (m_lastDirectionUpdate == m_duration)
    {
        updateDirection();
    }

    Labyrinthe* laby = m_guard->getMaze();
    Vec2f       nextPosition(m_guard->_x + m_guard->m_speedX, m_guard->_y + m_guard->m_speedY);
    Vec2i       gridPosition = laby->realToGrid(nextPosition.x, nextPosition.y);
    switch (laby->getCellType(m_guard, gridPosition.x, gridPosition.y))
    {
    case WALL:
    case TREASURE:
    case CMOVER:
        updateDirection();
        break;
    default:
        break;
    }
    m_guard->move(m_guard->m_speedX, m_guard->m_speedY);
    ++m_lastDirectionUpdate;
}

/**************************************************************************************************
 * 
 * Defense
 * 
 *************************************************************************************************/

Defense::Defense(Guard* g): Walking(g) {}

std::pair<Vec2f, float> Defense::newDirection()
{
    if (randomFloat(0., 1.) < 0.5)
    {
        Labyrinthe* maze = m_guard->getMaze();
        Vec2i p = maze->realToGrid(m_guard->_x, m_guard->_y);
        int bestX = 0;
        int bestY = 0;
        uint bestValue = maze->distanceFromTreasure(p.x, p.y);
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                uint tmp = maze->distanceFromTreasure(p.x + i, p.y + j);
                if (tmp < bestValue)
                {
                    bestValue   = tmp;
                    bestX       = i;
                    bestY       = j;
                }
            }
        }
        Vec2f unit(Vec2f(bestX, bestY).normalize());
        return std::pair<Vec2f, float>(unit, unit.angle());
    }
    else
    {
        return randomVector();
    }
    
}

void Defense::enter() {}


/**************************************************************************************************
 * 
 * Attack
 * 
 *************************************************************************************************/

Attack::Attack(Guard* g): State(g) {}

void Attack::update()
{
    if (!m_guard->canSeeHunter(true))
    {
        m_guard->setState(new Pursuit(m_guard));
    }
    else
    {
        if (m_guard->m_weapon.canFire())
        {
            m_guard->fire(0);
        }
    }
}
void Attack::enter() {}


/**************************************************************************************************
 * 
 * Pursuit
 * 
 *************************************************************************************************/

Pursuit::Pursuit(Guard* g): State(g) {}

void Pursuit::update() {}
void Pursuit::enter() {}


/**************************************************************************************************
 * 
 * Patrol
 * 
 *************************************************************************************************/

Patrol::Patrol(Guard* g): Walking(g) {}

std::pair<Vec2f, float> Patrol::newDirection()
{
    return randomVector();
}

void Patrol::enter() {}


/**************************************************************************************************
 * 
 * Dead
 * 
 *************************************************************************************************/

Dead::Dead(Guard* g): State(g) {}

void Dead::update() {}
void Dead::enter()
{
    m_guard->rester_au_sol();
    m_guard->getMaze()->free(m_guard);
}


/**************************************************************************************************
 * 
 * Guard
 * 
 *************************************************************************************************/

// Fonctions auxiliaires pour l'implémentation de DDA
// https://www.scratchapixel.com/lessons/advanced-rendering/introduction-acceleration-structure/grid
// Par rapport au tutoriel donné, gridMin vaudra toujours (0, 0) ici,
// donc rayOrigGrid défini comme rayOrigin - gridMin sera toujours égal à rayOrigin
namespace _Guard_private_
{
    // x pour t, y pour deltaT
    Vec2f initT(float rayOrigin, float rayDirection)
    {
        // Si rayDirection vaut 0, les résultats renvoyés seront +inf
        // Ce qui se comporte bien avec l'algorithme DDA par la suite
        // Donc pas besoin de protection
        float tmp = floor(rayOrigin / Environnement::scale);
        float delta = Environnement::scale / rayDirection;
        if (rayDirection < 0.)
        {
            delta = -delta;
        }
        else
        {
            tmp += 1;
        }
        
        return Vec2f(
            (tmp * Environnement::scale - rayOrigin) / rayDirection,
            delta
        );
    }

    // Renvoie true si origin peut voir la destination
    // (destination = origin + direction * tmax)
    // direction doit être unitaire
    // tmax doit être positif
    bool dda(CMover* mover, Vec2f origin, Vec2f direction, float tmax)
    {
        // Initialisation
        Labyrinthe* laby = mover->getMaze();
        float t = 0.;
        Vec2i cell(Labyrinthe::realToGrid(origin));
        Vec2f initX = initT(origin.x, direction.x);
        Vec2f initY = initT(origin.y, direction.y);
        Vec2f tv(initX.x, initY.x);
        Vec2f deltaT(initX.y, initY.y);

        while (true)
        {
            // Mise à jour
            if (tv.x < tv.y)
            {
                t = tv.x;
                tv.x += deltaT.x;
                (direction.x < 0.)? --cell.x : ++cell.x;
            }
            else
            {
                t = tv.y;
                tv.y += deltaT.y;
                (direction.y < 0.)? --cell.y : ++cell.y;
            }
            
            // Conditions de sortie
            if (t >= tmax)
            {
                // On a atteint la destination
                return true;
            }

            // Out-of-bounds / obstacle sur le chemin
            CellType c = laby->getCellType(mover, cell.x, cell.y);
            if (c != CellType::_EMPTY && c != CellType::CMOVER)
            {
                // Pour l'instant, on peut voir à travers les autres CMover
                return false;
            }
        }
    }
} // namespace _Guard_private_
using namespace _Guard_private_;

Sound* Guard::damage_hit = new Sound("sons/roblox_hit.wav");
Sound* Guard::heal_sound = new Sound("sons/heal_sound.wav");
Sound* Guard::fire_sound = new Sound("sons/pk_fire.wav");

const std::vector<const char*> Guard::modeles({"drfreak", "Marvin", "Potator", "garde", "Droid",
                                               "Lezard", "Samourai", "Serpent", "Squelette",
                                               "Blade"});

Guard::Guard(Labyrinthe* l, const char* modele, uint id, int maxpvs):
    Character       (l, modele, id, maxpvs), 
    m_speedX        (1), 
    m_speedY        (1), 
    m_vision        (10 * Environnement::scale),
    m_state         (new Defense(this)),
    m_toBeDeleted   (nullptr)
{
    m_damage_hit = damage_hit;
    m_heal_sound = heal_sound;
    m_weapon.setNbBalls(1);
    m_weapon.setCooldown(30);
    m_weapon.setOnFire(fire_sound);
}

Guard::Guard(Labyrinthe* l, int modele, uint id, int maxpvs):
    Guard(l, modeles.at(modele), id, maxpvs)
{}

Guard::~Guard()
{
    delete m_state;
}

void Guard::hit(CMover* m, int damage) { Character::hit(m, damage); }

void Guard::die(CMover* m) { setState(new Dead(this)); }

void Guard::setState(State* state)
{
    //(facultatif: m_state->quit();)
    m_toBeDeleted = m_state;
    m_state = state;
    state->enter();
}

bool Guard::canSeeHunter(bool _walk)
{
    Hunter* hunter = getMaze()->getHunter();
    Vec2f h(hunter->_x, hunter->_y);
    Vec2f g(_x, _y);
    Vec2f gh(h - g);
    float norm = gh.norm();

    bool see = norm < m_vision;
    if (see)
    {
        // Les gardes ne peuvent pas voir à travers les murs
        // Et leur champ de vision est limité à 180° sur les côtés
        // (d'où le produit scalaire)
        Vec2f facing(m_speedX, m_speedY);
        see = (gh.dot(facing) > 0)? dda(this, g, gh / norm, norm) : false;
    }

    if (_walk && see)
    {
        walk(gh / norm);
    }
    return see;
}

void Guard::face(const Vec2f& d)
{
    face(d, d.angle());
}

void Guard::face(const Vec2f& d, float radians)
{
    m_speedX = d.x;
    m_speedY = d.y;
    _angle   = radiansToDegrees(radians);
}

void Guard::walk(const Vec2f& d)
{
    face(d);
    move(d.x, d.y);
}

void Guard::walk(const Vec2f& d, float radians)
{
    face(d, radians);
    move(d.x, d.y);
}

void Guard::update()
{
    Character::update();
    if (m_toBeDeleted)
    {
        delete m_toBeDeleted;
        m_toBeDeleted = nullptr;
    }
    m_state->update();
}