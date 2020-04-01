#include "CMover.h"
#include "Labyrinthe.h"
#include "utils.h"

uint CMover::m_tick = 0;

CMover::CMover(int x, int y, Labyrinthe* l, const char* modele, uint id):
    Mover(x, y, l, modele), m_laby(l), m_id(id)
{
}

bool CMover::move(double dx, double dy) { return m_laby->move(this, dx, dy); }

void CMover::update()
{
    // La méthode update du joueur n'est jamais appelée
    // Donc on ne peut pas s'appuyer dessus pour le compteur d'updates,
    // et on ne peut pas compter sur le fait que chaque objet mettra son tick à jour
    // De plus, Labyrinthe n'a aucune méthode garantie d'être appelée une et une seule fois par
    // update Donc on désigne arbitrairement le CMover d'ID 1 le rôle de mettre à jour l'attribut
    // statique... Ce qui signifie qu'on devra toujours avoir au moins un CMover en plus du joueur
    // (si seulement la fonction update du joueur était appelée !)
    if (m_id == 1)
    {
        ++m_tick;
    }
}
