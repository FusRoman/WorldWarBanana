#include "Guard.h"

#include "Heap.h"
#include "Labyrinthe.h"

/**************************************************************************************************
 *
 * A* (pour Pursuit)
 *
 *************************************************************************************************/

namespace _Guard_private_
{

    const static int A_STAR_LIMIT = 10;

    class Node
    {
    public:
        int positionX;
        int positionY;
        int dx;
        int dy;
        int cost_g;
        int cost_h;
        int cost_f;
        int parent;
        int idNode;

        Node(int posX, int posY, int dx, int dy, int cost_g, int cost_h, int cost_f, int id,
             int parent):
            positionX(posX),
            positionY(posY),
            dx(dx),
            dy(dy),
            cost_g(cost_g),
            cost_h(cost_h),
            cost_f(cost_f),
            parent(parent),
            idNode(id)
        {
        }

        Node(const Node& u):
            positionX(u.positionX),
            positionY(u.positionY),
            dx(u.dx),
            dy(u.dy),
            cost_g(u.cost_g),
            cost_h(u.cost_h),
            cost_f(u.cost_f),
            parent(u.parent),
            idNode(u.idNode)
        {
        }

        friend inline bool operator<(const Node& n1, const Node& n2)
        {
            return n1.cost_f < n2.cost_f;
        }

        friend inline bool operator==(const Node& n1, const Node& n2)
        {
            return n1.positionX == n2.positionX && n1.positionY == n2.positionY;
        }

        Node& operator=(const Node& n)
        {
            positionX = n.positionX;
            positionY = n.positionY;
            dx        = n.dx;
            dy        = n.dy;
            cost_g    = n.cost_g;
            cost_h    = n.cost_h;
            cost_f    = n.cost_f;
            parent    = n.parent;
            idNode    = n.idNode;
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& os, const Node& v)
        {
            return os << " (x = " << v.positionX << ", y = " << v.positionY << ", dx = " << v.dx
                      << ", dy = " << v.dy << ", g = " << v.cost_g << ", h = " << v.cost_h
                      << ", f = " << v.cost_f << ", mon id : " << v.idNode
                      << ", mon parent : " << v.parent << ")";
        }
    };

    int heuristique(Node& u, Node& v)
    {
        // return u.positionX * v.positionX + u.positionY * v.positionY;
        int x = u.positionX - v.positionX;
        int y = u.positionY - v.positionY;
        return x * x + y * y;
    }

    void addNeighbor(std::forward_list<Node>& neighbor_list, std::vector<Node>& parent, Node& u,
                     int i, int j, Node& end)
    {
        Node newNode(u.positionX + i, u.positionY + j, i, j, 0, 0, 0, parent.size(), u.idNode);
        newNode.cost_g = u.cost_g + heuristique(newNode, u);
        newNode.cost_h = heuristique(newNode, end);
        newNode.cost_f = newNode.cost_g + newNode.cost_h;
        parent.push_back(newNode);
        neighbor_list.push_front(newNode);
    }

