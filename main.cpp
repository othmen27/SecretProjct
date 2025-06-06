#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

void splitRespectingQuotes(const string& s, vector<string>& parts) {
    bool inQuotes = false;
    string current;
    for (char c : s) {
        if (c == '"') {
            inQuotes = !inQuotes;
            current += c;
        } else if (c == ',' && !inQuotes) {
            parts.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    if (!current.empty()) parts.push_back(current);
}

int main() {
    ifstream input("algo.tn");
    ofstream output("translated.cpp");

    string line;
    output << "#include <iostream>\nusing namespace std;\n\n";

    while (getline(input, line)) {
        while (!line.empty() && isspace(line[0])) line.erase(0, 1);
        while (!line.empty() && isspace(line.back())) line.pop_back();

        if (line == "Debut") {
            output << "int main() {\n";
        } else if (line == "Fin") {
            output << "    return 0;\n}\n";
        } else if (line.find("ecrire(") != string::npos) {
            size_t start = line.find("ecrire(") + 7;
            size_t end = line.rfind(")");
            if (end > start) {
                string content = line.substr(start, end - start);
                vector<string> segments;
                splitRespectingQuotes(content, segments);

                output << "    cout";
                for (auto& segment : segments) {
                    while (!segment.empty() && isspace(segment[0])) segment.erase(0, 1);
                    while (!segment.empty() && isspace(segment.back())) segment.pop_back();

                    output << " << " << segment;
                }
                output << " << endl;\n";
            }
        } else if (line.find("lire(") != string::npos) {
            size_t start = line.find("lire(") + 5;
            size_t end = line.rfind(")");
            if (end > start) {
                string content = line.substr(start, end - start);
                output << "    cin >> " << content << ";\n";
            }
        } else if (line.find(":") != string::npos) {
            size_t colon = line.find(":");
            string varName = line.substr(0, colon);
            string type = line.substr(colon + 1);

            while (!varName.empty() && isspace(varName[0])) varName.erase(0, 1);
            while (!varName.empty() && isspace(varName.back())) varName.pop_back();
            while (!type.empty() && isspace(type[0])) type.erase(0, 1);
            while (!type.empty() && isspace(type.back())) type.pop_back();

            if (type == "Entier") {
                output << "    int " << varName << ";\n";
            } else if (type == "Reel") {
                output << "    float " << varName << ";\n";
            } else if (type == "Caractere") {
                output << "    char " << varName << ";\n";
            } else if (type == "Chaine") {
                output << "    string " << varName << ";\n";
            } else if (type == "Booleen") {
                output << "    bool " << varName << ";\n";
            } else {
                output << "    // Unknown type: " << type << "\n";
            }
        } else if (line.find("si") != string::npos && line.find("alors") != string::npos) {
            size_t start = line.find("si") + 2;
            size_t end = line.find("alors");
            if (end > start) {
                string condition = line.substr(start, end - start);
                while (!condition.empty() && isspace(condition[0])) condition.erase(0, 1);
                while (!condition.empty() && isspace(condition.back())) condition.pop_back();
                output << "    if (" << condition << ") {\n";
            }
        } else if (line == "finsi" || line == "fin si") {
            output << "    }\n";
        }
    }

    input.close();
    output.close();
    return 0;
}
