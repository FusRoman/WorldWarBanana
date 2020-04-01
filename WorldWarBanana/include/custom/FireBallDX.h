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
    Weapon* m_weapon;

public:
    FireBallDX(Labyrinthe* laby, uint id);

    /**
     * @brief Change l'arme, de façon à pouvoir mettre à jour la boule de feu.
     */
    void setWeapon(Weapon* weapon);

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