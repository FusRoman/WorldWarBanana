#include "Labyrinthe.h"

#include <string>
#include <vector>
#include <list>
#include <limits>

#include "Chasseur.h"
#include "Gardien.h"
#include "utils.h"
#include "macros.h"

Sound* Chasseur::_hunter_fire; // bruit de l'arme du chasseur.
Sound* Chasseur::_hunter_hit;  // cri du chasseur touch�.
Sound* Chasseur::_wall_hit;    // on a tap� un mur.

Environnement* Environnement::init(char* filename) { return new Labyrinthe(filename); }

/*
 *	EXEMPLE de labyrinthe.
 */
/*
Labyrinthe::Labyrinthe (char* filename)
{
        // taille du labyrinthe.
        lab_height = 80;
        lab_width = 25;

        // les murs: 4 dans cet EXEMPLE!
        int	n = 0;

        _walls = new Wall [4];
        // le premier.
        _walls [n]._x1 = 0; _walls [n]._y1 = 0;
        _walls [n]._x2 = 0; _walls [n]._y2 = lab_height-1;
        _walls [n]._ntex = 0;
        ++n;
        // le deuxieme.
        _walls [n]._x1 = 0; _walls [n]._y1 = lab_height-1;
        _walls [n]._x2 = lab_width-1; _walls [n]._y2 = lab_height-1;
        _walls [n]._ntex = 0;
        ++n;
        // le troisieme.
        _walls [n]._x1 = lab_width-1; _walls [n]._y1 = lab_height-1;
        _walls [n]._x2 = lab_width-1; _walls [n]._y2 = 0;
        _walls [n]._ntex = 0;
        ++n;
        // le dernier.
        _walls [n]._x1 = lab_width-1; _walls [n]._y1 = 0;
        _walls [n]._x2 = 0; _walls [n]._y2 = 0;
        _walls [n]._ntex = 0;
        ++n;
        _nwall = n;

        // une affiche.
        //  (attention: pour des raisons de rapport d'aspect, les affiches
doivent faire 2 de long) n = 0; _picts = new Wall [2];
        // la premi�re (texture par d�faut).
        _picts [n]._ntex = 0;
        _picts [n]._x1 = 0; _picts [n]._y1 = 4;
        _picts [n]._x2 = 0; _picts [n]._y2 = 6;
        ++n;

        // la deuxi�me a une texture diff�rente.
        char	tmp [128];
        sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");

        _picts [n]._ntex = wall_texture (tmp);
        _picts [n]._x1 = 0; _picts [n]._y1 = 8;
        _picts [n]._x2 = 0; _picts [n]._y2 = 10;
        ++n;
        _npicts = n;

        // 3 caisses.
        _boxes = new Box [3];

        n = 0;
        // la premi�re.
        _boxes [n]._x = 12; _boxes [n]._y = 70; _boxes [n]._ntex = 0;
        ++n;
        // la deuxieme.
        _boxes [n]._x = 5; _boxes [n]._y = 10; _boxes [n]._ntex = 0;
        ++n;
        // la derni�re.
        _boxes [n]._x = 22; _boxes [n]._y = 65; _boxes [n]._ntex = 0;
        ++n;
        _nboxes = n;

        // cr�ation du tableau d'occupation du sol.
        _data = new char* [lab_width];
        for (int i = 0; i < lab_width; ++i)
                _data [i] = new char [lab_height];
        // initialisation du tableau d'occupation du sol.
        for (int i = 0; i < lab_width; ++i)
                for (int j = 0; j < lab_height; ++j) {
                        // murs sur les bords.
                        if (i == 0 || i == lab_width-1 || j == 0 || j ==
lab_height-1) _data [i][j] = 1; else
                                // rien dedans.
                                _data [i][j] = EMPTY;
                }

        // indiquer qu'on ne marche pas sur une caisse.
        _data [_boxes [0]._x][_boxes [0]._y] = 1;
        _data [_boxes [1]._x][_boxes [1]._y] = 1;
        _data [_boxes [2]._x][_boxes [2]._y] = 1;

        // coordonn�es du tr�sor.
        _treasor._x = 10;
        _treasor._y = 10;
        // indiquer qu'on ne marche pas dessus.
        _data [_treasor._x][_treasor._y] = 1;

        // le chasseur et les 4 gardiens.
        _nguards = 1+10;
        _guards = new Mover* [_nguards];
        _guards [0] = new Chasseur (this);
        _guards [1] = new Gardien (this, "drfreak");
        _guards [2] = new Gardien (this, "Marvin"); _guards [2] -> _x = 90.; _guards [2] -> _y = 70.; 
        _guards [3] = new Gardien (this, "Potator"); _guards[3] -> _x = 60.; _guards [3] -> _y = 10.; 
        _guards [4] = new Gardien (this, "garde"); _guards [4] -> _x = 130.; _guards [4] -> _y = 100.; 
        _guards [5] = new Gardien (this, "Droid"); _guards [5] -> _x = 130.; _guards [5] -> _y = 70.;
        _guards [6] = new Gardien (this, "Lezard"); _guards [personnage6] -> _x = 160.; _guards [6] -> _y = 100.; 
        _guards [7] = new Gardien (this, "Samourai");
_guards [7] -> _x = 160.; _guards [7] -> _y = 70.; _guards [8] = new Gardien
(this, "Serpent"); _guards [8] -> _x = 190.; _guards [8] -> _y = 100.; _guards
[9] = new Gardien (this, "Squelette"); _guards [9] -> _x = 190.; _guards [9] ->
_y = 70.; _guards[10] = new Gardien (this, "Blade"); _guards[10] -> _x = 220.;
_guards[10] -> _y = 100.;

        // indiquer qu'on ne marche pas sur les gardiens.
        for (int i = 1; i < _nguards; ++i)
        {
                _data [(int)(_guards [i] -> _x / scale)][(int)(_guards [i] -> _y
/ scale)] = 1;
        }
}*/

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
        std::cerr << "Error line " << cursor.line << ", column " << cursor.column
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

    // Renvoie l'index de l'affiche correspondant au caractère passé en paramètre.
    inline int indexOfPoster(char c) { return c - 'a'; }
}
using namespace Labyrinthe_private;


