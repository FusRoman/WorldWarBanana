#include "Labyrinthe.h"

#include <string.h>

#include <limits>
#include <list>
#include <string>
#include <vector>

#include "Weapon.h"

Environnement* Environnement::init(char* filename) { return new Labyrinthe(filename); }

Vec2f Labyrinthe::gridToReal(int x, int y) { return Vec2f(x * scale, y * scale); }

Vec2i Labyrinthe::realToGrid(float x, float y) { return Vec2i(x / scale, y / scale); }

Sound* Labyrinthe::weaponBonusSound = new Sound("sons/weapon_bonus.wav");
Music* Labyrinthe::mainMusic = new Music("sons/funny.wav");
Music* Labyrinthe::victoryMusic = new Music("sons/happy.wav");

// On utilise ce namespace pour ne pas que ces fonctions qu'on désire n'utiliser que dans ce fichier
// ne rentrent pas en conflit avec d'autres fonctions locales à un fichier
// (on pourrait aussi utiliser static il me semble)
namespace Labyrinthe_private
{

    typedef struct
    {
        int line;
        int column;
    } Cursor;

    Cursor cursor = {line : 0, column : 0};

    void reset()
    {
        cursor.line   = 0;
        cursor.column = 0;
    }

    void newLine()
    {
        ++cursor.line;
        cursor.column = 0;
    }

    void error(const std::string& msg)
    {
        ERROR("Error line " << cursor.line + 1 << ", column " << cursor.column + 1 << ": " << msg);
    }

    void stray(char c)
    {
        error(std::string("Stray character ") + c + " (ASCII: " + std::to_string((int) c) + ")");
    }

    // Renvoie l'index du premier caractère non blanc (ou size si aucun)
    int ignoreSpace(const std::string& line, unsigned int start)
    {
        while (start < line.size() && isSpace(line[start]))
        {
            ++start;
        }
        return start;
    }

    // Renvoie l'index du dernier caractère non blanc ( ou size si aucun)
    int ignoreNonSpace(const std::string& line, unsigned int start)
    {
        while (start < line.size() && !isSpace(line[start]))
        {
            ++start;
        }
        return start;
    }
} // namespace Labyrinthe_private
using namespace Labyrinthe_private;

/**************************************************************************************************
 *
 * Gestion des posters
 *
 *************************************************************************************************/

#define MAX_SIZE_POSTER_PATH 256

void Labyrinthe::storePoster(int index, const char* path)
{
    size_t dirlen = strlen(texture_dir);
    size_t pathlen = strlen(path);
    if (dirlen + pathlen + 10 > MAX_SIZE_POSTER_PATH)
    {
        ERROR("file name '" << path << "' is too long, make it shorter.");
    }
    sprintf(m_posters[index], "%s/%s", texture_dir, path);
}

void Labyrinthe::parsePosters(std::ifstream& file)
{
    // On initialise tous les posters avec une image par défaut
    for (int i = 0; i < NB_POSTERS; ++i)
    {
        m_posters[i] = new char[MAX_SIZE_POSTER_PATH];
        storePoster(i, "default_poster.jpg");
    }

    std::string currentLine;
    auto        currentPos = file.tellg();
    Cursor      saved      = cursor;
    while (std::getline(file, currentLine))
    {
        // On ignore les espaces préliminaires
        uint i = ignoreSpace(currentLine, 0);

        // Si la ligne est vide ou c'est un commentaire, on ne fait rien
        if (i < currentLine.size() && currentLine[i] != '#')
        {
            if (currentLine[i] == '+')
            {
                // On est tombé sur le labyrinthe lui-même
                // On repositionne le curseur pour que la suite se passe bien avant de retourner
                file.seekg(currentPos);
                cursor = saved;
                return;
            }
            else if (isLowerAlpha(currentLine[i]))
            {
                // On est sur une ligne qui associe une lettre minuscule à une affiche
                uint startPict = ignoreSpace(currentLine, i + 1);
                if (startPict == i + 1)
                {
                    cursor.column = i;
                    error("Invalid line. Did you forget a '#'?");
                }

                uint        endPict = ignoreNonSpace(currentLine, startPict);
                const char* path    = currentLine.substr(startPict, endPict - startPict).c_str();
                storePoster(currentLine[i] - 'a', path);

                // On vérifie qu'il n'y a rien d'autre sur la ligne
                int next = ignoreSpace(currentLine, endPict);
                if (next != -1 && currentLine[next] != '#' && currentLine[next] != 0)
                {
                    cursor.column = next;
                    if (isLowerAlpha(currentLine[next]))
                    {
                        error("Only one poster per line");
                    }
                    else
                    {
                        stray(currentLine[next]);
                    }
                }
            }
            else
            {
                cursor.column = i;
                // error("Stray character " + currentLine.substr(i, i+1));
                stray(currentLine[i]);
            }
        }

        currentPos = file.tellg();
        saved      = cursor;
        newLine();
    }

    error("No maze provided");
}

