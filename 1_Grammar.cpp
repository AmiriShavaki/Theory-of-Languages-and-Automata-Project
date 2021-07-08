#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>

using namespace std;

template <int N>
class Grammar {
private:
    string rawProdRules[N];
    int n; // Number of production rules
    map < string, vector < string > > prodRules;
    string stV;
public:
    void display();
    bool removeNullable(); // Returns true if something modified
    bool removeUnitProd(); // Returns true if something modified
    bool removeUnreachable();
    bool removeUnterminatable();
    void removeDuplicate();
    void removeInvalidProdRules(); // Remove prodoction rules with at least one undefined variable
    void simplify();
    const string CYKMembership(const string& w);

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
bool Grammar<N>::removeUnreachable() {
    queue < string > q;
    set < string > visited;
    q.push(stV);
    visited.insert(stV);
    while (!q.empty()) {
        string top = q.front();
        q.pop();
        for (int i = 0; i < prodRules[top].size(); i++) {
            if (prodRules[top][i].size() < 3) continue;
            for (int j = 0; j < prodRules[top][i].size() - 2; j++) {
                if (prodRules[top][i][j] == '<' && prodRules[top][i][j + 2] == '>') {
                    if (visited.find(prodRules[top][i].substr(j, 3)) == visited.end()) {
                        visited.insert(prodRules[top][i].substr(j, 3));
                        q.push(prodRules[top][i].substr(j, 3));
                    }
                }
            }

        }
    }
    map < string, vector < string > >::iterator it;
    int i = 0;
    vector < string > toDel;
    bool res = false;
    for (it = prodRules.begin(); it != prodRules.end(); it++, i++) {
        if (visited.find(it -> first) == visited.end()) {
            toDel.push_back(it -> first);
            res = true;
        }
    }
    for (int i = 0; i < toDel.size(); i++) {
        prodRules.erase(prodRules.find(toDel[i]));
    }
    return res;
}

template <int N>
bool Grammar<N>::removeUnterminatable() {
    bool flg = true;
    bool res = false;
    set < string > terminatable;
    while (flg) {
        flg = false;
        map < string, vector < string > >::iterator it;
        for (it = prodRules.begin(); it != prodRules.end(); it++) {
            if (terminatable.find(it -> first) != terminatable.end()) {
                continue;
            }
            for (int i = 0; i < it -> second.size(); i++) {

                if (it -> second[i].size() < 3) {
                    terminatable.insert(it -> first);
                    flg = true;
                    break;
                }
                bool isTerminatable = true;
                for (int j = 0; j < it -> second[i].size() - 2; j++) {
                    if (it -> second[i][j] == '<' && it -> second[i][j + 2] == '>') {
                        if (terminatable.find(it -> second[i].substr(j, 3)) == terminatable.end()) {
                            isTerminatable = false;
                        }
                    }
                }
                if (isTerminatable) {
                    terminatable.insert(it -> first);
                    flg = true;
                    break;
                }

            }
        }
    }
    map < string, vector < string > >::iterator it;
    vector < string > toDel;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        if (terminatable.find(it -> first) == terminatable.end()) {
            toDel.push_back(it -> first);
            res = true;
        }
    }
    for (int i = 0; i < toDel.size(); i++) {
        prodRules.erase(prodRules.find(toDel[i]));
    }
    return res;
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
void Grammar<N>::removeInvalidProdRules() {
    set < string > validVars;
    map < string, vector < string > >::iterator it;
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        if (it -> second.size() > 0) {
            validVars.insert(it -> first);
        }
    }
    for (it = prodRules.begin(); it != prodRules.end(); it++) {
        for (int i = it -> second.size() - 1; i >= 0; i--) {

            if (it -> second[i].size() < 3) {
                continue;
            }
            for (int j = 0; j < it -> second[i].size() - 2; j++) {
                if (it -> second[i][j] == '<' && it -> second[i][j + 2] == '>') {
                    if (validVars.find(it -> second[i].substr(j, 3)) == validVars.end()) {
                        it -> second.erase(it -> second.begin() + i);
                        break;
                    }
                }
            }

        }
    }
}

template <int N>
void Grammar<N>::simplify() {
    bool flg = true;
    while (flg) {
        flg = false;
        if (this -> removeNullable() || this -> removeUnitProd() || this -> removeUnreachable() || this -> removeUnterminatable()) {
            flg = true;
            this -> removeDuplicate();
            this -> removeInvalidProdRules();
            continue;
        }
    }
}

template <int N>
const string Grammar<N>::CYKMembership(const string& w) {

    // Construct the table
    set < string >** dp = new set < string >* [w.size()];
    for (int i = 0; i < w.size(); i++) {
        dp[i] = new set < string > [w.size()];
    }

    // Fill the table
    for (int l = 1; l <= w.size(); l++) { // l = length of the range
        for (int i = 0; i < 1 + w.size() - l; i++) { // range [i, i + l - 1]
            int j = i + l - 1; // rightside position

            // Base case: rightside includes of just terminals
            map < string, vector < string > >::iterator it;
            for (it = prodRules.begin(); it != prodRules.end(); it++) {
                for (int k = 0; k < it -> second.size(); k++) {
                    if (it -> second[k] == w.substr(i, l)) {
                        dp[i][j].insert(it -> first);
                        cout << "Found a terminate! " << "i:" << i << " j:" << j << " var:" << it -> first << endl;
                    }
                }
            }

            for (int k = i; k < j; k++) { // dp[i][k], dp[k + 1][j]
                cout << "range i:" << i << " k:" << k << " j:" << j << endl;
                set < string > targetProds;
                set < string >::iterator it2;
                set < string >::iterator it3;
                for (it2 = dp[i][k].begin(); it2 != dp[i][k].end(); it2++) {
                    for (it3 = dp[k + 1][j].begin(); it3 != dp[k + 1][j].end(); it3++) {
                        string target1 = *it2;
                        target1 += *it3;
                        string target2 = *it2;
                        target2 += w.substr(k + 1, j - k);
                        string target3 = w.substr(i, k - i + 1);
                        target3 += *it3;
                        cout << " i,j:" << i << ' ' << j << " target1: " << target1 << " target2: " << target2 << " target3: " << target3 << endl;
                        targetProds.insert(target1);
                        targetProds.insert(target2);
                        targetProds.insert(target3);
                    }
                }

                // In the search of target productions among available production rules
                map < string, vector < string > >::iterator it4;
                for (it4 = prodRules.begin(); it4 != prodRules.end(); it4++) {
                    for (int p = 0; p < it4 -> second.size(); p++) {
                        if (targetProds.find(it4 -> second[p]) != targetProds.end()) {
                            dp[i][j].insert(it4 -> first);
                            cout << "target acceptable found: " << "i:" << i << " j:" << j << " var:" << it4 -> first << endl;
                        }
                    }
                }

            }
        }
    }

    return dp[0][w.size() - 1].find(stV) != dp[0][w.size() - 1].end() ? "Accepted" : "Rejected" ;
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
    g.stV = g.rawProdRules[0].substr(0, 3);
}

int main() {
    Grammar < 10000 > g; // 10000 means maximum number of production rules
    cin >> g;
    g.simplify();
    g.display();

    string s;
    cin >> s;
    cout << g.CYKMembership(s);
}
