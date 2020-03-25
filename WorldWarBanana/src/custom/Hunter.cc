#include "Hunter.h"

#include <string.h>

#include "Labyrinthe.h"

Sound* Hunter::_hunter_fire = new Sound("sons/hunter_fire.wav");
Sound* Hunter::_hunter_hit  = new Sound("sons/hunter_hit.wav");
Sound* Hunter::_wall_hit = new Sound("sons/hit_wall.wav");
Sound* Hunter::heal_sound = new Sound("sons/heal_sound.wav");

/*
 *	Constructeur.
 */

Hunter::Hunter(Labyrinthe* l, uint id):
    Character(100, 80, l, 0, id)
{
    m_damage_hit = _hunter_hit;
    m_heal_sound = heal_sound;
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les
 *collisions sp�cifiques...)
 */

bool Hunter::process_fireball(float dx, float dy)
{
    // calculer la distance entre le chasseur et le lieu de l'explosion.
    float x     = (_x - _fb->get_x()) / Environnement::scale;
    float y     = (_y - _fb->get_y()) / Environnement::scale;
    float dist2 = x * x + y * y;
    // on bouge que dans le vide!
    Vec2i fbp = m_laby->realToGrid(_fb->get_x() + dx, _fb->get_y() + dy);
    if (m_laby->canGoTo(this, fbp.x, fbp.y))
    {
        message("Woooshh ..... %d", (int) dist2);
        // il y a la place.
        return true;
    }
    // collision...
    // calculer la distance maximum en ligne droite.
    float dmax2 = (_l->width()) * (_l->width()) + (_l->height()) * (_l->height());
    // faire exploser la boule de feu avec un bruit fonction de la distance.
    _wall_hit->play(1. - dist2 / dmax2);
    message("Booom...");
    // teste si on a touch� le tr�sor: juste pour montrer un exemple de la
    // fonction � partie_terminee �.
    if ((int) ((_fb->get_x() + dx) / Environnement::scale) == _l->_treasor._x &&
        (int) ((_fb->get_y() + dy) / Environnement::scale) == _l->_treasor._y)
    {
        partie_terminee(true);
    }
    return false;
}

/*
 *	Tire sur un ennemi.
 */

void Hunter::fire(int angle_vertical)
{
    message("Woooshh...");
    _hunter_fire->play();
    _fb->init(/* position initiale de la boule */ _x, _y, 10.,
              /* angles de vis�e */ angle_vertical, _angle);
}

/*
 *	Clic droit: par d�faut fait tomber le premier gardien.
 *
 *	Inutile dans le vrai jeu, mais c'est juste pour montrer
 *	une utilisation des fonctions � tomber � et � rester_au_sol �
 */

void Hunter::right_click(bool shift, bool control)
{
    if (shift)
        _l->_guards[1]->rester_au_sol();
    else
        _l->_guards[1]->tomber();
}

void Hunter::die(CMover* m)
{
    partie_terminee(false);
}
