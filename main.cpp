#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
using namespace std;

string toLowerCase(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c){ return tolower(c); });
    return result;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}
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
void splitrespectingquotespoint(const string& s, vector<string>& parts){
    bool inQuotes = false;
    string current;
    for (char c : s) {
        if (c == '"') {
            inQuotes = !inQuotes;
            current +=c;
            }
        else if (c == ':' && !inQuotes) {
                parts.push_back(current);
                current.clear();}
        else{
                current+=c;
            }
        }
    if(!current.empty()) parts.push_back(current);
}
string makevariable(string l) {
    string result;
    vector<string> types = {"entier", "reel", "booleen", "chaine", "caractere"};
    vector<string> parts;
    splitrespectingquotespoint(l,parts);
    parts[1]=trim(parts[1]);
    for (const string& t : types) {
        if (parts[1]==t) {
            string rest = parts[0];
            string type = translateType(t);
            return type + " " + rest;
        }
    }
    return "";
}
void translateLine(const string& line, ostream& output, int& openBlocks) {
    string l = trim(line);
    if (l.empty() || l[0] == '#') return;
    if(line.find("ecrire(")==0){
        size_t start =7;
        size_t end = line.rfind(")");
        if(end==string::npos || end <= start) return;
        string content= line.substr(start,end-start);
        vector<string> segments;
        splitRespectingQuotes(content,segments);
        output << "cout";
        for(auto& seg : segments)
            output << " <<" << trim(seg);
        output << " << endl;\n";
        return;
    }
    if(line.find("lire(")==0){
        size_t start = 5, end = line.rfind(")");
        output << "cin >> " << trim(line.substr(start, end - start)) << ";\n";
        return;
    }
    if(line.find("<-") !=string::npos){
        size_t pos = line.find("<-");
        output << trim(line.substr(0, pos)) << " = " << trim(line.substr(pos + 2)) << ";\n";
        return;
    }
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
    for (const string& t : {"entier", "reel", "booleen", "chaine", "caractere"}) {
        if (l.find(t) == 0 && (l.size() == t.size() || isspace(l[t.size()]))) {
            string rest = trim(l.substr(t.size()));
            if (rest.empty()) return;
            size_t initPos = rest.find("<-");
            if (initPos != string::npos) {
                string varName = trim(rest.substr(0, initPos));
                string val = trim(rest.substr(initPos + 2));
                output << translateType(t) << " " << varName << " = " << val << ";\n";
                return;
            } else {
                vector<string> vars;
                splitVars(rest, vars);
                for (auto& v : vars) {
                    output << translateType(t) << " " << v << ";\n";
                }
                return;
            }
        }
    }
    if (l.find("procedure ") == 0) {
        string procName = l.substr(10);
        procName = procName.substr(0, procName.find('('));
        size_t openParen = l.find('(');
        size_t closeParen = l.find(')', openParen);
        string params = l.substr(openParen, closeParen - openParen);
        params = params.substr(1, params.find(')')-1);
        vector<string> parts;
        splitRespectingQuotes(params,parts);
        int length = parts.size();
        string paramStr = "";
        for (size_t i = 0; i < length; i++){
            string cppVar = makevariable(parts[i]);
            cppVar = trim(cppVar);
            if (!cppVar.empty() && cppVar.back() == ';') {
                cppVar.pop_back();
            }
            if(!paramStr.empty()) paramStr += ", ";
            paramStr += cppVar;
            if(i>1){
                paramStr+=", ";
            }
        }
        output << "void " << procName << "("<<paramStr<<") {\n";
        openBlocks++;
        return;
    }
    if (l.find("fonction ") == 0) {
        string procName = l.substr(9);
        procName = procName.substr(0, procName.find('('));
        size_t openParen = l.find('(');
        size_t closeParen = l.find(')', openParen);
        string params = l.substr(openParen, closeParen - openParen);
        params = params.substr(1, params.find(')')-1);
        vector<string> parts;
        splitRespectingQuotes(params,parts);
        int length = parts.size();
        string paramStr = "";
        for (size_t i = 0; i < length; i++){
            string cppVar = makevariable(parts[i]);
            cppVar = trim(cppVar);
            if (!cppVar.empty() && cppVar.back() == ';') {
                cppVar.pop_back();
            }
            if(!paramStr.empty()) paramStr += ", ";
            paramStr += cppVar;
            if(i>1){
                paramStr+=", ";
            }
        }
        output << "auto " << procName << "("<<paramStr<<") {\n";
        openBlocks++;
        return;
    }
    if (l == "finprocedure" || l == "finfonction") {
        output << "}\n";
        if (openBlocks > 0) openBlocks--;
        return;
    }
    if (l.find("(") != string::npos && l.back() == ')') {
        output << l << ";\n";
        return;
    }
    size_t assignPos = l.find("<-");
    if (assignPos != string::npos) {
        string left = trim(l.substr(0, assignPos));
        string right = trim(l.substr(assignPos + 2));
        output << left << " = " << right << ";\n";
        return;
    }
    if (line == "retourne vrai") {
            output << "    return true;\n";}
    else if (line == "retourne faux") {
            output << "    return false;\n";}
    else if (line.find("retourne") == 0) {
            output << "    return " << trim(line.substr(8)) << ";\n";}
    else{
        output << "// " << l << "\n";
    }
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
    while (openBlocks-- > 0) output << "}\n";
    input.close();
    output.close();
    cout << "Translation finished.\n";
    return 0;
}
