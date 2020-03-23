#include "Labyrinthe.h"

#include <string.h>

#include <string>
#include <vector>
#include <list>
#include <limits>

#include "Hunter.h"
#include "Guard.h"
#include "utils.h"
#include "macros.h"

Environnement* Environnement::init(char* filename) { return new Labyrinthe(filename); }

// On utilise ce namespace pour ne pas que ces fonctions qu'on désire n'utiliser que dans ce fichier
// ne rentrent pas en conflit avec d'autres fonctions locales à un fichier
// (on pourrait aussi utiliser static il me semble)
namespace Labyrinthe_private {

    typedef struct {
        int line;
        int column;
    } Cursor;

    Cursor cursor = {line: 0, column: 0};

    void reset() {
        cursor.line = 0;
        cursor.column = 0;
    }

    void newLine() {
        ++cursor.line;
        cursor.column = 0;
    }

    void error(const std::string& msg) 
    {
        std::cerr << "Error line " << cursor.line + 1 << ", column " << cursor.column + 1
            << ": " << msg << std::endl;
        exit(1);
    }

    void stray(char c)
    {
        error(std::string("Stray character ") + c
            + " (ASCII: " + std::to_string((int) c) + ")");
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
}
using namespace Labyrinthe_private;


/**************************************************************************************************
 *
 * Gestion des posters
 *
 *************************************************************************************************/

void Labyrinthe::storePoster(int index, const char* path)
{
    if (m_posters[index])
    {
        delete[] m_posters[index];
    }

    size_t dirlen = strlen(texture_dir);
    size_t pathlen = strlen(path);
    m_posters[index] = new char[dirlen + pathlen + 1];
    strcpy(m_posters[index], texture_dir);
    m_posters[index][dirlen] = '/';
    strcpy(m_posters[index] + dirlen + 1, path);
}

void Labyrinthe::parsePosters(std::ifstream& file) 
{
	// On initialise tous les posters avec une image par défaut
	for (int i = 0; i < NB_POSTERS; ++i) {
        m_posters[i] = nullptr;
        storePoster(i, "default_poster.jpg");
	}

	std::string currentLine;
	auto currentPos = file.tellg();
    Cursor saved = cursor;
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

                uint endPict   = ignoreNonSpace(currentLine, startPict);
                const char* path = currentLine.substr(startPict, endPict - startPict).c_str();
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
                //error("Stray character " + currentLine.substr(i, i+1));
                stray(currentLine[i]);
            }
        }

		currentPos = file.tellg();
        saved = cursor;
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
        GUARD,
        HUNTER,
        BOX,
        TREASURE,
        NOTHING
    };

    // Structure de données pour le parser
    class ParserState {
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
        uint mazeX, mazeY;

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
            return (x <= state.maze[y].size())? state.maze[y][x] : ' ';
        }
    }

    // A appeler quand on lit un poster et qu'on ne sait pas dans quelle direction il est orienté
    // Renvoie true si le poster est aligné
    // saved est le curseur positionné sur le tout début du labyrinthe
    Type getPosterOrientation(const ParserState& state)
    {
        const int x = state.mazeX;
        const int y = state.mazeY;
        // Sensible à l'overflow (mais a priori ce n'est pas notre principal souci 
        // si les labyrinthes sont assez grands pour causer ce genre de problèmes...)
        char left   = getChar(state, x - 1, y), 
             right  = getChar(state, x + 1, y), 
             up     = getChar(state, x, y - 1),
             bottom = getChar(state, x, y + 1);
        bool h = left == '-' || left == '+' || right == '-' || right == '+',
             v = up == '|' || up == '+' || bottom == '|' || bottom == '+';
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
            return h? ALIGNED_POSTER : PERPENDICULAR_POSTER;
        }
        else
        {
            return v? ALIGNED_POSTER : PERPENDICULAR_POSTER;
        }
    }

    // Utile pour régler certains cas problématiques
    // Lors de l'analyse horizontale, rencontrer une croix sans murs alignés après ou avant
    // n'est pas forcément une erreur, s'il y a des murs verticaux au-dessus ou en-dessous
    // Cette fonction permet de considérer de tels croix comme des murs verticaux
    // Le parser peut s'occuper de tous les autres cas
    Type getCrossOrientation(const ParserState& state)
    {
        const int x = state.mazeX;
        const int y = state.mazeY;
        char left   = getChar(state, x - 1, y), 
             right  = getChar(state, x + 1, y), 
             up     = getChar(state, x, y - 1),
             bottom = getChar(state, x, y + 1);
        bool h = left == '-' || left == '+' || right == '-' || right == '+',
             v = up == '|' || up == '+' || bottom == '|' || bottom == '+';
        if (state.horizontal)
        {
            return (!h && v)? PERPENDICULAR_WALL : CROSS;
        }
        else
        {
            return (!v && h)? PERPENDICULAR_WALL : CROSS;
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
            return NOTHING;

        case 'G':
            return GUARD;

        case 'T':
            return TREASURE;

        case 'X':
            return BOX;

        case 'C':
            return HUNTER;

        case '-':
            return state.horizontal? ALIGNED_WALL : PERPENDICULAR_WALL;

        case '|':
            return state.horizontal? PERPENDICULAR_WALL : ALIGNED_WALL;

        default:
            stray(c);
            return NOTHING; // Juste pour le compilateur
        }
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
        case 'X': {
            Box* box = laby->_boxes + state.indexBoxes;
            box->_x = state.x;
            box->_y = state.y;
            box->_ntex = 0;
            ++state.indexBoxes;
            break;
        }

        case 'G': {
            Mover* guard = laby->_guards[state.indexGuards];
            guard->_x = state.x * Environnement::scale;
            guard->_y = state.y * Environnement::scale;
            ++state.indexGuards;
            break;
        }

        case 'T':
            // Grâce à la première passe, on sait qu'il n'y a qu'un seul trésor
            laby->_treasor._x = state.x;
            laby->_treasor._y = state.y;
            break;

        case 'C': {
            Mover* hunter = laby->_guards[0];
            hunter->_x = state.x * Environnement::scale;
            hunter->_y = state.y * Environnement::scale;
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
            error("Met a vertical wall while reading a horizontal wall. You may want to put '+' at the intersection.");
        }
        else
        {
            error("Met a horizontal wall while reading a vertical wall. You may want to put '+' at the intersection.");
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
            state.transition = wall;
            break;

        case PERPENDICULAR_WALL:
        case PERPENDICULAR_POSTER:
        case NOTHING:
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

        case ALIGNED_POSTER: {
            Wall* pict = laby->_picts + state.indexPosters;
            pict->_ntex = laby->wall_texture(c - 'a');
            //pict->_ntex = 0;
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
}

void Labyrinthe::parseMaze(std::ifstream& file) 
{
    _npicts  = 0;
    _nwall   = 0;
    _nguards = 0;
    _nboxes  = 0;
    m_width = 0;
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
    Cursor saved = cursor;
    bool treasureSet = false,
         hunterSet   = false;
    uint xmin = std::numeric_limits<uint>::max(),
         xmax = std::numeric_limits<uint>::min();
    uint i = 0; // Index de la ligne (la première ligne n'est jamais vide grâce à l'implémentation de parsePosters)
    std::vector<std::string> maze;
    std::string line;
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
                error("Tabulations are not allowed in the description of the maze for readability purpose.");
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
                left = x;
                leftSet = true;
            }
            right = x;
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
    ++m_height; // m_height est une dimension, or on lui a assigné des valeurs d'index : +1 pour compenser

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
    _picts = new Wall[_npicts];
    _boxes = new Box[_nboxes];
    _guards = new Mover*[_nguards];
    _guards[0] = new Hunter(this, 0);
    for (int i = 1; i < _nguards; ++i)
    {
        int rd = randomInt(0, Guard::modeles.size() - 1);
        _guards[i] = new Guard(this, rd, i);
    }

    std::list<Wall> walls;
    ParserState state =
    {
        0,          // x
        0,          // y
        true,       // horizontal
        Wall(),     // currentWall
        walls,      // walls
        0,          // indexBoxes
        1,          // indexGuards (0 = hunter)
        0,          // indexPosters
        maze,       // maze
        0,          // mazeX
        0,          // mazeY
        initial     // transition
    };

    // Deuxième passe : lecture horizontale
    for (uint y = 0; y < maze.size(); ++y)
    {
        cursor.line = saved.line + y;
        // On considère que (0, 0) est le point le plus en bas à gauche
        // On inverse donc sur l'axe y
        state.y = m_height - y - 1;
        state.mazeY = y;
        uint end = min<uint>(xmax + 1, maze[y].size());
        for (uint x = xmin; x < end; ++x)
        {
            cursor.column = x;
            state.x = x - xmin;
            state.mazeX = x;
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
        state.x = x - xmin;
        state.mazeX = x;
        for (uint y = 0; y < m_height; ++y)
        {
            cursor.line = saved.line + y;
            state.y = m_height - y - 1;
            state.mazeY = y;
            char c = (x < maze[y].size())? maze[y][x] : ' ';
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
        int x;
        int y;
        uint d;
    } BFSData;

    void floodNeighbour(Labyrinthe* laby, int x, int y, uint d, std::list<BFSData>& stack)
    {
        if (x < 0 || x >= laby->width() || y < 0 || y >= laby->height())
        {
            std::cerr << "The maze is not closed. Check its borders." << std::endl;
            exit(1);
        }
        if (laby->data(x, y) == EMPTY && laby->distanceFromTreasure(x, y) > d + 1)
        {
            stack.push_back({x, y, d + 1});
        }
    }
}

// Remplit m_data sans considérer les Mover (pour l'algorithme de flood après)
void Labyrinthe::fillData()
{
    // Allocation de m_data et remplissage avec EMPTY
    m_data = new char*[m_height];
    for (uint y = 0; y < m_height; ++y)
    {
        char* row = new char[m_width];
        for (uint x = 0; x < m_width; ++x)
        {
            row[x] = EMPTY;
        }
        m_data[y] = row;
    }

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
                m_data[wall->_y1][x] = !EMPTY;
            }
        }
        else
        {
            // On suppose que le mur est vertical
            int end = max(wall->_y1, wall->_y2);
            for (int y = min(wall->_y1, wall->_y2); y <= end; ++y)
            {
                m_data[y][wall->_x1] = !EMPTY;
            }
        }
    }

    // Gestion des caisses
    for (int i = 0; i < _nboxes; ++i)
    {
        Box* box = _boxes + i;
        m_data[box->_y][box->_x] = !EMPTY;
    }

    // Trésor
    m_data[_treasor._y][_treasor._x] = 1;

    // On ne s'occupe pas des Mover ici
    // Le but était juste de préparer le terrain pour flood
}

