#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include <iostream>
#include <fstream>

#include "Environnement.h"

class Labyrinthe : public Environnement
{
private:
    char** m_data;   // indique si la case est libre ou occup�e.
    uint   m_width;  // dimensions du rectangle.
    uint   m_height; // englobant le labyrinthe.

	std::string* parsePosters(std::ifstream& file);
    void loadPosters(std::string* paths);

	void parseMaze(std::ifstream& file);
    
    void flood();

public:
    Labyrinthe(char*);

    int width() override { return m_width; }

    int height() override { return m_height; }

    // retourne l'�tat (occupation) de la case (i, j).
    char data(int i, int j) override { return m_data[i][j]; }
};

#endif
