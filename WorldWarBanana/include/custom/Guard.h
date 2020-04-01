#pragma once

#include <vector>

#include "Character.h"
#include "Labyrinthe.h"
#include "Sound.h"

class Labyrinthe;

class State {

    virtual void update(Guard& g)=0;
    virtual void enter(Guard& g)=0;

};


class Guard : public Character
{
private:
    static Sound* damage_hit;
    static Sound* heal_sound;
    int m_guardianSpeedX; // déplacement dx et dy du gardiens, utilisé en argument de move(dx, dy)
    int m_guardianSpeedY;
    bool m_defense; // indique si le gardien est en mode défense ou patrouille -> true si mode défense

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
