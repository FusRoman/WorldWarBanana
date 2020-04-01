#pragma once

#include <vector>

#include "Character.h"
#include "Labyrinthe.h"
#include "Sound.h"
#include "utils.h"

class Labyrinthe;




class Guard : public Character
{
private:
    static Sound* damage_hit;
    static Sound* heal_sound;
    float m_speedX; // déplacement dx et dy du gardiens, utilisé en argument de move(dx, dy)
    float m_speedY;
    class State {

        protected:
            Guard* m_guard;

        public:
            State(Guard* g) : m_guard(g) {}
            virtual void update()=0;
            virtual void enter()=0;

    };
    State* m_state; // état de l'automate du gardien
    friend class Patrol;
    friend class Defense;
    friend class Dead;
    friend class Attack;
    friend class Pursuit;
    friend class Damage;

    void setState(State* state);

public:
    static const std::vector<const char*> modeles;

    Guard(Labyrinthe* l, const char* modele, uint id);
    Guard(Labyrinthe* l, int modele, uint id);

    // ne sait pas tirer sur un ennemi.
    void fire(int angle_vertical);
    // quand a faire bouger la boule de feu...
    bool process_fireball(float dx, float dy);

    // Le tombe tombe au sol pour un instant
    virtual void hit(CMover* m, int damage);
    virtual void die(CMover* m) override;
    virtual void update() override;


};