/**************************************************************************************************
 *
 * parseMaze
 *
 *************************************************************************************************/

namespace Labyrinthe_private
{
    enum Type
    {
        CROSS,
        ALIGNED_WALL,
        PERPENDICULAR_WALL,
        ALIGNED_POSTER,
        PERPENDICULAR_POSTER,
        OBJECT
    };

    // Structure de données pour le parser
    class ParserState
    {
    public:
        // Coordonnées actuelles
        int x, y;

        // Analyse verticale ou horizontale ?
        bool horizontal;

        // Le mur actuel, en cours de formation
        Wall currentWall;

        // La liste des murs, à compléter au fur et à mesure
        std::list<Wall>& walls;

        // Index auxquels placer les différents objets
        uint indexBoxes;
        uint indexGuards;
        uint indexPosters;

        // Le labyrinthe à parser et les coordonnées du caractère actuel par rapport à ce vecteur.
        std::vector<std::string> maze;
        uint                     mazeX, mazeY;

        // Fonction de transition
        void (*transition)(Labyrinthe* laby, ParserState& state, char c);
    };

    char getChar(const ParserState& state, uint x, uint y)
    {
        if (y >= state.maze.size())
        {
            return ' ';
        }
        else
        {
            return (x <= state.maze[y].size()) ? state.maze[y][x] : ' ';
        }
    }

    /*bool isHWall(char c)
    {
        return c == '-' || c == '+' || isLowerCase(c);
    }

    bool isVWall(char c)
    {
        return c == '|' || c == '+' || isLowerCase(c);
    }*/

    // A appeler quand on lit un poster et qu'on ne sait pas dans quelle direction il est orienté
    // Renvoie true si le poster est aligné
    // saved est le curseur positionné sur le tout début du labyrinthe
    Type getPosterOrientation(const ParserState& state)
    {
        const int x = state.mazeX;
        const int y = state.mazeY;
        // Sensible à l'overflow (mais a priori ce n'est pas notre principal souci
        // si les labyrinthes sont assez grands pour causer ce genre de problèmes...)
        char left = getChar(state, x - 1, y), right = getChar(state, x + 1, y),
             up = getChar(state, x, y - 1), bottom = getChar(state, x, y + 1);
        bool h = left == '-' || left == '+' || right == '-' || right == '+',
             v = up == '|' || up == '+' || bottom == '|' || bottom == '+';
        /*bool h = isHWall(left) || isHWall(right),
             v = isVWall(down) || isVWall(up);*/
        if (h && v)
        {
            error("Poster used in place of '+'.");
        }
        if (!h && !v)
        {
            error("Poster can not be placed without any supporting wall.");
        }
        if (state.horizontal)
        {
            return h ? ALIGNED_POSTER : PERPENDICULAR_POSTER;
        }
        else
        {
            return v ? ALIGNED_POSTER : PERPENDICULAR_POSTER;
        }
    }

    // Utile pour régler certains cas problématiques
    // Lors de l'analyse horizontale, rencontrer une croix sans murs alignés après ou avant
    // n'est pas forcément une erreur, s'il y a des murs verticaux au-dessus ou en-dessous
    // Cette fonction permet de considérer de tels croix comme des murs verticaux
    // Le parser peut s'occuper de tous les autres cas
    Type getCrossOrientation(const ParserState& state)
    {
        const int x    = state.mazeX;
        const int y    = state.mazeY;
        char      left = getChar(state, x - 1, y), right = getChar(state, x + 1, y),
             up = getChar(state, x, y - 1), bottom = getChar(state, x, y + 1);
        bool h = left == '-' || left == '+' || right == '-' || right == '+',
             v = up == '|' || up == '+' || bottom == '|' || bottom == '+';
        /*bool h = isHWall(left) || isHWall(right),
             v = isVWall(down) || isVWall(up);*/
        if (state.horizontal)
        {
            return (!h && v) ? PERPENDICULAR_WALL : CROSS;
        }
        else
        {
            return (!v && h) ? PERPENDICULAR_WALL : CROSS;
        }
    }

