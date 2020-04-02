#pragma once

#include <vector>

#include "Character.h"
#include "Labyrinthe.h"
#include "Sound.h"
#include "utils.h"
#include "Hunter.h"

class Labyrinthe;




class Guard : public Character
{
private:
    class State {
    protected:
        Guard* m_guard;

    public:
        State(Guard* g) : m_guard(g) {}
        virtual void update()=0;
        virtual void enter()=0;
        virtual ~State(){}
    };

    friend class Walking;
    friend class Patrol;
    friend class Defense;
    friend class Dead;
    friend class Attack;
    friend class Pursuit;
    friend class Damaged;

    static Sound* damage_hit;
    static Sound* heal_sound;
    static Sound* fire_sound;

    float m_speedX; // déplacement dx et dy du gardiens, utilisé en argument de move(dx, dy)
    float m_speedY;
    float m_vision; // distance de vision maximale du gardiens
    State* m_state; // état de l'automate du gardien
    State* m_toBeDeleted; // ancien état de l'automate, à détruire (on ne peut pas le faire dans setState)

    void setState(State* state);
    bool canSeeHunter(bool _walk);
    void face(const Vec2f& d);
    void face(const Vec2f& d, float radians);
    void walk(const Vec2f& d);
    void walk(const Vec2f& d, float radians);

public:
    static const std::vector<const char*> modeles;

    Guard(Labyrinthe* l, const char* modele, uint id, int maxpvs);
    Guard(Labyrinthe* l, int modele, uint id, int maxpvs);
    virtual ~Guard();

    virtual void hit(CMover* m, int damage);
    virtual void die(CMover* m) override;
    virtual void update() override;
};
