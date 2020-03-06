#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"

class Labyrinthe: public Environnement 
{
private:
	char	**_data;	// indique si la case est libre ou occup�e.
	int		lab_width;	// dimensions du rectangle.
	int		lab_height;	// englobant le labyrinthe.

public:
	Labyrinthe (char*);

	int width() const override 
	{ 
		return lab_width;
	}

	int height() const override 
	{ 
		return lab_height;
	}

	// retourne l'�tat (occupation) de la case (i, j).
	char data (int i, int j)
	{
		return _data[i][j];
	}
};

#endif