    Type _type(char c, const ParserState& state)
    {
        if (isLowerAlpha(c))
        {
            return getPosterOrientation(state);
        }

        switch (c)
        {
        case '+':
            return getCrossOrientation(state);

        case ' ':
        case 'G':
        case 'T':
        case 'X':
        case 'C':
        case 'H':
        case 'A':
        case 'D':
        case 'P':
            return OBJECT;

        case '-':
            return state.horizontal ? ALIGNED_WALL : PERPENDICULAR_WALL;

        case '|':
            return state.horizontal ? PERPENDICULAR_WALL : ALIGNED_WALL;

        default:
            stray(c);
            return OBJECT; // Juste pour le compilateur
        }
    }

    void placeBox(Labyrinthe* laby, ParserState& state, int tex)
    {
        Box* box   = laby->_boxes + state.indexBoxes;
        box->_x    = state.x;
        box->_y    = state.y;
        box->_ntex = tex;
        ++state.indexBoxes;
    }

    void placeObject(Labyrinthe* laby, ParserState& state, char c)
    {
        if (state.horizontal)
        {
            // On ne veut placer les objets qu'une seule fois
            return;
        }

        switch (c)
        {
        case ' ':
            break;

        case 'X':
            placeBox(laby, state, laby->boxTex);
            break;

        case 'H':
            placeBox(laby, state, laby->healthBoxTex);
            break;

        case 'A':
            placeBox(laby, state, laby->attackBoxTex);
            break;

        case 'D':
            placeBox(laby, state, laby->cooldownBoxTex);
            break;

        case 'P':
            placeBox(laby, state, laby->projectilesBoxTex);
            break;

        case 'G':
        {
            Mover* guard = laby->_guards[state.indexGuards];
            Vec2f  p     = Labyrinthe::gridToReal(state.x, state.y);
            guard->_x    = p.x;
            guard->_y    = p.y;
            ++state.indexGuards;
            break;
        }

        case 'T':
            // Grâce à la première passe, on sait qu'il n'y a qu'un seul trésor
            laby->_treasor._x = state.x;
            laby->_treasor._y = state.y;
            break;

        case 'C':
        {
            Mover* hunter = laby->_guards[0];
            Vec2f  p      = Labyrinthe::gridToReal(state.x, state.y);
            hunter->_x    = p.x;
            hunter->_y    = p.y;
            break;
        }

        default:
            error("Labyrinthe::parseMaze: unexpected character");
        }
    }

    void errorOnPerpendicularWall(ParserState& state)
    {
        if (state.horizontal)
        {
            error("Met a vertical wall while reading a horizontal wall. You may want to put '+' at "
                  "the intersection.");
        }
        else
        {
            error("Met a horizontal wall while reading a vertical wall. You may want to put '+' at "
                  "the intersection.");
        }
    }

    void errorOnEnd(ParserState& state)
    {
        if (state.horizontal)
        {
            error("Horizontal wall without right end.");
        }
        else
        {
            error("Vertical wall without bottom end.");
        }
    }

    // Etat initial : on n'est pas dans un mur
    void initial(Labyrinthe*, ParserState&, char);

    // Etat où on est en train de lire un mur dont il nous manque au moins encore un caractère
    void wall(Labyrinthe*, ParserState&, char);

    // Etat où on a précédemment lu un poster
    // Devant, il ne peut y avoir qu'un mur (sans poster)
    void poster(Labyrinthe*, ParserState&, char);

    // Etat où le mur qu'on lit est potentiellement fini
    // Si on trouve encore un mur, on considère que c'est le même
    // Mais si on ne trouve pas de mur, il n'y a pas non plus d'erreur
    void endOfWall(Labyrinthe*, ParserState&, char);

    void initial(Labyrinthe* laby, ParserState& state, char c)
    {
        switch (_type(c, state))
        {
        case ALIGNED_WALL:
        case ALIGNED_POSTER:
            if (state.horizontal)
            {
                error("Horizontal wall without left end");
            }
            else
            {
                error("Vertical wall without top end");
            }

        case CROSS:
            // On positionne le début du mur
            state.currentWall._x1 = state.x;
            state.currentWall._y1 = state.y;
            state.transition      = wall;
            break;

        case PERPENDICULAR_WALL:
        case PERPENDICULAR_POSTER:
            // Rien à faire
            break;

        default:
            placeObject(laby, state, c);
            break;
        }
    }

