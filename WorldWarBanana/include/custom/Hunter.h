#pragma once

#include "CMover.h"
#include "Sound.h"
#include <stdio.h>

class Labyrinthe;

class Hunter: public CMover
{
private:
    static Sound* _hunter_fire; // bruit de l'arme du chasseur.
    static Sound* _hunter_hit;  // cri du chasseur touch�.
    static Sound* _wall_hit;    // on a tap� un mur.

    // accepte ou non un deplacement.
    bool move_aux(double dx, double dy);

public:

    Hunter(Labyrinthe* l, uint id);
    // ne bouger que dans une case vide (on 'glisse' le long des obstacles)
    bool move(double dx, double dy)
    {
        return move_aux(dx, dy) || move_aux(dx, 0.0) || move_aux(0.0, dy);
    }
    // le chasseur ne pense pas!
    void update(void){};
    // fait bouger la boule de feu (ceci est une exemple, � vous de traiter les
    // collisions sp�cifiques...)
    bool process_fireball(float dx, float dy);
    // tire sur un ennemi (clic gauche).
    void fire(int angle_vertical);
    // clic droit.
    void right_click(bool shift, bool control);
};
