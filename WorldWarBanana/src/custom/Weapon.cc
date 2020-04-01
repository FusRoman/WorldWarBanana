#include "Weapon.h"

#include <limits>

#include "FireBallDX.h"
#include "Labyrinthe.h"

#define NEVER_FIRED assertNeverFired(__FUNCTION__)

Sound* Weapon::onFire    = new Sound("sons/hunter_fire.wav");
Sound* Weapon::onHit     = new Sound("sons/hit_wall.wav");
Sound* Weapon::onTrigger = new Sound("sons/hunter_hit.wav");

const uint Weapon::maxNbBalls = 10;

const float Weapon::infiniteReach = std::numeric_limits<float>::max();

FireBallDX* Weapon::nextFireBall()
{
    FireBallDX* r = nullptr;
    if (m_index != 0)
    {
        r = m_owner->getMaze()->getFireBall(m_index - 1);
    }
    m_index = (m_index + 1) % maxNbBalls;
    return r;
}

void Weapon::assert(bool condition, const std::string& msg)
{
    if (!condition)
    {
        std::cerr << "[ERROR] Weapon: " << msg << std::endl;
        exit(1);
    }
}

void Weapon::assertNeverFired(const std::string& msg)
{
    assert(!m_firedOnce, msg + ": can not change a weapon that is being used");
}

Weapon::Weapon(CMover* owner):
    Weapon(owner, "Banana Blaster", 10, 0, infiniteReach, onFire, onHit, onTrigger)
{
    /*setNbBalls(11);
    setAngle(5);*/
    setNbBalls(3);
    setAngle(15);
}

Weapon::Weapon(CMover* owner, const std::string& name, int damage, uint cooldown, float reach,
               Sound* onFire, Sound* onHit, Sound* onTrigger):
    m_owner(owner),
    m_name(name),
    m_firedOnce(false),
    m_cooldown(cooldown),
    m_lastFired(0),
    m_reach(reach),
    m_damage(damage),
    m_nbballs(1),
    m_angle(0),
    m_fire(onFire),
    m_hit(onHit),
    m_trigger(onTrigger),
    m_index(0)
{
    assert(owner != nullptr, "owner must not be null");
}

Weapon::Weapon(const Weapon& w):
    m_owner(w.m_owner),
    m_name(w.m_name),
    m_firedOnce(false),
    m_cooldown(w.m_cooldown),
    m_lastFired(0),
    m_reach(w.m_reach),
    m_damage(w.m_damage),
    m_nbballs(w.m_nbballs),
    m_angle(w.m_angle),
    m_fire(w.m_fire),
    m_hit(w.m_hit),
    m_trigger(w.m_trigger),
    m_index(w.m_index)
{
}

void Weapon::setOwner(CMover* owner)
{
    NEVER_FIRED;
    assert(owner != nullptr, "owner must not be null");
    m_owner = owner;
}

void Weapon::setName(const std::string& name)
{
    NEVER_FIRED;
    m_name = name;
}

void Weapon::setCooldown(uint cooldown)
{
    NEVER_FIRED;
    m_cooldown = cooldown;
}

void Weapon::setReach(float reach)
{
    NEVER_FIRED;
    assert(reach > 0, "Reach is negative or zero");
    m_reach = reach;
}

void Weapon::setDamage(int damage)
{
    NEVER_FIRED;
    m_damage = damage;
}

void Weapon::setOnFire(Sound* onFire)
{
    NEVER_FIRED;
    m_fire = onFire;
}

void Weapon::setOnHit(Sound* onHit)
{
    NEVER_FIRED;
    m_hit = onHit;
}

void Weapon::setOnTrigger(Sound* onTrigger)
{
    NEVER_FIRED;
    m_trigger = onTrigger;
}

void Weapon::setNbBalls(uint nbballs)
{
    NEVER_FIRED;
    assert(nbballs <= maxNbBalls + 1, std::string("Number of balls (") + std::to_string(nbballs) +
                                          ") greater than allowed (" +
                                          std::to_string(maxNbBalls + 1) + ")");
    assert(nbballs % 2 == 1, "Only odd numbers of balls allowed");
    m_nbballs = nbballs;
}

void Weapon::setAngle(int angle)
{
    NEVER_FIRED;
    m_angle = angle;
}

void Weapon::fire(int angle)
{
    m_firedOnce   = true;
    uint tick     = CMover::tick();
    int  ellapsed = tick - m_lastFired;
    if (ellapsed >= m_cooldown)
    {
        m_lastFired = tick;
        play(m_fire);
        if (m_owner->id() == 0)
        {
            int a = m_owner->_angle - m_angle * (m_nbballs / 2);
            for (uint i = 0; i < m_nbballs; ++i)
            {
                FireBallDX* dx = nextFireBall();
                if (dx)
                {
                    dx->setWeapon(this);
                    dx->_fb->init(m_owner->_x, m_owner->_y, 10., angle, a);
                }
                else
                {
                    m_owner->_fb->init(m_owner->_x, m_owner->_y, 10., angle, a);
                }
                a += m_angle;
            }
        }
        else
        {
            if (m_nbballs > 1)
            {
                WARNING("Mover "
                        << m_owner->id()
                        << ": only the player can fire more than one fire ball at the same time");
            }
            m_owner->_fb->init(m_owner->_x, m_owner->_y, 10., angle, m_owner->_angle);
        }
    }
    else
    {
        play(m_trigger);
    }
}

bool Weapon::process_fireball(FireBall* fb, double dx, double dy)
{
    // calculer la distance entre le chasseur et le lieu de l'explosion
    Labyrinthe* laby  = m_owner->getMaze();
    Vec2i       p     = laby->realToGrid(m_owner->_x - fb->get_x(), m_owner->_y - fb->get_y());
    float       dist2 = p.x * p.x + p.y * p.y;

    // On a les mêmes permissions de déplacement que m_owner
    Vec2i fbp = laby->realToGrid(fb->get_x() + dx, fb->get_y() + dy);
    if (laby->canGoTo(m_owner, fbp.x, fbp.y))
    {
        return true;
    }

    // Faire exploser la boule de feu avec un bruit fonction de la distance
    if (m_hit)
    {
        m_hit->play(max(0., 1. - dist2 / 1200.));
    }

    switch (laby->getCellType(fbp.x, fbp.y))
    {
    case TREASURE:
    {
        if (m_owner->id() == 0)
        {
            partie_terminee(true);
        }
        break;
    }

    case CMOVER:
    {
        CMover* target = laby->getMover(fbp.x, fbp.y);
        target->hit(m_owner, m_damage);
        break;
    }

    default:
        break;
    }
    return false;
}
