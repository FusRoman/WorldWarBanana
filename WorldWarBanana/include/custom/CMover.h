#pragma once

#include "Mover.h"
#include "Sound.h"
#include "utils.h"

class Labyrinthe;

/**
 * @brief Une extension de Mover qui servira de classe de base pour tous les autres Mover de ce
 * projet.
 */
class CMover: public Mover
{
protected:
    Labyrinthe* m_laby;
    const uint  m_id;

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
    inline Labyrinthe* getMaze() const { return m_laby; }

    /**
     * @brief Retourne la position du centre de l'objet.
     */
    virtual Vec2f getRealPosition() const;

    /**
     * @brief Retourne la case où se trouverait l'objet s'il se déplaçait de (dx, dy).
     */
    virtual Vec2i getGridPosition(float dx = 0., float dy = 0.) const;

    /**
     * @brief Renvoie l'ID de l'objet (par rapport à son labyrinthe).
     */
    inline uint id() const { return m_id; }

    /**
     * @brief Déplace l'objet du mieux possible.
     * "Glisse" sur les obstacles si l'angle le permet.
     */
    virtual bool move(double dx, double dy) override;

    /**
     * @brief Retourne vrai si l'objet est bloquant, faux sinon.
     */
    virtual bool block() const = 0;

    /**
     * @brief Appelé lorsque l'objet reçoit une boule de feu.
     * damage peut être négatif.
     */
    virtual void hit(CMover* culprit, int damage) = 0;
};