    void wall(Labyrinthe* laby, ParserState& state, char c)
    {
        switch (_type(c, state))
        {
        case CROSS:
            // Le mur pourrait être fini maintenant
            // Mais pas forcément, on met donc le parser dans un état adapté
            state.transition = endOfWall;
            break;

        case PERPENDICULAR_WALL:
        case PERPENDICULAR_POSTER:
            errorOnPerpendicularWall(state);

        case ALIGNED_POSTER:
        {
            Wall* pict  = laby->_picts + state.indexPosters;
            pict->_ntex = laby->wall_texture(c - 'a');
            // pict->_ntex = 0;
            pict->_x1 = state.x;
            pict->_y1 = state.y;
            if (state.horizontal)
            {
                pict->_x2 = state.x + 2;
                pict->_y2 = state.y;
            }
            else
            {
                pict->_x2 = state.x;
                pict->_y2 = state.y + 2;
            }
            ++state.indexPosters;
            state.transition = poster;
            break;
        }

        case ALIGNED_WALL:
            // Rien à faire ici
            break;

        default:
            errorOnEnd(state);
        }
    }

    void poster(Labyrinthe* laby, ParserState& state, char c)
    {
        switch (_type(c, state))
        {
        case CROSS:
            state.transition = endOfWall;
            //error("Not enough room left for the poster");
            break;

        case ALIGNED_WALL:
            state.transition = wall;
            break;

        case ALIGNED_POSTER:
            if (state.horizontal)
            {
                error("Found two horizontally consecutive posters.");
            }
            else
            {
                error("Found two vertically consecutive posters.");
            }

        case PERPENDICULAR_WALL:
        case PERPENDICULAR_POSTER:
            errorOnPerpendicularWall(state);

        default:
            errorOnEnd(state);
        }
    }

    void endOfWall(Labyrinthe* laby, ParserState& state, char c)
    {
        switch (_type(c, state))
        {
        // 3 case suivants : le mur ne s'est pas terminé finalement
        // Pareil que pour wall
        case ALIGNED_POSTER:
        case CROSS:
        case ALIGNED_WALL:
            state.transition = wall;
            wall(laby, state, c);
            break;

        // Pour les autres cas, le mur ne continue pas
        // On l'ajoute, puis on revient sur le cas initial
        default:
            // Le mur a terminé sur le caractère précédent, pas sur (state.x, state.y)
            if (state.horizontal)
            {
                state.currentWall._x2 = state.x - 1;
                state.currentWall._y2 = state.y;
            }
            else
            {
                state.currentWall._x2 = state.x;
                state.currentWall._y2 = state.y + 1;
                // +1 et pas -1 à cause de l'inversion sur l'axe y
            }
            state.currentWall._ntex = 0;
            state.walls.push_back(state.currentWall);

            state.transition = initial;
            initial(laby, state, c);
        }
    }
} // namespace Labyrinthe_private

uint Labyrinthe::allocate()
{
    _picts = new Wall[_npicts];
    _boxes = new Box[_nboxes];

    // Gardes
    uint nbguards = Weapon::maxNbBalls + _nguards;
    _guards       = new Mover*[nbguards];
    _guards[0]    = new Hunter(this, 0);

    // Les boules de feu
    uint i;
    for (i = 1; i <= Weapon::maxNbBalls; ++i)
    {
        _guards[i] = new FireBallDX(this, i);
    }

    // Les gardes eux-mêmes
    for (uint g = i; g < nbguards; ++g)
    {
        int rd     = randomInt(0, Guard::modeles.size() - 1);
        _guards[g] = new Guard(this, rd, g, 100);
    }

    _nguards = nbguards;
    return i;
}

