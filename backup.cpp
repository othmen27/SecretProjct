#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
using namespace std;
std::string toLowerCase(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}
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

string convertLogicOperators(string cond) {
    size_t pos;
    while ((pos = cond.find(" et ")) != string::npos) cond.replace(pos, 4, " && ");
    while ((pos = cond.find(" ou ")) != string::npos) cond.replace(pos, 4, " || ");
    while ((pos = cond.find("non ")) != string::npos) cond.replace(pos, 4, "!");
    return cond;
}

string trim(string str) {
    
    while (!str.empty() && isspace(str.front())) str.erase(str.begin());
    while (!str.empty() && isspace(str.back())) str.pop_back();
    return str;
}

string translateType(string type) {
    std::string lower = toLowerCase(type);
    if (lower == "entier") return "int";
    if (lower == "reel") return "float";
    if (lower == "booleen") return "bool";
    if (lower == "chaine") return "string";
    if (lower == "caractere") return "char";
    return type;
}

int main() {
    ifstream input("algo.tn");
    ofstream output("translated.cpp");

    string line;
    output << "#include <iostream>\n#include <cmath>\n#include <algorithm>\n#include <string>\nusing namespace std;\n\n";
    int openBlocks = 0;
    bool inMain = false;

    while (getline(input, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line == "Debut") {
            output << "int main() {\n";
            inMain = true;
        } else if (line == "Fin") {
            while (openBlocks-- > 0) output << "    }\n";
            if (inMain) output << "    return 0;\n}\n";
        } else if (line.find("ecrire(") == 0) {
            size_t start = 7, end = line.rfind(")");
            string content = line.substr(start, end - start);
            vector<string> segments;
            splitRespectingQuotes(content, segments);
            output << "    cout";
            for (auto& segment : segments)
                output << " << " << trim(segment);
            output << " << endl;\n";
        } else if (line.find("lire(") == 0) {
            size_t start = 5, end = line.rfind(")");
            output << "    cin >> " << trim(line.substr(start, end - start)) << ";\n";
        } else if (line.find(":") != string::npos && line.find("fonction") != 0 && line.find("procedure") != 0) {
            size_t colon = line.find(":");
            string varName = trim(line.substr(0, colon));
            string type = trim(line.substr(colon + 1));
            output << "    " << translateType(type) << " " << varName << ";\n";
        } else if (line.find("si") == 0 && line.find("alors") != string::npos) {
            string condition = trim(line.substr(2, line.find("alors") - 2));
            output << "    if (" << convertLogicOperators(condition) << ") {\n";
            openBlocks++;
        } else if (line == "sinon") {
            output << "    } else {\n";
        } else if (line == "finsi" || line == "fin si") {
            output << "    }\n";
            if (openBlocks > 0) openBlocks--;
        } else if (line.find("tantque") == 0 || line.find("tq") == 0) {
            size_t start = (line.find("tantque") == 0) ? 7 : 2;
            size_t end = line.find("faire");
            string condition = trim(line.substr(start, end - start));
            output << "    while (" << convertLogicOperators(condition) << ") {\n";
            openBlocks++;
        } else if (line.find("fintq") != string::npos || line.find("fin tantque") != string::npos) {
            output << "    }\n";
            if (openBlocks > 0) openBlocks--;
        } else if (line.find("<-") != string::npos) {
            size_t pos = line.find("<-");
            output << "    " << trim(line.substr(0, pos)) << " = " << trim(line.substr(pos + 2)) << ";\n";
        } else if (line.find("pour") == 0) {
            size_t dePos = line.find("de"), aPos = line.find("a"), fairePos = line.find("faire");
            string var = trim(line.substr(4, dePos - 4));
            string startVal = trim(line.substr(dePos + 2, aPos - dePos - 2));
            string endVal = trim(line.substr(aPos + 1, fairePos - aPos - 1));
            output << "    for (int " << var << " = " << startVal << "; " << var << " <= " << endVal << "; " << var << "++) {\n";
            openBlocks++;
        } else if (line == "finpour") {
            output << "    }\n";
            if (openBlocks > 0) openBlocks--;
        } else if (line == "repeter") {
            output << "    do {\n";
            openBlocks++;
        } else if (line.find("jusqua") != string::npos || line.find("jusqu'a") != string::npos) {
            string condition = trim(line.substr(line.find("jusqu") + 6));
            output << "    } while (!(" << convertLogicOperators(condition) << "));\n";
            if (openBlocks > 0) openBlocks--;
        } else if (line.find("fonction") == 0) {
            size_t nameStart = line.find(" ") + 1;
            size_t parenStart = line.find("(", nameStart), parenEnd = line.find(")", parenStart);
            size_t returnColon = line.find(":", parenEnd);
            string functionName = trim(line.substr(nameStart, parenStart - nameStart));
            string paramList = trim(line.substr(parenStart + 1, parenEnd - parenStart - 1));
            string returnType = translateType(trim(line.substr(returnColon + 1)));
            vector<string> params;
            splitRespectingQuotes(paramList, params);
            string cppParams;
            for (size_t i = 0; i < params.size(); ++i) {
                size_t colon = params[i].find(":");
                string var = trim(params[i].substr(0, colon));
                string type = translateType(trim(params[i].substr(colon + 1)));
                if (i > 0) cppParams += ", ";
                cppParams += type + " " + var;
            }
            output << returnType << " " << functionName << "(" << cppParams << ") {\n";
            openBlocks++;
        } else if (line.find("procedure") == 0) {
            size_t nameStart = line.find(" ") + 1;
            size_t parenStart = line.find("(", nameStart), parenEnd = line.find(")", parenStart);
            string procName = trim(line.substr(nameStart, parenStart - nameStart));
            string paramList = trim(line.substr(parenStart + 1, parenEnd - parenStart - 1));
            vector<string> params;
            splitRespectingQuotes(paramList, params);
            string cppParams;
            for (size_t i = 0; i < params.size(); ++i) {
                size_t colon = params[i].find(":");
                string var = trim(params[i].substr(0, colon));
                string type = translateType(trim(params[i].substr(colon + 1)));
                if (i > 0) cppParams += ", ";
                cppParams += type + " " + var;
            }
            output << "void " << procName << "(" << cppParams << ") {\n";
            openBlocks++;
        } else if (line == "finfonction" || line == "fin procedure" || line == "finprocedure") {
            output << "}\n";
            if (openBlocks > 0) openBlocks--;
        } else if (line == "retourne vrai") {
            output << "    return true;\n";
        } else if (line == "retourne faux") {
            output << "    return false;\n";
        } else if (line.find("retourne") == 0) {
            output << "    return " << trim(line.substr(8)) << ";\n";
        } else {
            output << "    // " << line << "\n";
        }
    }

    input.close();
    output.close();
    return 0;
}
