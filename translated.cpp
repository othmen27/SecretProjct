#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
using namespace std;

void afficherMessage(string msg) {
cout <<"Message : " <<msg << endl;
}
auto estMajeur(int a) {
    if (a >= 18 && !(a > 120)) {
    return true;
    } else {
    return false;
    }
}
auto calculerMoyenne(int n1, int  n2) {
    return (n1 + n2) / 2.0;
}
int main() {
int age;
int note1;
int note2;
float moyenne;
bool majeur;
cout <<"Donner l'âge : " << endl;
cin >> age;
majeur = estMajeur(age);
    if (majeur) {
afficherMessage("Vous êtes majeur.");
    } else {
afficherMessage("Vous êtes mineur.");
    }
cout <<"Entrer la note 1 : " << endl;
cin >> note1;
cout <<"Entrer la note 2 : " << endl;
cin >> note2;
moyenne = calculerMoyenne(note1, note2);
cout <<"Moyenne est : " <<moyenne << endl;
    if (majeur && (moyenne >= 10 || moyenne == 9)) {
afficherMessage("Admis");
    } else {
afficherMessage("Non admis");
    }
int i;
int somme;
somme = 0;
    for (int i = 1; i <= 10; i++) {
somme = somme + i;
    }
cout <<"Somme de 1 à 10 est : " <<somme << endl;
    do {
cout <<"Tapez un nombre négatif : " << endl;
cin >> i;
    } while (!(i < 0));
    return 0;
}
