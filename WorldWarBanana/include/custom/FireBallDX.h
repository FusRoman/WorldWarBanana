#pragma once

#include "CMover.h"
#include "Weapon.h"

/**
 * @brief Une classe dont le seul but est de permettre au joueur de lancer plusieurs boules de feu.
 * Classe auxiliaire à Weapon : elle ne fait rien d'elle-même.
 */
class FireBallDX: public CMover
{
private:
    static uint m_tick;
    Weapon*     m_weapon;

public:
    /**
     * @brief Renvoie le nombre d'updates depuis le début du jeu.
     * Cette méthode n'a évidemment pas grand-chose à faire dans cette classe,
     * mais puisque le labyrinthe n'a pas de méthode update, il fallait que ce soit
     * un Mover qui se charge de mettre ça à jour.
     * Or par défaut seul le joueur est toujours présent en jeu, mais sa méthode update
     * n'est jamais appelée. Maintenant que les FireBallDX ont été créées et qu'elles sont
     * toujours présentes, on met cette fonction dans cette classe.
     */
    inline static uint tick() { return FireBallDX::m_tick; }

    FireBallDX(Labyrinthe* laby, uint id);

    /**
     * @brief Change l'arme, de façon à pouvoir mettre à jour la boule de feu.
     */
    void setWeapon(Weapon* weapon);

    /**
     * @brief Met à jour le compteur d'updates si l'ID est 1.
     */
    virtual void update() override;

    /**
     * @brief Non bloquant.
     */
    virtual bool block() const override;

    /**
     * @brief Ne fait rien.
     */
    virtual void fire(int angle_vertical) override;

    /**
     * @brief Appelle m_weapon->process_fireball.
     */
    virtual bool process_fireball(float dx, float dy) override;

    /**
     * @brief Ne fait rien.
     */
    virtual void hit(CMover*, int) override;
};