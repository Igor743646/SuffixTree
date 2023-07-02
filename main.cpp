#include <iostream>
#include <string>
#include <unordered_map>
#include <chrono>
#include <random>

#include "suffix_tree.h"

using namespace std;

string randString(size_t size) {
    string result;

    for (size_t i = 0; i < size; i++) {
        result += 'a' + rand() % 2;
    }

    return result;
}

int main()
{
    srand(time(nullptr));

    for (double i = 2; i <= 1000000; i *= 1.2) {
        
        string s = randString(static_cast<size_t>(i));
        suffix_tree st(s, '$');
        st.build();

        const auto start = chrono::steady_clock::now();

        st.contains(s.substr(1, s.size() - 1));

        const auto end = chrono::steady_clock::now();

        const chrono::duration<double> elapsed_seconds = end - start;
        cout << s.size() << " " << elapsed_seconds.count() << endl;
        
    }

    return 0;
}