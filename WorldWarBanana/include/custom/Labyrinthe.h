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
    
    void fillData();
    void flood();

public:
    Labyrinthe(char*);
    virtual ~Labyrinthe();

    int width() override { return m_width; }

    int height() override { return m_height; }

    // retourne l'état (occupation) de la case (x, y).
    char data(int x, int y) override;
};

#endif
