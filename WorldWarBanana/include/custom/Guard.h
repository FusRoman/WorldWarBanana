#pragma once

#include <vector>

#include "Character.h"
#include "Sound.h"

class Labyrinthe;

class Guard: public Character
{
private:
    static Sound* damage_hit;
    static Sound* heal_sound;

public:
    static const std::vector<const char*> modeles;

    Guard(Labyrinthe* l, const char* modele, uint id);
    Guard(Labyrinthe* l, int modele, uint id);

    // mon gardien pense tr�s mal!
    void update(void){};

    // ne sait pas tirer sur un ennemi.
    void fire(int angle_vertical) {}
    // quand a faire bouger la boule de feu...
    bool process_fireball(float dx, float dy) { return false; }

    virtual void die(CMover* m) override;
};
