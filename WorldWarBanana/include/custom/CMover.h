#pragma once

#include "Mover.h"
#include "Sound.h"
#include "utils.h"

class Labyrinthe;

/**
 * @brief Ajoute un pointeur de labyrinthe par rapport à la classe Mover.
 */ 
class CMover: public Mover
{
protected:
    Labyrinthe*     m_laby;
    uint            m_id;

public:
    /**
     * @brief Constructeur
     * @param x Abscisse de départ
     * @param y Ordonnée de départ
     * @param l Labyrinthe propriétaire
     * @param modele Modèle 3D à utiliser, 0 si aucun
     * @param id ID de l'objet, défini par l
     */
    CMover(int x, int y, Labyrinthe* l, const char* modele, uint id);

    /**
     * @brief Renvoie le labyrinthe dans lequel cet objet se situe
     */
    inline Labyrinthe* getMaze() const
    {
        return m_laby;
    }

    /**
     * @brief Renvoie l'ID de l'objet (par rapport à son labyrinthe).
     */
    inline uint id() const
    {
        return m_id;
    }

    /**
     * @brief Déplace l'objet du mieux possible.
     * L'objet est considéré comme solide, i.e. les autrs ne pourront pas passer dessus.
     * "Glisse" sur les obstacles si l'angle le permet.
     */
    virtual bool move(double dx, double dy) override;
};
