#ifndef GARDIEN_H
#define GARDIEN_H

#include <vector>

#include "Mover.h"

class Labyrinthe;

class Gardien : public Mover
{
public:
    static const std::vector<const char*> modeles;

    Gardien(Labyrinthe* l, const char* modele): Mover(120, 80, l, modele) {}
    Gardien(Labyrinthe* l, int modele): Mover(120, 80, l, modeles.at(modele)) {}

    // mon gardien pense tr�s mal!
    void update(void){};
    // et ne bouge pas!
    bool move(double dx, double dy) { return false; }
    // ne sait pas tirer sur un ennemi.
    void fire(int angle_vertical) {}
    // quand a faire bouger la boule de feu...
    bool process_fireball(float dx, float dy) { return false; }
};

#endif
