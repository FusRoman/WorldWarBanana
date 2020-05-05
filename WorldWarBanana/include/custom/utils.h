#pragma once

#include <iostream>
#include <list>
#include <math.h>
#include <utility>

#include "Sound.h"
#include "macros.h"
#include "Vec2.h"

/**************************************************************************************************
 *
 * Fonctions diverses
 *
 *************************************************************************************************/

typedef unsigned int uint;

template <class T> T max(T a, T b) { return (a > b) ? a : b; }

template <class T> T min(T a, T b) { return (a < b) ? a : b; }


/**************************************************************************************************
 *
 * Fonctions sur les arrays
 *
 *************************************************************************************************/

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
 * @brief Alloue un tableau 2D d'éléments T.
 * Le type T doit avoir un constructeur par défaut.
 */
template <class T> T** create2DArray(uint width, uint height, const T& _default)
{
    T** result = new T*[height];
    for (uint y = 0; y < height; ++y)
    {
        T* row = new T[width];
        for (uint x = 0; x < width; ++x)
        {
            row[x] = _default;
        }
        result[y] = row;
    }
    return result;
}

/**
 * @brief Libère un array 2D.
 */
template <class T> void free2DArray(T** array, uint height)
{
    for (uint y = 0; y < height; ++y)
    {
        delete[] array[y];
    }
    delete[] array;
}

/**
 * @brief Renvoie une copie d'array avec un élément en moins.
 * La taille de l'array renvoyé est naturellement size - 1.
 * L'ancien array n'est pas supprimé, c'est donc à l'appeleur de le faire si nécessaire.
 */
template <class T> T* removeFromArray(T* array, size_t size, uint index)
{
    if (size == 0)
    {
        ERROR("removeFromArray: invalid size " << (int) size);
    }
    if (index >= size)
    {
        ERROR("removeFromArray: invalid index " << (int) index << " (size: " << (int) size << ")");
    }

    T* result = new T[size - 1];
    for (uint i = 0; i < index; ++i)
    {
        result[i] = array[i];
    }
    for (uint i = index + 1; i < size; ++i)
    {
        result[i - 1] = array[i]; 
    }
    return result;
}

/**************************************************************************************************
 *
 * Fonctions aléatoires
 *
 *************************************************************************************************/

/**
 * @brief Retourne un entier dans [lower, upper].
 */
int randomInt(int lower, int upper);

/**
 * @brief Retourne un flottant dans [low, upper[.
 */
float randomFloat(float lower, float upper);

/**
 * @brief Retourne un vecteur unitaire aléatoire, et son angle en radians.
 */
std::pair<Vec2f, float> randomVector();

/**************************************************************************************************
 *
 * Fonctions sur les angles
 *
 *************************************************************************************************/

const float pi = 3.14159265359;

/**
 * @brief Convertit des radians en degrés.
 */
int radiansToDegrees(float radians);

/**
 * @brief Convertit des degrés en radians.
 */
float degreesToRadians(int degrees);

/**
 * @brief Renvoie le vecteur unitaire dans l'angle donné.
 */
Vec2f unitVector(float radians);

/**************************************************************************************************
 *
 * Fonctions sur les caractères
 *
 *************************************************************************************************/

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

/**************************************************************************************************
 *
 * Fonction sur les sons
 *
 *************************************************************************************************/

/**
 * @brief Joue un son uniquement s'il ne vaut pas null.
 */
inline void play(Sound* s, float intensity = 1.)
{
    if (s)
    {
        s->play(intensity);
    }
}