void Labyrinthe::parseMaze(std::ifstream& file)
{
    _npicts  = 0;
    _nwall   = 0;
    _nguards = 0;
    _nboxes  = 0;
    m_width  = 0;
    m_height = 0;

    /*
     * 1er passe : récupération des informations du labyrinthe
     *  - nombre de gardes
     *  - nombre de caisses
     *  - nombre d'affiches
     *  - dimensions du labyrinthe
     * On stocke également les caractères dans un vecteur pour simplifier les passes suivantes.
     *
     * On vérifie également qu'il y a un et un seul chasseur (pareil pour le trésor).
     * Cependant on ne les place pas encore. En effet, les premières colonnes du labyrinthe
     * peuvent être complètement vides, on veut donc "décaler" tous les caractères à droite
     * pour éviter d'allouer de l'espace non utilisé. De ce fait, la position d'un caractère
     * dans le fichier ne donne pas sa position dans la labyrinthe.
     *
     * Les murs ne sont pas comptés dans cette phase puisque cela requiert une analyse plus poussée.
     */
    Cursor saved       = cursor;
    bool   treasureSet = false, hunterSet = false;
    uint   xmin = std::numeric_limits<uint>::max(), xmax = std::numeric_limits<uint>::min();
    uint i = 0; // Index de la ligne (la première ligne n'est jamais vide grâce à l'implémentation
                // de parsePosters)
    std::vector<std::string> maze;
    std::string              line;
    while (std::getline(file, line))
    {
        maze.push_back(line);
        uint left = xmin, right = xmax;
        bool leftSet = false;
        for (uint x = 0; x < line.size(); ++x)
        {
            cursor.column = x;

            if (line[x] == '\t')
            {
                error("Tabulations are not allowed in the description of the maze for readability "
                      "purpose.");
            }

            if (line[x] == ' ')
            {
                // Absolument rien à faire ici
                continue;
            }

            // Si on arrive ici, la ligne n'est pas complètement vide
            // => Mise à jour des variables pour le calcul des dimensions
            if (!leftSet)
            {
                left    = x;
                leftSet = true;
            }
            right    = x;
            m_height = i;

            // Analyse du caractère
            if (isLowerAlpha(line[x]))
            {
                ++_npicts;
                continue;
            }

            switch (line[x])
            {
            case '+':
            case '-':
            case '|':
                // Rien à faire (à part right = x, déjà fait)
                break;

            case 'T':
                if (treasureSet)
                {
                    error("There must be a single treasure");
                }
                treasureSet = true;
                break;

            case 'C':
                if (hunterSet)
                {
                    error("There must be a single starting point.");
                }
                hunterSet = true;
                ++_nguards;
                break;

            case 'X':
            case 'H':
            case 'A':
            case 'D':
            case 'P':
                ++_nboxes;
                break;

            case 'G':
                ++_nguards;
                break;

            default:
                stray(line[x]);
            }
        }

        xmin = min(left, xmin);
        xmax = max(right, xmax);
        ++i;
        newLine();
    }

    m_width = xmax - xmin + 1;
    ++m_height; // m_height est une dimension, or on lui a assigné des valeurs d'index : +1 pour
                // compenser

    if (!treasureSet)
    {
        error("No treasure found.");
    }
    if (!hunterSet)
    {
        error("No starting point found");
    }
    // Fin de la première passe

    // Préparation pour les deux passes suivantes (allocations essentiellement)
    uint g = allocate();

    std::list<Wall> walls;
    ParserState     state = {
        0,      // x
        0,      // y
        true,   // horizontal
        Wall(), // currentWall
        walls,  // walls
        0,      // indexBoxes
        g,      // indexGuards
        0,      // indexPosters
        maze,   // maze
        0,      // mazeX
        0,      // mazeY
        initial // transition
    };

    // Deuxième passe : lecture horizontale
    for (uint y = 0; y < maze.size(); ++y)
    {
        cursor.line = saved.line + y;
        // On considère que (0, 0) est le point le plus en bas à gauche
        // On inverse donc sur l'axe y
        state.y     = m_height - y - 1;
        state.mazeY = y;
        uint end    = min<uint>(xmax + 1, maze[y].size());
        for (uint x = xmin; x < end; ++x)
        {
            cursor.column = x;
            state.x       = x - xmin;
            state.mazeX   = x;
            state.transition(this, state, maze[y][x]);
        }

        // Dernier appel à transition pour la ligne
        // Pour vérifier que le mur s'est bien fini
        ++state.mazeX;
        ++state.x;
        state.transition(this, state, ' ');

        state.transition = initial;
    }

    // Troisième passe : lecture verticale
    state.horizontal = false;
    for (uint x = xmin; x <= xmax; ++x)
    {
        cursor.column = x;
        state.x       = x - xmin;
        state.mazeX   = x;
        for (uint y = 0; y < m_height; ++y)
        {
            cursor.line = saved.line + y;
            state.y     = m_height - y - 1;
            state.mazeY = y;
            char c      = (x < maze[y].size()) ? maze[y][x] : ' ';
            state.transition(this, state, c);
        }

        ++state.mazeY;
        --state.y;
        state.transition(this, state, ' ');

        state.transition = initial;
    }

    _nwall = state.walls.size();
    _walls = toArray(state.walls);
}

/**************************************************************************************************
 *
 * Algorithme d'inondation
 *
 *************************************************************************************************/

namespace Labyrinthe_private
{
    typedef struct
    {
        int  x;
        int  y;
        uint d;
    } BFSData;

