#include <iostream>
using namespace std;

int main() {
    int x;
    string nom;
    cout << "Donnez votre nom:" << endl;
    cin >> nom;
    cin >> x;
    if (x>5) {
    cout << "Bonjour, " << nom << " ! Vous avez " << x << " ans." << endl;
    }
    return 0;
}
