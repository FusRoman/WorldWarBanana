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

public:

    Hunter(Labyrinthe* l, uint id);
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