    void floodNeighbour(Labyrinthe* laby, int x, int y, uint d, std::list<BFSData>& stack)
    {
        if (x < 0 || x >= laby->width() || y < 0 || y >= laby->height())
        {
            ERROR("The maze is not closed. Check its borders.");
        }
        if (laby->data(x, y) == _EMPTY && laby->distanceFromTreasure(x, y) > d + 1)
        {
            stack.push_back({x, y, d + 1});
        }
    }
} // namespace Labyrinthe_private

// Remplit m_data sans considérer les Mover (pour l'algorithme de flood après)
void Labyrinthe::fillData()
{
    // Gestion des murs
    // Pas besoin de s'occuper des posters a priori puisqu'il y a toujours un mur derrière
    for (int i = 0; i < _nwall; ++i)
    {
        Wall* wall = _walls + i;
        if (wall->_y1 == wall->_y2)
        {
            // Mur horizontal
            int end = max(wall->_x1, wall->_x2);
            for (int x = min(wall->_x1, wall->_x2); x <= end; ++x)
            {
                m_data[wall->_y1][x] = WALL;
            }
        }
        else
        {
            // On suppose que le mur est vertical
            int end = max(wall->_y1, wall->_y2);
            for (int y = min(wall->_y1, wall->_y2); y <= end; ++y)
            {
                m_data[y][wall->_x1] = WALL;
            }
        }
    }

    // Gestion des caisses
    // (ne marche que si chaque type de caisse ne partage qu'une seule texture, mais a priori c'est le cas)
    for (int i = 0; i < _nboxes; ++i)
    {
        Box* box                 = _boxes + i;
        int type = WALL;
        if (box->_ntex == healthBoxTex)
        {
            type = HEALTH;
        } 
        else if (box->_ntex == attackBoxTex)
        {
            type = ATTACK;
        } 
        else if (box->_ntex == cooldownBoxTex)
        {
            type = COOLDOWN;
        }
        else if (box->_ntex == projectilesBoxTex)
        {
            type = PROJECTILES;
        }
        m_data[box->_y][box->_x] = type;
    }

    // Trésor
    m_data[_treasor._y][_treasor._x] = TREASURE;

    // On ne s'occupe pas des Mover ici
    // Le but était juste de préparer le terrain pour flood
}

void Labyrinthe::flood()
{
    // BFS en partant du trésor
    std::list<BFSData> stack;
    stack.push_back({_treasor._x, _treasor._y, 0});

    while (!stack.empty())
    {
        BFSData data = stack.front();
        stack.pop_front();
        if (data.d >= m_distances[data.y][data.x])
        {
            continue;
        }
        m_distances[data.y][data.x] = data.d;

        floodNeighbour(this, data.x, data.y - 1, data.d, stack);
        floodNeighbour(this, data.x, data.y + 1, data.d, stack);
        floodNeighbour(this, data.x - 1, data.y, data.d, stack);
        floodNeighbour(this, data.x - 1, data.y - 1, data.d, stack);
        floodNeighbour(this, data.x - 1, data.y + 1, data.d, stack);
        floodNeighbour(this, data.x + 1, data.y, data.d, stack);
        floodNeighbour(this, data.x + 1, data.y - 1, data.d, stack);
        floodNeighbour(this, data.x + 1, data.y + 1, data.d, stack);
    }

    // Calcul de la plus grande distance
    uint umax   = std::numeric_limits<uint>::max();
    m_maxDistance = 0;
    for (uint x = 0; x < m_width; ++x) 
    {
        for (uint y = 0; y < m_height; ++y) 
        {
            uint d = m_distances[y][x];
            if (d != umax && d > m_maxDistance) 
            {
                m_maxDistance = d;
            }
        }
    }
}

void Labyrinthe::firstFlood()
{
    // Allocation et initialisation de m_distances
    uint umax   = std::numeric_limits<uint>::max();
    m_distances = create2DArray<uint>(m_width, m_height, umax);
    flood();

    // Le joueur peut-il accéder au trésor ?
    CMover* hunter = static_cast<CMover*>(_guards[0]);
    //Vec2i  p      = realToGrid(hunter->_x, hunter->_y);
    Vec2i p = hunter->getGridPosition();
    if (m_distances[p.y][p.x] == umax)
    {
        ERROR("The player has no way to get to the treasure.");
    }

    // Mettre la case du trésor à umax ?
}

