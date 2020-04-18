#pragma once

#include "Character.h"
#include "Sound.h"
#include <stdio.h>

class Labyrinthe;

class Hunter: public Character
{
private:
    static Sound* _hunter_hit; // cri du chasseur touch�.
    static Sound* heal_sound;

public:
    Hunter(Labyrinthe* l, uint id);
    virtual void update() override;
    virtual void die(CMover* m) override;
    virtual void hit(CMover* m, int damage) override;
};
