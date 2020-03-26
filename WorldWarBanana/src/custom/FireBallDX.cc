#include "FireBallDX.h"
#include "Labyrinthe.h"

/*bool process_fireball(CMover* owner, FireBall* fb, float dx, float dy)
{
    // calculer la distance entre le chasseur et le lieu de l'explosion.
    Labyrinthe* laby = owner->getMaze();
    //Vec2i p = laby->realToGrid(owner->_x - fb->get_x(), owner->_y - fb->get_y());
    //float dist2 = p.x * p.x + p.y * p.y;

    // on bouge que dans le vide!
    Vec2i fbp = laby->realToGrid(fb->get_x() + dx, fb->get_y() + dy);
    if (laby->canGoTo(owner, fbp.x, fbp.y))
    {
        return true;
    }
    // collision...
    // calculer la distance maximum en ligne droite.
    //float dmax2 = (laby->width()) * (laby->width()) + (laby->height()) * (laby->height());
    // faire exploser la boule de feu avec un bruit fonction de la distance.
    //_wall_hit->play(1. - max(dist2 / dmax2);

    // teste si on a touch� le tr�sor: juste pour montrer un exemple de la
    // fonction � partie_terminee �.
    if (owner->id() == 0 && fbp.x == laby->_treasor._x && fbp.y == laby->_treasor._y)
    {
        partie_terminee(true);
    }
    return false;
}*/
