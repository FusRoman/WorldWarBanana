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
    void         right_click(bool shift, bool control);
    virtual void die(CMover* m) override;
};
