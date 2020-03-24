#include "Guard.h"

Sound* Guard::damage_hit = new Sound("sons/roblox_hit_damage.wav");
Sound* Guard::heal_sound = new Sound("sons/heal_sound.wav");

const std::vector<const char*> Guard::modeles({
    "drfreak",
    "Marvin",
    "Potator",
    "garde",
    "Droid", 
    "Lezard",
    "Samourai",
    "Serpent",
    "Squelette",
    "Blade"
});

Guard::Guard(Labyrinthe* l, const char* modele, uint id): Character(120, 80, l, modele, id) {
    m_damage_hit = damage_hit;
    m_heal_sound = heal_sound;
}

Guard::Guard(Labyrinthe* l, int modele, uint id): Character(120, 80, l, modeles.at(modele), id) {
    m_damage_hit = damage_hit;
    m_heal_sound = heal_sound;
}