    void computeNeighbor(CMover& m, Node& u, Node& end, std::forward_list<Node>& neighbor,
                         std::vector<Node>& parent, CMover* blockingMover)
    {
        Labyrinthe* l = m.getMaze();
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                if (i != 0 || j != 0)
                {
                    switch (l->getCellType(u.positionX + i, u.positionY + j))
                    {
                    case _EMPTY:
                        addNeighbor(neighbor, parent, u, i, j, end);
                        break;
                    case CMOVER:
                        if (l->getMover(u.positionX + i, u.positionY + j) != blockingMover)
                        {
                            addNeighbor(neighbor, parent, u, i, j, end);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }

    std::list<Node> findShortestPath(CMover& m, Node end, CMover* blockingMover)
    {
        std::vector<Node> parent;
        std::set<Node>    closedList;
        Heap<Node>        openList;
        Vec2i             posGuard(Labyrinthe::realToGrid(m._x, m._y));
        Node              start(posGuard.x, posGuard.y, 0, 0, 0, 0, 0, 0, -1);
        openList.push(start);
        parent.push_back(start);
        int round = 0;
        while (!openList.empty() && round < A_STAR_LIMIT)
        {
            Node u = openList.top();
            openList.pop();
            if (u == end)
            {
                std::list<Node> path;
                path.push_front(u);
                int Idparent = u.parent;
                while (Idparent != -1)
                {
                    Node& currentParent = parent.at(Idparent);
                    path.push_front(currentParent);
                    Idparent = currentParent.parent;
                }
                return path;
            }
            std::forward_list<Node> neighbor;
            computeNeighbor(m, u, end, neighbor, parent, blockingMover);
            for (Node& v: neighbor)
            {
                Node* otherV = openList.contains(v);
                if (closedList.find(v) == closedList.end())
                {
                    if (otherV != nullptr)
                    {
                        if (otherV->cost_f > v.cost_f)
                        {
                            *otherV = v;
                        }
                    }
                    else
                    {
                        openList.push(v);
                    }
                }
            }
            closedList.insert(u);
            ++round;
        }
        return std::list<Node>();
    }
} // namespace _Guard_private_
using namespace _Guard_private_;

/**************************************************************************************************
 *
 * Déclaration des états
 *
 *************************************************************************************************/

class Walking: public Guard::State
{
private:
    int m_duration;
    int m_lastDirectionUpdate;

    void updateDirection();

protected:
    virtual std::pair<Vec2f, float> newDirection() = 0;

public:
    Walking(Guard* g, bool isDefault);
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
    std::list<Node>           m_pursuitPath;
    std::list<Node>::iterator m_nextPosition;
    int                       m_dest_x;
    int                       m_dest_y;

public:
    Pursuit(Guard* g, int x,
            int y); // x et y sont les coordonnées du noeud initial pour l'algorithme de pathFinding
    virtual void update() override;
    virtual void enter() override;
};

class Damaged: public Guard::State
{
private:
    uint m_start;

public:
    Damaged(Guard* g);
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

Walking::Walking(Guard* g, bool isDefault): State(g, isDefault) {}

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
        return;
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

Defense::Defense(Guard* g): Walking(g, true) {}

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

Attack::Attack(Guard* g): State(g, false) {}

void Attack::update()
{
    if (!m_guard->canSeeHunter(true))
    {
        Vec2i coordGrid = Labyrinthe::realToGrid(m_guard->getMaze()->getHunter()->_x,
                                                 m_guard->getMaze()->getHunter()->_y);
        m_guard->setState(new Pursuit(m_guard, coordGrid.x, coordGrid.y));
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

Pursuit::Pursuit(Guard* g, int x, int y): State(g, false), m_dest_x(x), m_dest_y(y)
{
    Node end(m_dest_x, m_dest_y, 0, 0, 0, 0, 0, 0, 0);
    m_pursuitPath = findShortestPath(*g, end, nullptr);
    if (!m_pursuitPath.empty())
    {
        m_nextPosition = m_pursuitPath.begin();
        ++m_nextPosition;
    }
}

void Pursuit::update()
{
    if (m_guard->canSeeHunter(false))
    {
        m_guard->setState(new Attack(m_guard));
        return;
    }
    else if (m_pursuitPath.empty() || m_nextPosition == m_pursuitPath.end())
    {
        m_guard->enterDefaultState();
        return;
    }
    else
    {

        Vec2i coordGuard = Labyrinthe::realToGrid(m_guard->_x, m_guard->_y);
        Vec2i nextCase(coordGuard.x + m_nextPosition->dx, coordGuard.y + m_nextPosition->dy);
        if (!m_guard->getMaze()->canGoTo(m_guard, nextCase.x, nextCase.y))
        {
            // si la prochaine est case est bloqué a cause d'un mover, on refait un a*
            // pour update le chemin
            CMover* blockingMover = m_guard->getMaze()->getMover(nextCase.x, nextCase.y);
            Node    end(m_dest_x, m_dest_y, 0, 0, 0, 0, 0, 0, 0);
            m_pursuitPath = findShortestPath(*m_guard, end, blockingMover);
            if (m_pursuitPath.empty())
            {
                m_guard->enterDefaultState();
            }
            else
            {
                m_nextPosition = m_pursuitPath.begin();
                ++m_nextPosition;
            }
        }
        Vec2f unit(Vec2f(m_nextPosition->dx, m_nextPosition->dy).normalize());
        m_guard->walk(unit, unit.angle());
        Vec2i newCoordGuard = Labyrinthe::realToGrid(m_guard->_x, m_guard->_y);
        // On passe au prochain du noeud du chemin si le gardien a changé de case dans le
        // labyrinthe.
        if (newCoordGuard.x == m_nextPosition->positionX &&
            newCoordGuard.y == m_nextPosition->positionY)
        {
            ++m_nextPosition;
        }
    }
}

void Pursuit::enter() {}

/**************************************************************************************************
 *
 * Patrol
 *
 *************************************************************************************************/

Patrol::Patrol(Guard* g): Walking(g, true) {}

std::pair<Vec2f, float> Patrol::newDirection() { return randomVector(); }

void Patrol::enter() {}

/**************************************************************************************************
 *
 * Damaged
 *
 *************************************************************************************************/

Damaged::Damaged(Guard* g): State(g, false), m_start(FireBallDX::tick()) {}

void Damaged::update()
{
    if (FireBallDX::tick() - m_start >= 90)
    {
        m_guard->enterDefaultState();
    }
}

void Damaged::enter() { m_guard->tomber(); }

/**************************************************************************************************
 *
 * Dead
 *
 *************************************************************************************************/

Dead::Dead(Guard* g): State(g, false) {}

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
        float tmp   = floor(rayOrigin / Environnement::scale);
        float delta = Environnement::scale / rayDirection;
        if (rayDirection < 0.)
        {
            delta = -delta;
        }
        else
        {
            tmp += 1;
        }

        return Vec2f((tmp * Environnement::scale - rayOrigin) / rayDirection, delta);
    }

    // Renvoie true si origin peut voir la destination
    // (destination = origin + direction * tmax)
    // direction doit être unitaire
    // tmax doit être positif
    bool dda(CMover* mover, Vec2f origin, Vec2f direction, float tmax)
    {
        // Initialisation
        Labyrinthe* laby = mover->getMaze();
        float       t    = 0.;
        Vec2i       cell(Labyrinthe::realToGrid(origin));
        Vec2f       initX = initT(origin.x, direction.x);
        Vec2f       initY = initT(origin.y, direction.y);
        Vec2f       tv(initX.x, initY.x);
        Vec2f       deltaT(initX.y, initY.y);

        while (true)
        {
            // Mise à jour
            if (tv.x < tv.y)
            {
                t = tv.x;
                tv.x += deltaT.x;
                (direction.x < 0.) ? --cell.x : ++cell.x;
            }
            else
            {
                t = tv.y;
                tv.y += deltaT.y;
                (direction.y < 0.) ? --cell.y : ++cell.y;
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

Sound* Guard::damage_hit = new Sound("sons/roblox_hit.wav");
Sound* Guard::heal_sound = new Sound("sons/heal_sound.wav");
Sound* Guard::fire_sound = new Sound("sons/pk_fire.wav");
Sound* Guard::death_sound = new Sound("sons/guard_die.wav");

const std::vector<const char*> Guard::modeles({"drfreak", "Marvin", "Potator", "garde", "Droid",
                                               "Lezard", "Samourai", "Serpent", "Squelette",
                                               "Blade"});

Guard::Guard(Labyrinthe* l, const char* modele, uint id, int maxpvs):
    Character(l, modele, id, maxpvs),
    m_speedX(1),
    m_speedY(1),
    m_vision(15 * Environnement::scale),
    m_state(new Patrol(this)),
    m_toBeDeleted(nullptr),
    m_defense(false)
{
    m_damage_hit = damage_hit;
    m_heal_sound = heal_sound;
    m_weapon.setNbBalls(1);
    m_weapon.setCooldown(30);
    m_weapon.setOnFire(fire_sound);
}

Guard::Guard(Labyrinthe* l, int modele, uint id, int maxpvs):
    Guard(l, modeles.at(modele), id, maxpvs)
{
}

Guard::~Guard() { delete m_state; }

void Guard::affectToDefense(bool defense)
{
    if (m_defense != defense)
    {
        m_defense = defense;
        if (m_state->m_default)
        {
            enterDefaultState();
        }
    }
}

void Guard::hit(CMover* m, int damage)
{
    Character::hit(m, damage);
    if (damage > 0 && !isDead())
    {
        setState(new Damaged(this));
    }
}

void Guard::die(CMover* m) 
{ 
    play(death_sound);
    setState(new Dead(this)); 
}

void Guard::setState(State* state)
{
    m_toBeDeleted = m_state;
    m_state       = state;
    state->enter();
}

void Guard::enterDefaultState()
{
    if (m_defense)
    {
        setState(new Defense(this));
    }
    else
    {
        setState(new Patrol(this));
    }
}

bool Guard::canSeeHunter(bool _walk)
{
    Hunter* hunter = getMaze()->getHunter();
    Vec2f   h(hunter->_x, hunter->_y);
    Vec2f   g(_x, _y);
    Vec2f   gh(h - g);
    float   norm = gh.norm();

    bool see = norm < m_vision;
    if (see)
    {
        // Les gardes ne peuvent pas voir à travers les murs
        // Et leur champ de vision est limité à 180° sur les côtés
        // (d'où le produit scalaire)
        // Vec2f facing(m_speedX, m_speedY);
        // see = (gh.dot(facing) > 0) ? dda(this, g, gh / norm, norm) : false;
        see = dda(this, g, gh / norm, norm);
    }

    if (_walk && see)
    {
        walk(gh / norm);
    }
    return see;
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
    if (m_toBeDeleted)
    {
        delete m_toBeDeleted;
        m_toBeDeleted = nullptr;
    }
    m_state->update();
}