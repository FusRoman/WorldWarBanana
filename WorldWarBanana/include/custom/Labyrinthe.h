#pragma once

#include <fstream>
#include <iostream>

#include "Environnement.h"
#include "FireBallDX.h"
#include "Guard.h"
#include "Hunter.h"
#include "macros.h"
#include "utils.h"

#define NB_POSTERS 26

/**
 * @brief Un enum pour communiquer avec les autres classes,
 * représentant les données présentes sur chaque case.
 */
enum CellType
{
    _EMPTY = EMPTY,
    WALL,
    TREASURE,
    CMOVER // Toujours en dernière place !
};

class Labyrinthe: public Environnement
{
private:
    /**
     * @brief Tableau d'occupation des cases.
     * Vaut EMPTY si la case est praticable, 1 sinon.
     */
    char** m_data;

    /**
     * @brief Contient la distance de chaque case au trésor.
     */
    uint** m_distances;

    /**
     * @brief Distance maximale d'une case atteignable au trésor.
     */
    uint m_maxDistance;

    /**
     * @brief Largeur du labyrinthe.
     */
    uint m_width;

    /**
     * @brief Hauteur du labyrinthe.
     */
    uint m_height;

    /**
     * @brief Contient les chemins associés à chaque poster.
     */
    char* m_posters[NB_POSTERS];

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
     * @brief Alloue l'espace nécessaire pour les caisses, les gardes et les posters.
     * Instancie aussi les gardes (ce ne sera pas forcément le cas plus tard,
     * s'il y en a de plusieurs types).
     * @return L'id du premier garde
     */
    uint allocate();

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
     * @brief Initialise m_distances pour le path finding des guardiens.
     * Concrètement, chaque case praticable sera associée à sa distance au trésor
     * en nombre minimal de déplacements case par case, via un algorithme de flooding
     * (d'où le nom, fort astucieux si vous me demandez mon avis).
     * Additionnellement, l'algorithme vérifie deux critères de validité pour le labyrinthe :
     * - il est bien fermé
     *      Si on sort des dimensions du labyrinthe au cours d'exécution,
     *      alors il n'est pas fermé et on doit lancer une erreur appropriée.
     * - le joueur peut accéder au trésor
     *      Si on atteint jamais la position du joueur lors de l'algorithme,
     *      alors le trésor est complètement inacessible au joueur et le labyrinthe n'est pas
     * valide. Peu importe si d'autres objets sont hors de portée du joueur, puisqu'ils ne sont pas
     * essentiels pour gagner.
     */
    void flood();

    /**
     * @brief Modifie m_data de façon à rendre les gardiens bloquants.
     */
    void fillDataMovers();

    /**
     * @brief Fonction auxiliaire de move.
     */
    bool moveAux(CMover* mover, double dx, double dy);

public:
    /**
     * @brief Texture des boîtes
     */
    int boxTex;

    /**
     * @brief Renvoie des coordonnées utilisables par les Mover depuis les coordonnées d'une case.
     */
    static Vec2f gridToReal(int x, int y);

    /**
     * @brief Overload de gridToReal.
     */
    inline static Vec2f gridToReal(Vec2i p) { return gridToReal(p.x, p.y); }

    /**
     * @brief Renvoie la case correspondant aux coordonnées d'un Mover.
     */
    static Vec2i realToGrid(float x, float y);

    /**
     * @brief Overload de realToGrid.
     */
    inline static Vec2i realToGrid(Vec2f p) { return realToGrid(p.x, p.y); }

    Labyrinthe(char*);
    virtual ~Labyrinthe();

    int width() override { return m_width; }

    int height() override { return m_height; }

    /**
     * @brief Renvoie le numéro de texture pour l'affiche n°index.
     */
    int wall_texture(uint index);

    /**
     * @brief Renvoie l'occupation de la case (x, y).
     * Renvoie EMPTY si la case est praticable, autre chose sinon.
     * Si la case est en-dehors des dimensions du labyrinthe,
     * on renvoie 1.
     */
    char data(int x, int y) override;

    /**
     * @brief Une méthode similaire à data, mais plus explicite.
     * Si le résultat renvoyé est CMOVER, on peut appeler getMover avec les mêmes
     * argument sans crainte.
     */
    CellType getCellType(int x, int y);

    /**
     * @brief Similaire à getCellType.
     * Renvoie juste _EMPTY si c'est mover qui se trouve sur la case désignée.
     */
    CellType getCellType(CMover* mover, int x, int y);

    /**
     * @brief Renvoie le chasseur.
     */
    Hunter* getHunter();

    /**
     * @brief Renvoie le CMover en (x, y), nullptr si aucun.
     */
    CMover* getMover(int x, int y);

    /**
     * @brief Renvoie le nombre de gardes dans ce labyrinthe.
     */
    uint getNbGuards() const;

    /**
     * @brief Renvoie le agrde à l'index donné, en supposant que le premier garde est à 0.
     */
    Guard* getGuard(uint index);

    /**
     * @brief Renvoie true si mover peut aller sur la case indiquée.
     */
    bool canGoTo(CMover* mover, int x, int y);

    /**
     * @brief Retourne la distance de la case (x, y) au trésor.
     */
    uint distanceFromTreasure(int x, int y) const;

    /**
     * @brief Retourne la distance au trésor de la case atteignable la plus éloignée de celui-ci.
     */
    inline uint getMaxDistance() const {
        return m_maxDistance;
    }

    /**
     * @brief Libère la case sur laquelle se trouve le mover actuel.
     */
    void free(CMover* mover);

    /**
     * @brief Renvoie true si mover peut se déplacer de (dx, dy).
     * Effectue le déplacement si possible.
     * Si mover se trouve bloqué par un obstacle, au lieu de ne rien
     * faire, la fonction va tenter de faire glisser l'objet.
     */
    bool move(CMover* mover, double dx, double dy);

    /**
     * @brief Renvoie la boule de feu correspondant à l'index donné.
     * Les boules de feu renvoyées par cette fonction sont exclusivement
     * à l'usage du joueur.
     * L'index donné doit être inférieur à Weapon::maxNbBalls.
     */
    FireBallDX* getFireBall(uint i) const;
};