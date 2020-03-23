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
    int             m_pv;
    Sound*          m_damage_hit;
    Sound*          m_heal_sound;

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
     * @brief Renvoie l'ID de l'objet (par rapport à son labyrinthe).
     */
    inline uint id() const
    {
        return m_id;
    }

    /**
     * @brief Retourne le nombre de PVs restants de l'objet.
     */
    inline const int getPV() const {
        return m_pv;
    }

    /**
     * @brief Blesse l'objet.
     * Si damage est négatif, il sera au contraire soigné.
     */
    void hurt(CMover* m, int damage);

    /**
     * @brief Fonction appelée quand l'objet meurt.
     * Peut être utilisée même si les PVs ne sont pas à 0 pour tuer instantanément l'objet.
     */
    virtual void die() =0;

    /**
     * @brief Déplace l'objet du mieux possible.
     * "Glisse" sur les obstacles si l'angle le permet.
     */
    virtual bool move(double dx, double dy) override;
};
