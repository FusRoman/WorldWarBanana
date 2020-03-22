#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include <iostream>
#include <fstream>

#include "Environnement.h"

#define NB_POSTERS 26
class Labyrinthe : public Environnement
{
private:
    /**
     * @brief Tableau d'occupation des cases.
     * Vaut EMPTY si la case est praticable, 1 sinon.
     */
    char** m_data;

    /**
     * @brief Largeur du labyrinthe.
     */
    uint   m_width;

    /**
     * @brief Hauteur du labyrinthe.
     */
    uint   m_height;

    /**
     * @brief Contient les chemins associés à chaque poster.
     */
    char*  m_posters[NB_POSTERS];

    /**
     * @brief Stocke un chemin vers une images dans m_posters à l'index donné.
     */
    void storePoster(int index, const char* path);

    /**
     * @brief Interprète le début du fichier
     * # Avec des commentaires commençant par #
     * # Et ensuite des lignes de la forme
     * # [a-z] <fichier>
     * a affiche1.jpg
     * b affiche2.jpg
     * z affiche3.jpg
     * # A noter qu'on peut écraser une association précédente
     * a affiche4.jpg
     * # a fait maintenant référence à affiche4.jpg et plus affiche1.jpg
     */
	void parsePosters(std::ifstream& file);

    /**
     * @brief Interprète le labyrinthe lui-même.
     * # Voici un exemple minimal :
     * +--+
     * |TC|
     * +--+
     *
     * @todo Pour l'instant, les commentaires à la fin du fichier ne sont pas tolérés !
     */
	void parseMaze(std::ifstream& file);
    
    /**
     * @brief Remplis m_data pour les objets statiques (murs, caisses et trésor)
     * Le but de cette fonction est de préparer le terrain pour flood.
     */
    void fillData();

    /**
     * @brief Initialise les données internes pour le path finding des guardiens.
     * Concrètement, chaque case praticable sera associée à sa distance au trésor
     * en nombre minimal de déplacements case par case, via un algorithme de flooding
     * (d'où le nom, fort astucieux si vous me demandez mon avis).
     * Additionnellement, l'algorithme vérifie deux critères de validité pour le labyrinthe :
     * - il est bien fermé
     *      Si on sort des dimensions du labyrinthe au cours d'exécution, 
     *      alors il n'est pas fermé et on doit lancer une erreur appropriée.
     * - le joueur peut accéder au trésor
     *      Si on atteint jamais la position du joueur lors de l'algorithme, 
     *      alors le trésor est complètement inacessible au joueur et le labyrinthe n'est pas valide. 
     * Peu importe si d'autres objets sont hors de portée du joueur, 
     * puisqu'ils ne sont pas essentiels pour gagner .
     *
     * @todo On utile m_data ou un autre array avec exactement les mêmes dimensions ?
     * Si on prend la première solution, il faut changer le type de m_data (on met plutôt uint).
     * La valeur std::numeric_limits<uint>::max() représente alors une case occupée.
     * Toute autre valeur est considérée comme praticable, et représente la distance au trésor
     * en nombre de déplacements case par case minimum.
     * Inconvénient : on ne peut pas modifier m_data quand on fait bouger les Mover au risque
     * de perdre des données, donc on va devoir itérer sur les Mover dans data().
     */
    void flood();

public:
    Labyrinthe(char*);
    virtual ~Labyrinthe();

    int width() override { return m_width; }

    int height() override { return m_height; }

    /**
     * @brief Renvoie le chemin vers l'affiche n°index.
     * L'array renvoyé n'est pas const parce que Environnement::wall_texture prend un char* 
     * et pas un const char*...
     */
    char* getPosterPath(uint index) const;

    /**
     * @brief Renvoie l'occupation de la case (x, y).
     * Renvoie EMPTY si la case est praticable, 1 sinon.
     * Si la case est en-dehors des dimensions du labyrinthe, 
     * on renvoie 1.
     */
    char data(int x, int y) override;
};

#endif