void Labyrinthe::fillDataMovers()
{
    for (int i = 0; i < _nguards; ++i)
    {
        CMover* mover = (CMover*) _guards[i];
        if (mover->block())
        {
            //Vec2i p          = realToGrid(mover->_x, mover->_y);
            Vec2i p = mover->getGridPosition();
            m_data[p.y][p.x] = mover->id() + CMOVER;
        }
    }
}

void Labyrinthe::reevaluate()
{
    // Réévaluation de m_data
    for (uint y = 0; y < m_height; ++y)
    {
        char* row = m_data[y];
        for (uint x = 0; x < m_width; ++x)
        {
            row[x] = _EMPTY;
        }
    }
    fillData();

    // Réévaluation de m_distances
    uint umax   = std::numeric_limits<uint>::max();
    for (uint y = 0; y < m_height; ++y)
    {
        uint* row = m_distances[y];
        for (uint x = 0; x < m_width; ++x)
        {
            row[x] = umax;
        }
    }
    flood();

    // Enfin on rajoute les movers et on rafraîchit l'affichage
    fillDataMovers();
    reconfigure();
}

/**************************************************************************************************
 *
 * Déplacements
 *
 *************************************************************************************************/

bool Labyrinthe::canGoTo(CMover* mover, int x, int y)
{
    return m_data[y][x] == _EMPTY || (mover && (uint) m_data[y][x] == mover->id() + CMOVER);
}

bool Labyrinthe::moveAux(CMover* mover, double dx, double dy)
{
    //Vec2i p = realToGrid(mover->_x + dx, mover->_y + dy);
    Vec2i p = mover->getGridPosition(dx, dy);
    if (mover->id() == 0)
    {
        checkBoxes(p.x, p.y);
    }

    if (canGoTo(mover, p.x, p.y))
    {
        // On libère l'ancienne case
        //Vec2i op = realToGrid(mover->_x, mover->_y);
        Vec2i op = mover->getGridPosition();
        mover->_x += dx;
        mover->_y += dy;

        if (mover->block())
        {
            m_data[op.y][op.x] = _EMPTY;
            m_data[p.y][p.x]   = mover->id() + CMOVER;
        }

        return true;
    }
    return false;
}

void Labyrinthe::free(CMover* mover)
{
    //Vec2i p          = realToGrid(mover->_x, mover->_y);
    Vec2i p = mover->getGridPosition();
    m_data[p.y][p.x] = _EMPTY;
}

bool Labyrinthe::move(CMover* mover, double dx, double dy)
{
    return moveAux(mover, dx, dy) || moveAux(mover, dx, 0.0) || moveAux(mover, 0.0, dy);
}

/**************************************************************************************************
 *
 * Autres membres publics
 *
 *************************************************************************************************/

Labyrinthe::Labyrinthe(char* filename)
{
    // Chargement de différentes textures
    char tmp[128];
    sprintf(tmp, "%s/%s", texture_dir, "boite.jpg");
    boxTex = Environnement::wall_texture(tmp);
    sprintf(tmp, "%s/%s", texture_dir, "health-box.jpg");
    healthBoxTex = Environnement::wall_texture(tmp);
    sprintf(tmp, "%s/%s", texture_dir, "attack-box.jpg");
    attackBoxTex = Environnement::wall_texture(tmp);
    sprintf(tmp, "%s/%s", texture_dir, "cooldown-box.jpg");
    cooldownBoxTex = Environnement::wall_texture(tmp);
    sprintf(tmp, "%s/%s", texture_dir, "projectiles-box.jpg");
    projectilesBoxTex = Environnement::wall_texture(tmp);

    // Chargement du fichier
    std::ifstream file;
    try
    {
        file.open(filename);
    }
    catch (const std::exception& e)
    {
        ERROR("Could not open file " << filename << ". Message: " << e.what());
    }
    reset();

    // Parsing
    parsePosters(file);
    parseMaze(file);
    file.close();

    // Initialisation des données
    m_data = create2DArray<char>(m_width, m_height, _EMPTY);
    fillData();
    firstFlood();
    fillDataMovers();

    if (mainMusic)
    {
        mainMusic->play();
    }
}

Labyrinthe::~Labyrinthe()
{
    // Jamais appelé visiblement donc on ne saura jamais si ce code est correct :(
    free2DArray(m_data, m_height);
    free2DArray(m_distances, m_height);

    for (int i = 0; i < NB_POSTERS; ++i)
    {
        delete[] m_posters[i];
    }
}

int Labyrinthe::wall_texture(uint index)
{
    if (index < NB_POSTERS)
    {
        return Environnement::wall_texture(m_posters[index]);
    }
    ERROR("Labyrinthe::wall_texture(uint): invalid index " << index << ".");
}