/**************************************************************************************************
 *
 * Gestion des posters
 *
 *************************************************************************************************/

#define NB_POSTERS 26

std::string* Labyrinthe::parsePosters(std::ifstream& file) 
{
	// Dossier contenant les images des posters
	std::string root(texture_dir);
	root.append("/");

	// Array contenant les chemins vers les affiches
	// (maximum puisque les posters sont identifiés par une lettre minuscule)
    std::string* postersPath = new std::string[NB_POSTERS]; // solution fainéante

	// On initialise tous les posters avec une image par défaut
	std::string _default(root + "default_poster.jpg");
	for (int i = 0; i < NB_POSTERS; ++i) {
		postersPath[i] = _default;
	}

	std::string currentLine;
	auto currentPos = file.tellg();
    Cursor saved = cursor;
    while (std::getline(file, currentLine))
    {
        newLine();

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
                return postersPath;
            }
            else if (isLowerAlpha(currentLine[i]))
            {
                // On est sur une ligne qui associe une lettre minuscule à une affiche
                int startPict = ignoreSpace(currentLine, i + 1);
                int endPict   = ignoreNonSpace(currentLine, startPict);
                int index = currentLine[i] - 'a';
                postersPath[index] = root;
                postersPath[index].append(currentLine.substr(startPict, endPict - startPict));

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
    }

	error("No maze provided");
    return nullptr; // Juste pour le compilateur
}

