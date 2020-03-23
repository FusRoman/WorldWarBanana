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
    uint            m_id;

public:
    CMover(int x, int y, Labyrinthe* l, const char* modele, uint id):
        Mover   (x, y, l, modele),
        m_laby  (l),
        m_id    (id)
    {}

    inline uint id() const
    {
        return m_id;
    }
};
