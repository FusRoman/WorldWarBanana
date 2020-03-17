#include <fstream>
#include <string>
#include <iostream>

#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

Sound *Chasseur::_hunter_fire; // bruit de l'arme du chasseur.
Sound *Chasseur::_hunter_hit;  // cri du chasseur touch�.
Sound *Chasseur::_wall_hit;	// on a tap� un mur.

Environnement *Environnement::init(char *filename)
{
	return new Labyrinthe(filename);
}

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
	//  (attention: pour des raisons de rapport d'aspect, les affiches doivent faire 2 de long)
	n = 0;
	_picts = new Wall [2];
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
			if (i == 0 || i == lab_width-1 || j == 0 || j == lab_height-1)
				_data [i][j] = 1;
			else
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
	_guards [3] = new Gardien (this, "Potator"); _guards [3] -> _x = 60.; _guards [3] -> _y = 10.;
	_guards [4] = new Gardien (this, "garde"); _guards [4] -> _x = 130.; _guards [4] -> _y = 100.;
	_guards [5] = new Gardien (this, "Droid"); _guards [5] -> _x = 130.; _guards [5] -> _y = 70.;
	_guards [6] = new Gardien (this, "Lezard"); _guards [personnage6] -> _x = 160.; _guards [6] -> _y = 100.;
	_guards [7] = new Gardien (this, "Samourai"); _guards [7] -> _x = 160.; _guards [7] -> _y = 70.;
	_guards [8] = new Gardien (this, "Serpent"); _guards [8] -> _x = 190.; _guards [8] -> _y = 100.;
	_guards [9] = new Gardien (this, "Squelette"); _guards [9] -> _x = 190.; _guards [9] -> _y = 70.;
	_guards[10] = new Gardien (this, "Blade"); _guards[10] -> _x = 220.; _guards[10] -> _y = 100.;

	// indiquer qu'on ne marche pas sur les gardiens.
	for (int i = 1; i < _nguards; ++i)
	{
		_data [(int)(_guards [i] -> _x / scale)][(int)(_guards [i] -> _y / scale)] = 1;
	}
}*/

bool isSpace(char c)
{
	switch (c)
	{
	case ' ':
	case '\t':
	case '\n':
	case '\r':
		return true;

	default:
		return false;
	}
}

bool isLowerAlpha(char c)
{
	return c >= 'a' && c <= 'z';
}

// Renvoie l'index du premier caractère non blanc (ou size si aucun)
int ignoreSpace(const std::string &line, unsigned int start)
{
	while (start < line.size() && isSpace(line[start]))
	{
		++start;
	}
	return start;
}

// Renvoie l'index du dernier caractère non blanc ( ou size si aucun)
int ignoreNonSpace(const std::string &line, unsigned int start)
{
	while (start < line.size() && !isSpace(line[start]))
	{
		++start;
	}
	return start;
}

// renvoie l'index de l'affiche correspondant au caractère passé en paramètre.
inline int indexOfPoster(char c)
{
	return c - 'a';
}

/*int readingWall(const std::string &line, int startWall, Wall *wall)
{personnage
	while (startWall < line.size() && line[startWall] != '+')
	{
		if()
	}
}*/

/*
void makeWall(const std::string &line, Wall *wall, int nWall, int labHeigth)
{
	int startWall = ignoreSpace(line, 0);
	int endWall = 0;
	while (startWall < line.size())
	{
		wall[nWall]._y1 = labHeigth;
		wall[nWall]._y2 = labHeigth;
		wall[nWall]._x1 = startWall;
		endWall = ignoreNonSpace(line, startWall);
		if (endWall == line.size())
		{
			std::cerr << "undefined wall" << std::endl;
		}
		else
		{
			wall[nWall]._x2 = endWall;
		}
		nWall++;
	}
}personnage
*/

Labyrinthe::Labyrinthe(char *filename)
{
	// Chargement du fichier
	std::ifstream file;
	try
	{
		file.open(filename);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Could not open file " << filename << ". Message: " << e.what() << '\n';
		exit(1);
	}

	// Première étape : commentaires et affiches

	// ce tableau de string contient le chemin vers toutes les affiches du labyrinthe (max 26) 
	std::string *arrayOfPathPoster = new std::string[26]; // solution fainéante
	unsigned int n_picts = 0;
	std::string currentLine;
	unsigned int beginLaby = 0;
	// On lit l'en-tête du fichier
	while (std::getline(file, currentLine))
	{
		// On ignore les espaces préliminaires
		unsigned int i = ignoreSpace(currentLine, 0);

		// Si la ligne est vide, ou c'est un commentaire
		if (i >= currentLine.size() || currentLine[i] == '#')
		{
			continue;
		}

		if (currentLine[i] == '+')
		{
			// On est (si le fichier est correct) sur la description du labyrinthe
			// (là on ignorerait la première ligne du laby, c pa bi1)
			break;
		}
		else if (isLowerAlpha(currentLine[i]))
		{
			// On est sur une ligne qui associe une lettre minuscule à une affiche
			int startPict = ignoreSpace(currentLine, i + 1);
			int endPict = ignoreNonSpace(currentLine, startPict);
			std::string picture("");
			picture.append(texture_dir);
			picture.append("/" + currentLine.substr(startPict, endPict - startPict));
			arrayOfPathPoster[n_picts] = picture;
			n_picts++;
		}
		//récupére la position du curseur de lecture du fichier
		beginLaby = file.tellg();
	}

	/*1er passe : récupération des informations du labyrinthe
		- nombre de mur
		- nombre de garde
		- nombre d'affiche
		- nombre de boite
		- taille du rectangle englobant le labyrinthe
	*/
	while (std::getline(file, currentLine))
	{

	}

	//retour au debut du labyrinthe
	file.seekg(beginLaby);

	/*2er passe : construction des objets du labyrinthe
		- pour la constructions des murs, les coordonnées x1 et y1 corresponde au
			coordonnée de début du mur et x2, y2 les coordonnée de fin. De plus, _n_tex est
			un identifiant pour la texture du mur ( par defaut, c'est 0)
		- les affiches sont juste des murs qui font 2 de longueur, il diffère des autres
			mur par leurs identifiant d'affiche _n_tex qu'il faut set avec la focntion
			wall_texture qui prend en paramètre le chemin vers l'image de l'affiche d'ou
			l'utilité du tableau arrayOfPathPoster. La bonne affiche en fonction
			du caractère dans le fichier est récupérable par la fontion
			indexOfPoster(char c).
	*/	
	while (std::getline(file, currentLine))
	{

	}
}