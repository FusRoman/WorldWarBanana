#include "Weapon.h"

#include <limits>

#include "Labyrinthe.h"

Sound* Weapon::onFire = new Sound("sons/hunter_fire.wav");
Sound* Weapon::onHit = new Sound("sons/hit_wall.wav");
Sound* Weapon::onTrigger = new Sound("sons/hunter_hit.wav");

const float Weapon::infiniteReach = std::numeric_limits<float>::max();

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
    Weapon(owner, "Banana Blaster", 10, 30, infiniteReach, onFire, onHit, onTrigger)
{}

Weapon::Weapon(CMover* owner, const std::string& name, 
        int damage, uint cooldown, float reach, 
        Sound* onFire, Sound* onHit, Sound* onTrigger):
    m_owner     (owner),
    m_name      (name),
    m_firedOnce (false),
    m_cooldown  (cooldown),
    m_lastFired (0),
    m_reach     (reach),
    m_damage    (damage),
    m_fire      (onFire),
    m_hit       (onHit),
    m_trigger   (onTrigger)
{
    assert(owner != nullptr, "owner must not be null");
}

Weapon::Weapon(const Weapon& w):
    m_owner     (w.m_owner),
    m_name      (w.m_name),
    m_firedOnce (false),
    m_cooldown  (w.m_cooldown),
    m_lastFired (0),
    m_reach     (w.m_reach),
    m_damage    (w.m_damage),
    m_fire      (w.m_fire),
    m_hit       (w.m_hit),
    m_trigger   (w.m_trigger)
{}

void Weapon::setOwner(CMover* owner)
{
    assertNeverFired(__FUNCTION__);
    assert(owner != nullptr, "owner must not be null");
    m_owner = owner;
}

void Weapon::setName(const std::string& name)
{
    assertNeverFired(__FUNCTION__);
    m_name = name;
}

void Weapon::setCooldown(uint cooldown)
{
    assertNeverFired(__FUNCTION__);
    m_cooldown = cooldown;
}

void Weapon::setReach(float reach)
{
    assertNeverFired(__FUNCTION__);
    assert(reach > 0, "Reach is negative or zero");
    m_reach = reach;
}

void Weapon::setDamage(int damage)
{
    assertNeverFired(__FUNCTION__);
    m_damage = damage;
}

void Weapon::setOnFire(Sound* onFire)
{
    assertNeverFired(__FUNCTION__);
    m_fire = onFire;
}

void Weapon::setOnHit(Sound* onHit)
{
    assertNeverFired(__FUNCTION__);
    m_hit = onHit;
}

void Weapon::setOnTrigger(Sound* onTrigger)
{
    assertNeverFired(__FUNCTION__);
    m_trigger = onTrigger;
}

void Weapon::fire(FireBall* fb, int angle)
{
    m_firedOnce = true;
    uint tick = CMover::tick();
    int ellapsed = tick - m_lastFired;
    if (ellapsed >= m_cooldown)
    {
        m_lastFired = tick;
        play(m_fire);
        fb->init(m_owner->_x, m_owner->_y, 10., angle, m_owner->_angle);
    }
    else
    {
        play(m_trigger);
    }
}

bool Weapon::process_fireball(FireBall* fb, double dx, double dy)
{
    // calculer la distance entre le chasseur et le lieu de l'explosion
    Labyrinthe* laby = m_owner->getMaze();
    Vec2i p = laby->realToGrid(m_owner->_x - fb->get_x(), m_owner->_y - fb->get_y());
    float dist2 = p.x * p.x + p.y * p.y;

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

    // Test sur le trésor
    if (m_owner->id() == 0 && fbp.x == laby->_treasor._x && fbp.y == laby->_treasor._y)
    {
        partie_terminee(true);
    }
    return false;
}