void Labyrinthe::flood()
{
    // Allocation de m_distances et remplissage avec le plus grand entier possible
    uint umax = std::numeric_limits<uint>::max();
    m_distances = new uint*[m_height];
    for (uint y = 0; y < m_height; ++y)
    {
        uint* row = new uint[m_width];
        for (uint x = 0; x < m_width; ++x)
        {
            row[x] = umax;
        }
        m_distances[y] = row;
    }

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

    // Le joueur peut-il accéder au trésor ?
    Mover* hunter = _guards[0];
    int x = hunter->_x / scale;
    int y = hunter->_y / scale;
    if (m_distances[y][x] == umax)
    {
        std::cerr << "The player has no way to get to the treasure." << std::endl;
        exit(1);
    }

    // Mettre la case du trésor à umax ?
}

void Labyrinthe::fillDataMovers()
{
    for (int i = 0; i < _nguards; ++i)
    {
        CMover* guard = (CMover*) _guards[i];
        m_data[(int) guard->_y / scale][(int) guard->_x / scale] = guard->id() + 2;
    }
}


/**************************************************************************************************
 *
 * Déplacements
 *
 *************************************************************************************************/

bool Labyrinthe::moveAux(CMover* mover, double dx, double dy)
{
    int x = (mover->_x + dx) / scale;
    int y = (mover->_y + dy) / scale;
    if (m_data[y][x] == EMPTY || (uint) m_data[y][x] == mover->id() + 2)
    {
        // On libère l'ancienne case
        int ox = mover->_x / scale;
        int oy = mover->_y / scale;
        m_data[oy][ox] = EMPTY;

        mover->_x += dx;
        mover->_y += dy;
        m_data[y][x] = mover->id() + 2;

        return true;
    }
    return false;
}

