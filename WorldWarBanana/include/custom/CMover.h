#pragma once

#include "Mover.h"
#include "Labyrinthe.h"

/**
 * @brief Ajoute un pointeur de labyrinthe par rapport à la classe Mover.
 */ 
class CMover: public Mover
{
protected:
    Labyrinthe*     m_laby;

public:
    CMover(int x, int y, Labyrinthe* l, const char* modele):
        Mover(x, y, l, modele),
        m_laby(l)
    {}
};
