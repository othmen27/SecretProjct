#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
using namespace std;

// Convert string to lowercase
string toLowerCase(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c){ return tolower(c); });
    return result;
}

// Trim whitespace from both ends
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

// Split variables by commas, ignoring commas inside quotes (simple)
void splitVars(const string& str, vector<string>& vars) {
    string current;
    bool inQuotes = false;
    for (char c : str) {
        if (c == '"') inQuotes = !inQuotes;
        if (c == ',' && !inQuotes) {
            vars.push_back(trim(current));
            current.clear();
        } else {
            current += c;
        }
    }
    if (!current.empty())
        vars.push_back(trim(current));
}

string translateType(const string& type) {
    string lower = toLowerCase(type);
    if (lower == "entier") return "int";
    if (lower == "reel") return "float";
    if (lower == "booleen") return "bool";
    if (lower == "chaine") return "string";
    if (lower == "caractere") return "char";
    return "auto"; // fallback
}

// Translate logic operators in condition
string convertLogicOperators(string cond) {
    size_t pos;
    while ((pos = cond.find(" et ")) != string::npos) cond.replace(pos, 4, " && ");
    while ((pos = cond.find(" ou ")) != string::npos) cond.replace(pos, 4, " || ");
    while ((pos = cond.find("non ")) != string::npos) cond.replace(pos, 4, "!");
    return cond;
}

// Split respecting quotes, for function args, ecrire, etc.
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

// Translate a single line and output C++ code to 'output'
void translateLine(const string& line, ostream& output, int& openBlocks) {
    string l = trim(line);
    if (l.empty() || l[0] == '#') return;

    // Control structures:
    if (l.find("si") == 0 && l.find("alors") != string::npos) {
        string condition = trim(l.substr(2, l.find("alors") - 2));
        output << "    if (" << convertLogicOperators(condition) << ") {\n";
        openBlocks++;
        return;
    }
    if (l == "sinon") {
        output << "    } else {\n";
        return;
    }
    if (l == "finsi" || l == "fin si") {
        output << "    }\n";
        if (openBlocks > 0) openBlocks--;
        return;
    }
    if (l.find("pour") == 0) {
        // for var de start a end faire
        size_t dePos = l.find("de");
        size_t aPos = l.find("a");
        size_t fairePos = l.find("faire");
        if (dePos != string::npos && aPos != string::npos && fairePos != string::npos) {
            string var = trim(l.substr(4, dePos - 4));
            string startVal = trim(l.substr(dePos + 2, aPos - dePos - 2));
            string endVal = trim(l.substr(aPos + 1, fairePos - aPos - 1));
            output << "    for (int " << var << " = " << startVal << "; " << var << " <= " << endVal << "; " << var << "++) {\n";
            openBlocks++;
            return;
        }
    }
    if (l == "finpour") {
        output << "    }\n";
        if (openBlocks > 0) openBlocks--;
        return;
    }
    if (l.find("tantque") == 0 || l.find("tq") == 0) {
        size_t start = (l.find("tantque") == 0) ? 7 : 2;
        size_t end = l.find("faire");
        if (end != string::npos) {
            string condition = trim(l.substr(start, end - start));
            output << "    while (" << convertLogicOperators(condition) << ") {\n";
            openBlocks++;
            return;
        }
    }
    if (l == "fintq" || l == "fin tantque") {
        output << "    }\n";
        if (openBlocks > 0) openBlocks--;
        return;
    }
    if (l == "repeter") {
        output << "    do {\n";
        openBlocks++;
        return;
    }
    if (l.find("jusqua") != string::npos || l.find("jusqu'a") != string::npos) {
        size_t pos = l.find("jusqu");
        string condition = trim(l.substr(pos + 6));
        output << "    } while (!(" << convertLogicOperators(condition) << "));\n";
        if (openBlocks > 0) openBlocks--;
        return;
    }

    // Variable declarations and initializations
    for (const string& t : {"entier", "reel", "booleen", "chaine", "caractere"}) {
        if (l.find(t) == 0 && (l.size() == t.size() || isspace(l[t.size()]))) {
            string rest = trim(l.substr(t.size()));
            if (rest.empty()) return;

            size_t initPos = rest.find("<-");
            if (initPos != string::npos) {
                // Declaration + initialization (only one var supported)
                string varName = trim(rest.substr(0, initPos));
                string val = trim(rest.substr(initPos + 2));
                output << translateType(t) << " " << varName << " = " << val << ";\n";
                return;
            } else {
                // Multiple declarations without initialization
                vector<string> vars;
                splitVars(rest, vars);
                for (auto& v : vars) {
                    output << translateType(t) << " " << v << ";\n";
                }
                return;
            }
        }
    }

    // Procedure declaration
    if (l.find("procedure ") == 0) {
        string procName = l.substr(10);
        procName = procName.substr(0, procName.find('('));
        output << "void " << procName << "() {\n";
        openBlocks++;
        return;
    }
    // Function declaration
    if (l.find("fonction ") == 0) {
        string funcName = l.substr(9);
        funcName = funcName.substr(0, funcName.find('('));
        output << "auto " << funcName << "() {\n";
        openBlocks++;
        return;
    }
    // End of procedure/function
    if (l == "finprocedure" || l == "finfonction") {
        output << "}\n";
        if (openBlocks > 0) openBlocks--;
        return;
    }

    // Function/procedure call (roughly)
    if (l.find("(") != string::npos && l.back() == ')') {
        output << l << ";\n";
        return;
    }

    // Assignments with <- 
    size_t assignPos = l.find("<-");
    if (assignPos != string::npos) {
        string left = trim(l.substr(0, assignPos));
        string right = trim(l.substr(assignPos + 2));
        output << left << " = " << right << ";\n";
        return;
    }

    // Default: output as comment to avoid losing any line
    output << "// " << l << "\n";
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

        // Handle main start
        if (line == "Debut") {
            output << "int main() {\n";
            openBlocks++;
            inMain = true;
            continue;
        }
        if (line == "Fin") {
            output << "    return 0;\n}\n";
            openBlocks--;
            inMain = false;
            continue;
        }

        translateLine(line, output, openBlocks);
    }

    // Close any still-open blocks just in case
    while (openBlocks-- > 0) output << "}\n";

    input.close();
    output.close();
    cout << "Translation finished.\n";
    return 0;
}