bool Labyrinthe::move(CMover* mover, double dx, double dy)
{
    return moveAux(mover, dx, dy) || moveAux(mover, dx, 0.0) || moveAux(mover, 0.0, dy);
}


/**************************************************************************************************
 *
 * Membres publics
 *
 *************************************************************************************************/

Labyrinthe::Labyrinthe(char* filename)
{
    // Chargement du fichier
    std::ifstream file;
    try
    {
        file.open(filename);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Could not open file " << filename << ". Message: " << e.what() << '\n';
        exit(1);
    }
    reset();
    DEBUG(1);

    // Gestion des affiches
    parsePosters(file);
    DEBUG(2);

    // Chargement du niveau
    parseMaze(file);
    DEBUG(3);

    fillData();
    flood();
    fillDataMovers();
    DEBUG(4);

    file.close();
    DEBUG(5);
}

Labyrinthe::~Labyrinthe()
{
    // Jamais appelé visiblement donc on ne saura jamais si ce code est correct :(
    for (uint y = 0; y < m_height; ++y)
    {
        delete[] m_data[y];
        delete[] m_distances[y];
    }
    delete[] m_data;
    delete[] m_distances;

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
    std::cerr << "Labyrinthe::wall_texture(uint): invalid index " << index << "." << std::endl;
    exit(1);
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

uint Labyrinthe::distanceFromTreasure(int x, int y) const
{
    if (x >= 0 && x < (int) m_width && y >= 0 && y < (int) m_height)
    {
        return m_distances[y][x];
    }
    else
    {
        DEBUG("Warning! Labyrinthe::distanceFromTreasure: out-of-bounds access");
        return std::numeric_limits<uint>::max();
    }
}