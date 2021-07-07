#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

template <int N>
class Grammar {
private:
    string rawProdRules[N];
    int n; // Number of production rules
    map < string, vector < string > > prodRules;
public:
    void display();
    bool removeNullable(); // Returns true if something modified
    bool removeUnitProd(); // Returns true if something modified
    void removeDuplicate();

    template <int M>
    friend istream& operator >> (istream& in, Grammar < M > &g);
};

template <int N>
void Grammar<N>::display() {
    cout << "\nGrammar Display for Debug Perposes:\n";
    map < string, vector < string > >::iterator it;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        for (int i = 0; i < it -> second.size(); i++) {
            cout << it -> first << " -> " << it -> second[i] << endl;
        }
    }
    cout << endl;
}

template <int N>
bool Grammar<N>::removeNullable() {
    map < string, vector < string > >::iterator it;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        for (int i = 0; i < it -> second.size(); i++) {
            if (it -> second[i] == "#") {
                it -> second.erase(it -> second.begin() + i);

                map < string, vector < string > >::iterator it2;
                for (it2 = prodRules.begin(); it2 != prodRules.end(); it2++) {
                    for (int j = 0; j < it2 -> second.size(); j++) {
                        for (int k = it2 -> second[j].size() - 3; k >= 0; k--) {
                            if (it2 -> second[j].substr(k, 3) == it -> first) {
                                it2 -> second.push_back(it2 -> second[j]);
                                it2 -> second.back().erase(k, 3);
                                if (it2 -> second.back().empty()) {
                                    it2 -> second.back() = "#";
                                }
                                break;
                            }
                        }
                    }
                }
                return true;
            }
        }
    }
    return false;
}

template <int N>
bool Grammar<N>::removeUnitProd() {
    map < string, vector < string > >::iterator it;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        for (int i = it -> second.size() - 1; i >= 0 ; i--) {
            if (it -> second[i].size() == 3 && it -> second[i][0] == '<' && it -> second[i][2] == '>') {
                if (it -> second[i][1] == it -> first[1]) { // Remove without sideefect
                    it -> second.erase(it -> second.begin() + i);
                } else { // Replace rightside of the prudoction with rightside of the removed variable
                    for (int j = 0; j < prodRules[it -> second[i]].size(); j++) {
                        it -> second.push_back(prodRules[it -> second[i]][j]);
                    }
                    it -> second.erase(it -> second.begin() + i);
                }
                return true;
            }
        }
    }
    return false;
}

template <int N>
void Grammar<N>::removeDuplicate() {
    map < string, vector < string > >::iterator it;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {

        vector < string >::iterator it2;
        vector < string >::iterator itEnd = it -> second.end();
        for (it2 = it -> second.begin(); it2 != itEnd; it2++) {
            itEnd = remove(it2 + 1, itEnd, *it2);
        }
        it -> second.erase(itEnd, it -> second.end());
    }
}

template <int N>
istream& operator >> (istream& in, Grammar < N > &g) {
    int n;
    cin >> n;
    g.n = n;
    cin.ignore();
    for (int i = 0; i < n; i++) {
        getline(cin, g.rawProdRules[i]);

        int j = 0;
        string left;
        for (;j < g.rawProdRules[i].size(); j++) {
            if (g.rawProdRules[i][j] == '-' && g.rawProdRules[i][j + 1] == '>') {
                left = g.rawProdRules[i].substr(0, j - 1);
                break;
            }
        } j += 3;

        string right;
        g.rawProdRules[i] += '|';
        for (;j < g.rawProdRules[i].size(); j++) {
            if (g.rawProdRules[i][j] == ' ') {
                continue;
            }
            if (g.rawProdRules[i][j] == '|') {
                g.prodRules[left].push_back(right);
                right.clear();
                continue;
            }
            right += g.rawProdRules[i][j];
        }
    }
}

int main() {
    Grammar < 10000 > g; // 10000 means maximum number of production rules
    cin >> g;

    string s;
    cin >> s;
}
