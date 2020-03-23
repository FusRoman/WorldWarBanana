#pragma once

#include <vector>

#include "CMover.h"

class Labyrinthe;

class Guard: public CMover
{
public:
    static const std::vector<const char*> modeles;

    Guard(Labyrinthe* l, const char* modele, uint id): CMover(120, 80, l, modele, id) {}
    Guard(Labyrinthe* l, int modele, uint id): CMover(120, 80, l, modeles.at(modele), id) {}

    // mon gardien pense tr�s mal!
    void update(void){};
    // et ne bouge pas!
    bool move(double dx, double dy) { return false; }
    // ne sait pas tirer sur un ennemi.
    void fire(int angle_vertical) {}
    // quand a faire bouger la boule de feu...
    bool process_fireball(float dx, float dy) { return false; }
};
