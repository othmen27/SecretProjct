#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
using namespace std;

void afficherMessage() {
ecrire("Message : ", msg);
}
auto estMajeur() {
    if (a >= 18 && !(a > 120)) {
// retourne vrai
    } else {
// retourne faux
    }
}
auto calculerMoyenne() {
// retourne (n1 + n2) / 2.0
}
int main() {
int age;
int note1;
int note2;
float moyenne;
bool majeur;
ecrire("Donner l'âge : ");
lire(age);
majeur <- estMajeur(age);
    if (majeur) {
afficherMessage("Vous êtes majeur.");
    } else {
afficherMessage("Vous êtes mineur.");
    }
ecrire("Entrer la note 1 : ");
lire(note1);
ecrire("Entrer la note 2 : ");
lire(note2);
moyenne <- calculerMoyenne(note1, note2);
ecrire("Moyenne est : ", moyenne);
    if (majeur && (moyenne >= 10 || moyenne == 9)) {
afficherMessage("Admis");
    } else {
afficherMessage("Non admis");
    }
int i;
int somme = 0;
    for (int i = 1; i <= 10; i++) {
somme = somme + i;
    }
ecrire("Somme de 1 à 10 est : ", somme);
    do {
ecrire("Tapez un nombre négatif : ");
lire(i);
    } while (!(i < 0));
    return 0;
}
