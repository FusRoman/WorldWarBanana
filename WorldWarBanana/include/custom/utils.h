#pragma once

#include <list>
#include <math.h>
#include <iostream>

#include "Sound.h"

typedef unsigned int uint;

template <class T> class Vec2
{
public:
    const T x;
    const T y;

    Vec2(T x, T y): x(x), y(y) {}

    float norm() const
    {
        return sqrt(x * x + y * y);
    }

    friend std::ostream& operator<<(std::ostream& os, Vec2 v)
    {
        return os << "(" << v.x << ", " << v.y << ")";
    }
};

typedef Vec2<int>   Vec2i;
typedef Vec2<float> Vec2f;

/**
 * @brief Retourne un entier dans [lower, upper].
 */
int randomInt(int lower, int upper);

/**
 * @brief Retourne un flottant dans [low, upper[.
 */
float randomFloat(float lower, int upper);

/**
 * @brief Retourne un vecteur unitaire aléatoire.
 */
Vec2f randomVector();

template <class T> T max(T a, T b) { return (a > b) ? a : b; }

template <class T> T min(T a, T b) { return (a < b) ? a : b; }

/**
 * @brief Convertit une liste en un array avec la bonne taille.
 * La destruction de l'array (avec delete[]) est laissé à la charge de l'appelant.
 * Tous les éléments sont copiés, donc le type T doit avoir un constructeur vide
 * et un opérateur = valide.
 */
template <class T> T* toArray(const std::list<T>& list)
{
    T*  result = new T[list.size()];
    int i      = 0;
    for (const T& el: list)
    {
        result[i] = el;
        ++i;
    }
    return result;
}

/**
 * @brief Renvoie true si c est un espace, une tabulation ou un retour de ligne.
 */
bool isSpace(char c);

/**
 * @brief Renvoie true si c est un chiffre.
 */
bool isNumeric(char c);

/**
 * @brief Renvoie true si c est un caractère alphabétique, majuscule ou
 * minuscule.
 */
bool isAlpha(char c);

/**
 * @brief Renvoie true si c est un caractère alphabétique minuscule.
 */
bool isLowerAlpha(char c);

/**
 * @brief Renvoie true si c est un caractère alphabétique majuscule.
 */
bool isUpperAlpha(char c);

/**
 * @brief Joue un son uniquement s'il ne vaut pas null.
 */
inline void play(Sound* s)
{
    if (s)
        s->play();
}