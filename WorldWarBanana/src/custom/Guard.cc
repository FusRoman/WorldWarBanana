#include "Guard.h"

#include "macros.h"

typedef struct Node Node;

struct Node
{
    Vec2i position;
    int   cout;
    int   heuristique;
    Node* father;

    Node(Vec2i pos, int cout, int heuristique, Node* father):
        position(pos), cout(cout), heuristique(heuristique), father(father)
    {
    }

    friend inline bool operator< (const Node& n1, const Node& n2){
        return n1.heuristique < n2.heuristique;
    }

    friend inline bool operator== (const Node &n1, const Node& n2){
        return n1.position == n2.position;
    }
};

bool compareNode(Node n1, Node n2) { return n1.heuristique > n2.heuristique; }

int heuristique(Node& u, Node& v)
{
    return u.position.x * v.position.x + u.position.y * v.position.y;
}

void computeNeighbor(CMover& m, Node& u, Node& end, std::forward_list<Node>& neighbor)
{
    Labyrinthe* l = m.getMaze();
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            if (i != 0 || j != 0)
            {
                if (l->canGoTo(&m, u.position.x + i, u.position.y + j))
                {
                    Vec2i newPos(u.position.x + i, u.position.y + j);
                    Node newNode(newPos, u.cout + 1, heuristique(u, end), &u);
                    neighbor.push_front(newNode);
                }
            }
        }
    }
}

std::forward_list<Node> findShortestPath(CMover& m, Node end)
{
    std::set<Node>    closedList;
    Heap<Node> openList;
    Vec2i             startCoord = Labyrinthe::realToGrid(m._x, m._y);
    Node              start      = {startCoord, 0, 0, nullptr};
    start.heuristique            = heuristique(start, end);
    openList.push(start);

    while (!openList.empty())
    {
        Node u = openList.top();
        std::forward_list<Node> neighbor;
        computeNeighbor(m, u, end, neighbor);
        for(Node v: neighbor){
            if(!(  ))
        }
    }
}

class Walking: public Guard::State
{
private:
    int m_duration;
    int m_lastDirectionUpdate;

    void updateDirection();

protected:
    virtual std::pair<Vec2f, float> newDirection() = 0;

public:
    Walking(Guard* g);
    virtual void update() override;
};

class Patrol: public Walking
{
public:
    Patrol(Guard* g);
    virtual std::pair<Vec2f, float> newDirection() override;
    virtual void                    enter() override;
};

class Defense: public Walking
{
private:
public:
    Defense(Guard* g);
    virtual std::pair<Vec2f, float> newDirection() override;
    virtual void                    enter() override;
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
    m_duration            = randomInt(60, 300);
    m_lastDirectionUpdate = 0;
    auto _new             = newDirection();
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
        Labyrinthe* maze      = m_guard->getMaze();
        Vec2i       p         = maze->realToGrid(m_guard->_x, m_guard->_y);
        int         bestX     = 0;
        int         bestY     = 0;
        uint        bestValue = maze->distanceFromTreasure(p.x, p.y);
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                uint tmp = maze->distanceFromTreasure(p.x + i, p.y + j);
                if (tmp < bestValue)
                {
                    bestValue = tmp;
                    bestX     = i;
                    bestY     = j;
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

std::pair<Vec2f, float> Patrol::newDirection() { return randomVector(); }

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

Sound* Guard::damage_hit = new Sound("sons/roblox_hit.wav");
Sound* Guard::heal_sound = new Sound("sons/heal_sound.wav");
Sound* Guard::fire_sound = new Sound("sons/pk_fire.wav");

const std::vector<const char*> Guard::modeles({"drfreak", "Marvin", "Potator", "garde", "Droid",
                                               "Lezard", "Samourai", "Serpent", "Squelette",
                                               "Blade"});

Guard::Guard(Labyrinthe* l, const char* modele, uint id):
    Character(120, 80, l, modele, id),
    m_speedX(1),
    m_speedY(1),
    m_vision(10 * Environnement::scale),
    m_state(new Defense(this)),
    m_toBeDeleted(nullptr)
{
    m_damage_hit = damage_hit;
    m_heal_sound = heal_sound;
    m_weapon.setNbBalls(1);
    m_weapon.setCooldown(30);
    m_weapon.setOnFire(fire_sound);
}

Guard::Guard(Labyrinthe* l, int modele, uint id): Guard(l, modeles.at(modele), id) {}

Guard::~Guard() { delete m_state; }

void Guard::hit(CMover* m, int damage) { Character::hit(m, damage); }

void Guard::die(CMover* m) { setState(new Dead(this)); }

void Guard::setState(State* state)
{
    m_toBeDeleted = m_state;
    m_state       = state;
    state->enter();
}

bool Guard::canSeeHunter(bool _walk)
{
    Hunter* h = getMaze()->getHunter();
    Vec2f   diffHunterGuard(h->_x - _x, h->_y - _y);
    float   norm = diffHunterGuard.norm();
    if (_walk)
    {
        walk(diffHunterGuard / norm);
    }
    return norm < m_vision;
}

void Guard::face(const Vec2f& d) { face(d, d.angle()); }

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