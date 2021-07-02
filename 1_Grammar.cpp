#include <iostream>

using namespace std;

template <int N>
class Grammar {
private:
    string prodRules[N];
    int n; // Number of production rules
public:

    template <int M>
    friend istream& operator >> (istream& in, Grammar < M > &g);
};

template <int N>
istream& operator >> (istream& in, Grammar < N > &g) {
    int n;
    cin >> n;
    g.n = n;
    cin.ignore();
    for (int i = 0; i < n; i++) {
        getline(cin, g.prodRules[i]);
    }
}

int main() {
    Grammar < 100 > g; // 100 means maximum number of production rules
    cin >> g;

    string s;
    cin >> s;
}
