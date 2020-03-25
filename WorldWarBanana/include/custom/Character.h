#ifndef CHARACTER_H
#define CHARACTER_H
#include "CMover.h"

class Character : public CMover 
{
protected:
    int             m_pv;           //Point de vie de chaque personnage compris entre 0 et 100
    Sound*          m_damage_hit;   //son joué lorsqu'un personnage prend des dégats
    /*
        Son joué lorsqu'un personnage reprend de la vie, potentiellement utilisé 
        uniquement pour le chasseur. Il n'est pas prévue que les gardiens se soigne.
    */
    Sound*          m_heal_sound;


public:

    /**
        * @brief Constructeur
        */
    Character(int x, int y, Labyrinthe* l, const char* modele, uint id);

    /**
     * @brief Retourne le nombre de PVs restants de l'objet.
     */
    inline const int getPV() const 
    {
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
     * @param m L'objet qui a tué cet objet (nullptr si c'est l'environnement)
     */
    virtual void die(CMover* m) =0;   

};

#endif