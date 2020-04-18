#pragma once

#include "CMover.h"
#include "Weapon.h"

class Character: public CMover
{
protected:
    int    m_pv;         // Point de vie de chaque personnage compris entre 0 et 100
    int    m_maxpvs;     // Points de vie maximum
    Sound* m_damage_hit; // son joué lorsqu'un personnage prend des dégats

    /*
        Son joué lorsqu'un personnage reprend de la vie, potentiellement utilisé
        uniquement pour le chasseur. Il n'est pas prévu que les gardiens se soigne.
    */
    Sound* m_heal_sound;

    Weapon m_weapon;

public:
    /**
     * @brief Constructeur
     */
    Character(Labyrinthe* l, const char* modele, uint id, int maxpvs);

    /**
     * @brief Retourne le nombre de PVs restants de l'objet.
     */
    inline const int getPV() const { return m_pv; }

    /**
     * @brief Blesse l'objet.
     * Si damage est négatif, il sera au contraire soigné.
     */
    virtual void hit(CMover* m, int damage) override;

    /**
     * @brief Fonction appelée quand l'objet meurt.
     * Peut être utilisée même si les PVs ne sont pas à 0 pour tuer instantanément l'objet.
     * @param m L'objet qui a tué cet objet (nullptr si c'est l'environnement)
     */
    virtual void die(CMover* m) = 0;

    /**
     * @brief Renvoie true si le Character est mort.
     */
    inline bool isDead() const {
        return m_pv <= 0;
    }

    virtual bool process_fireball(float dx, float dy) override;
    virtual void fire(int angle_vertical) override;

    virtual bool block() const override { return true; }

    /**
     * @brief Renvoie l'arme actuelle.
     */
    inline const Weapon& getWeapon() const { return m_weapon; }

    inline Weapon& weapon() { return m_weapon; }

    /**
     * @brief Change l'arme actuelle.
     */
    inline void setWeapon(const Weapon& weapon) { m_weapon = weapon; }
};