char Labyrinthe::data(int x, int y)
{
    if (x >= 0 && x < (int) m_width && y >= 0 && y < (int) m_height)
    {
        return m_data[y][x];
    }
    else
    {
        return 1;
    }
}

CellType Labyrinthe::getCellType(int x, int y)
{
    switch (data(x, y))
    {
    case _EMPTY:
        return _EMPTY;

    case WALL:
        return WALL;

    case TREASURE:
        return TREASURE;

    case HEALTH:
        return HEALTH;

    default:
        return CMOVER;
    }
}

CellType Labyrinthe::getCellType(CMover* mover, int x, int y)
{
    auto r = getCellType(x, y);
    if (mover)
    {
        switch (r)
        {
        case CMOVER:
            r = ((uint) m_data[y][x] == mover->id() + CMOVER) ? _EMPTY : CMOVER;
            break;

        default:
            break;
        }
    }
    return r;
}

Hunter* Labyrinthe::getHunter() { return static_cast<Hunter*>(_guards[0]); }

CMover* Labyrinthe::getMover(int x, int y)
{
    char d = m_data[y][x];
    return (d >= CMOVER) ? static_cast<CMover*>(_guards[d - CMOVER]) : nullptr;
}

uint Labyrinthe::getNbGuards() const { return _nguards - Weapon::maxNbBalls - 1; }

Guard* Labyrinthe::getGuard(uint index)
{
    return static_cast<Guard*>(_guards[index + Weapon::maxNbBalls + 1]);
}

uint Labyrinthe::distanceFromTreasure(int x, int y) const
{
    if (x >= 0 && x < (int) m_width && y >= 0 && y < (int) m_height)
    {
        return m_distances[y][x];
    }
    else
    {
        WARNING("Labyrinthe::distanceFromTreasure: out-of-bounds access");
        return std::numeric_limits<uint>::max();
    }
}

FireBallDX* Labyrinthe::getFireBall(uint index) const
{
    if (index >= Weapon::maxNbBalls)
    {
        ERROR("Labyrinthe::getFireBall: invalid index " << index);
    }
    return static_cast<FireBallDX*>(_guards[index + 1]);
}

bool Labyrinthe::checkBoxes(int x, int y)
{
    // Y a-t-il une caisse avec laquelle on peut interagir ? Si non, on quitte
    int data = m_data[y][x];
    if (data < HEALTH || data > PROJECTILES)
    {
        return false;
    }

    // On applique l'interaction
    Hunter* hunter = getHunter();
    Weapon& w = hunter->weapon();
    switch (data)
    {
    case HEALTH:
        hunter->hit(nullptr, -20);
        break;

    case ATTACK:
    {
        play(weaponBonusSound);
        w.setDamage(w.getDamage() + 10);
        message("PV: %d - Your damage output has increased!", hunter->getPV());
        break;
    }

    case COOLDOWN:
    {
        play(weaponBonusSound);
        int cooldown = w.getCooldown();
        if (cooldown < 20)
        {
            message("PV: %d - Your cooldown could not be decreased further.", hunter->getPV());
        }
        else
        {
            message("PV: %d - Your cooldown has decreased!", hunter->getPV());
            w.setCooldown(cooldown - 20);
        }
        break;
    }

    case PROJECTILES:
    {
        play(weaponBonusSound);
        int nb = w.getNbBalls();
        if (nb > 9)
        {
            message("PV: %d - You cannot fire more than 11 fire balls at once.", hunter->getPV());
        }
        else
        {
            message("PV: %d - You can now fire 2 extra balls at once!", hunter->getPV());   
            w.setNbBalls(nb + 2);
        }
        break;
    }

    default:
        // Ne devrait jamais arriver
        break;
    }

    // On cherche la boîte dont il est question
    for (int i = 0; i < _nboxes; ++i)
    {
        Box* box = _boxes + i;
        if (box->_x == x && box->_y == y)
        {
            // Trouvé !
            Box* newBoxes = removeFromArray<Box>(_boxes, _nboxes, i);
            delete[] _boxes;
            _boxes = newBoxes;
            _nboxes--;
            reevaluate();
            return true;
        }
    }

    ERROR("Labyrinthe::checkBoxes: corrupted data");
    return false;
}

void Labyrinthe::partie_terminee(bool victory)
{
    if (mainMusic)
    {
        mainMusic->stop();
    }
    ::partie_terminee(victory);
    if (victory && victoryMusic)
    {
        victoryMusic->play();
    }
}