#pragma once

#include <string>

#include "CMover.h"

class Weapon
{
private:
    // Propriétaire de l'arme
    CMover*     m_owner;
    // Le nom de l'arme (pour d'éventuels messages)
    std::string m_name;
    // Vaut true si on a déjà tiré au moins une fois avec cette arme
    bool        m_firedOnce;

    // Durée minimum entre chaque tir (en nombre d'updates du jeu)
    int         m_cooldown;
    // Date du dernier tir
    uint         m_lastFired;
    
    // Portée de l'arme
    float       m_reach;
    // Dégâts de l'arme (peut être négatif)
    int         m_damage;

    // Le son joué lorsqu'on tire avec l'arme
    Sound*      m_fire;
    // Le son joué lorsque la boule de feu générée par cette arme explose
    Sound*      m_hit;
    // Le son joué lorsqu'on veut tirer, mais qu'on ne peut pas à cause du cooldown
    Sound*      m_trigger;

    void assert(bool condition, const std::string& msg);
    void assertNeverFired(const std::string& msg);

public:
    /**
     * @brief Sons par défaut.
     */
    static Sound* onFire;
    static Sound* onHit;
    static Sound* onTrigger;

    /**
     * @brief Portée infinie.
     */
    static const float infiniteReach;

    /**
     * @brief Constructeur "par défaut".
     */
    Weapon(CMover* owner);

    /**
     * @brief Constructeur détaillé.
     */
    Weapon(CMover* owner, const std::string& name, 
        int damage, uint cooldown, float reach, 
        Sound* onFire = onFire, Sound* onHit = onHit, Sound* onTrigger = onTrigger);

    /**
     * @brief Constructeur par copie.
     */
    Weapon(const Weapon& w);

    inline CMover* getOwner() const { return m_owner; }
    void setOwner(CMover* owner);

    inline const std::string& getName() const { return m_name; }
    void setName(const std::string& name);

    inline int getCooldown() const { return m_cooldown; }
    void setCooldown(uint cooldown);

    inline float getReach() const { return m_reach; }
    void setReach(float reach);

    inline int getDamage() const { return m_damage; }
    void setDamage(int damage);

    inline Sound* getOnFire() const { return m_fire; }
    void setOnFire(Sound* onFire);

    inline Sound* getOnHit() const { return m_hit; }
    void setOnHit(Sound* onHit);

    inline Sound* getOnTrigger() const { return m_trigger; }
    void setOnTrigger(Sound* onTrigger);

    /**
     * @brief Lance une boule de feu.
     */
    void fire(FireBall* fb, int angle);

    /**
     * @brief Met à jour une boule de feu.
     */
    bool process_fireball(FireBall* fb, double dx, double dy);
};