void Labyrinthe::loadPosters(std::string* paths)
{
    // TODO
    for (int i = 0; i < NB_POSTERS; ++i) 
    {
        DEBUG(i << ": " << paths[i]);
    }
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
        POSTER,
        GUARD,
        HUNTER,
        BOX,
        TREASURE,
        NOTHING,
        END
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
        int indexBoxes;
        int indexGuards;
        int indexPosters;

        // Fonction de transition
        void (*transition)(Labyrinthe* laby, ParserState& state, char c);
    };

    #define END_CHAR '$'
    Type _type(char c, const ParserState& state)
    {
        if (isLowerAlpha(c))
        {
            return POSTER;
        }

        switch (c)
        {
        case '+':
            return CROSS;

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
            return (state.horizontal)? ALIGNED_WALL : PERPENDICULAR_WALL;

        case '|':
            return (state.horizontal)? PERPENDICULAR_WALL : ALIGNED_WALL;

        case END_CHAR:
            return END;

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
            error("Met '|' while reading a horizontal wall. You may want to replace '|' with '+'.");
        }
        else
        {
            error("Met '-' while reading a vertical wall. You may want to replace '-' with '+'.");
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

    // On a lu un nombre impair de caractères pour le mur actuel
    // => Le mur peut éventuellement se terminer au caractère suivant
    void wallOdd(Labyrinthe*, ParserState&, char);

    // On a lu un nombre pair de caractères pour le mur actuel et il n'est pas sur le point d'être fini
    // => on doit avoir au moins un autre caractère pour ce mur
    void wallEven(Labyrinthe*, ParserState&, char);

    // Le mur actuel s'est peut-être fini en lisant le dernier caractère
    // Mais il peut quand même continuer après
    // Cet état peut être vu comme un sous-cas de wallEven
    void endOfWall(Labyrinthe*, ParserState&, char);

    void initial(Labyrinthe* laby, ParserState& state, char c)
    {
        DEBUG("state: initial");
        switch (_type(c, state))
        {
        case ALIGNED_WALL:
        case POSTER:
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
            state.transition = wallOdd;
            break;

        case PERPENDICULAR_WALL:
        case NOTHING:
        case END:
            // Rien à faire
            break;


        default:
            placeObject(laby, state, c);
            break;
        }
    }

    void wallOdd(Labyrinthe* laby, ParserState& state, char c)
    {
        DEBUG("state: wallOdd");
        switch (_type(c, state))
        {
        case CROSS:
            // Le mur pourrait être fini maintenant
            // Mais pas forcément, on met donc le parser dans un état adapté
            state.transition = endOfWall;
            break;

        case ALIGNED_WALL:
            state.transition = wallEven;
            break;

        case PERPENDICULAR_WALL:
            errorOnPerpendicularWall(state);

        case POSTER:
            if (state.horizontal)
            {
                error("Met a poster after parsing an odd number of horizontal wall characters. \
                    You may want to move the poster right or left.");
            }
            else
            {
                error("Met a poster after parsing an odd number of vertical wall characters. \
                    You may want to move the poster up or down.");
            }

        default:
            errorOnEnd(state);
        }
    }

    void wallEven(Labyrinthe* laby, ParserState& state, char c)
    {
        DEBUG("state: wallEven");
        switch (_type(c, state))
        {
        case POSTER: {
            Wall* pict = laby->_picts + state.indexPosters;
            //pict->_ntex = laby->wall_texture(...);
            pict->_ntex = 0;
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
            // Pas de break ici ! On traite les posters comme des murs normaux par ailleurs
        }

        case CROSS:
        case ALIGNED_WALL:
            // Si on rencontre une croix ici, ça ne peut pas être la fin du mur, 
            // donc au final on le considère comme un mur simple
            // (indépendamment du fait qu'un autre mur perpendiculaire passe par là ou pas)
            state.transition = wallOdd;
            break;

        case PERPENDICULAR_WALL:
            errorOnPerpendicularWall(state);

        default:
            errorOnEnd(state);
        }
    }

    void endOfWall(Labyrinthe* laby, ParserState& state, char c)
    {
        DEBUG("state: endOfWall");
        switch(_type(c, state))
        {
            // 3 case suivants : le mur ne s'est pas terminé finalement
            // Pareil que pour wallEven
            case POSTER:
            case CROSS:
            case ALIGNED_WALL:
                state.transition = wallOdd;
                wallOdd(laby, state, c);
                break;

            // Pour les autres cas, le mur ne continue pas
            // On l'ajoute, puis on revient sur le cas initial
            default:
                state.currentWall._x2 = state.x;
                state.currentWall._y2 = state.y;
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
    uint xmin   = std::numeric_limits<uint>::max(),
         xmax   = std::numeric_limits<uint>::min();
    uint i = 0; // Index de la ligne (la première ligne n'est jamais vide grâce à l'implémentation de parsePosters)
    std::vector<std::string> buffer;
    std::string line;
    DEBUG("Lexical analysis");
    while (std::getline(file, line))
    {
        newLine();
        buffer.push_back(line);
        uint left = xmin, right = xmax;
        bool leftSet = false;
        for (uint x = 0; x < line.size(); ++x)
        {
            if (line[x] == '\t')
            {
                error("Tabulations are not allowed in the description of the maze for readability purpose.");
            }

            if (line[x] == END_CHAR)
            {
                // END_CHAR a un statut particulier puisque le signe est utilisé pour signaler aux états
                // que la ligne / colonne est finie
                // Mais END_CHAR ne doit pas apparaître dans le fichier
                // Si on donne END_CHAR à _type, il renverra END et le problème ne sera pas perçu,
                // donc on teste ici
                stray(END_CHAR);
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
    }

    m_width = xmax - xmin + 1;
    ++m_height; // m_height est une dimension, or on lui a assigné des valeurs d'index : +1 pour compenser
    DEBUG("Width: " << m_width << " (xmin: " << xmin << ", xmax: " << xmax << ")");
    DEBUG("Height: " << m_height);

    if (!treasureSet)
    {
        error("No treasure found.");
    }
    if (!hunterSet)
    {
        error("No starting point found");
    }
    DEBUG("Lexical analysis done" << std::endl);
    // Fin de la première passe

    // Préparation pour les deux passes suivantes (allocations essentiellement)
    _picts = new Wall[_npicts];
    _boxes = new Box[_nboxes];
    _guards = new Mover*[_nguards];
    _guards[0] = new Chasseur(this);
    for (int i = 1; i < _nguards; ++i)
    {
        int rd = randomInt(0, Gardien::modeles.size() - 1);
        _guards[i] = new Gardien(this, rd);
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
        initial     // transition
    };

    // TODO Gérer les croisements avec des posters
    // Deuxième passe : lecture horizontale
    DEBUG("Horizontal analysis");
    for (uint y = 0; y < buffer.size(); ++y)
    {
        cursor.line = saved.line + y;
        // On considère que (0, 0) est le point le plus en bas à gauche
        // On inverse donc sur l'axe y
        state.y = m_height - y - 1;
        uint end = min<uint>(xmax + 1, buffer[y].size());
        for (uint x = xmin; x < end; ++x)
        {
            cursor.column = x;
            state.x = x - xmin;
            DEBUG("[H] Read '" << buffer[y][x] << "'");
            state.transition(this, state, buffer[y][x]);
        }
        DEBUG("[H] New line");
        state.transition(this, state, END_CHAR);
        state.transition = initial;
    }
    DEBUG("Horizontal analysis done" << std::endl);

    // Deuxième passe : lecture verticale
    DEBUG("Vertical analysis");
    state.horizontal = false;
    DEBUG("Height: " << m_height);
    for (uint x = xmin; x <= xmax; ++x)
    {
        cursor.column = x;
        state.x = x - xmin;
        for (uint y = 0; y < m_height; ++y)
        {
            cursor.line = saved.line + y;
            state.y = m_height - y - 1;
            char c = (x < buffer[y].size())? buffer[y][x] : ' ';
            DEBUG("[V] Read '" << c << "'");
            state.transition(this, state, c);
        }
        DEBUG("[V] New column");
        state.transition(this, state, END_CHAR);
        state.transition = initial;
    }
    DEBUG("Vertical analysis done" << std::endl);

    _nwall = state.walls.size();
    _walls = toArray(state.walls);
}


/**************************************************************************************************
 *
 * Algorithme d'inondation
 *
 *************************************************************************************************/

void Labyrinthe::flood()
{

}


/**************************************************************************************************
 *
 * Constructeur
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
    auto posters = parsePosters(file);
    loadPosters(posters);
    delete[] posters;

    DEBUG(2);

    // Chargement du niveau
    parseMaze(file);
    DEBUG(3);

    file.close();
    DEBUG(4);
}