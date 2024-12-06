#include <iostream>
#include <vector>
#include <fstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

// User inputs declared as global variables

struct cacheLine {
    bool VB = false; // Valid bit (false: invalid, true: valid)
    int tag = -1;    // Tag (initialized to -1 for unused cache line)
};

// Memory Inputs
int memoryBits; // Memory address bits
int memAT;      // Memory access time

// Cache Inputs
int S;          // Cache size in bytes
int L;          // Cache line size in bytes
int cacheAT;    // Cache access time

// Program Outputs
float hitRatio;
float missRatio;
float AMAT;      // Average Memory Access Time
float SMAT = 0;  // Sum Memory Access Time
int hits = 0;
int misses = 0;

vector<int> memAdds; // Memory addresses in bytes (number of accesses)

// Reads memory addresses (accesses) from a sequence file into a vector
void readFile(string filePath) {
    ifstream inputFile(filePath);
    if (!inputFile) {
        cerr << "Error opening file: " << filePath << endl;
        return;
    }

    string line, address;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        while (getline(ss, address, ',')) {
            memAdds.push_back(stoi(address));
        }
    }
    inputFile.close();
}

// Cache simulation function
void cacheSim() {
    int cacheLines = S / L; // Number of cache lines
    int index, TAG;

    vector<cacheLine> cache(cacheLines);

    // Output header for trace
    cout << "Trace: \n";
    cout << "Access\tAddress\tIndex\tTag\tVB\tCTag\tResult\n";

    // Loop over each memory address (access)
    for (int i = 0; i < memAdds.size(); i++) {
        int address = memAdds[i];
       // cout << address << " " << L << " " << cacheLines
        index = (address / L) % cacheLines;
        TAG = (address / L) / cacheLines;

       cout << i + 1 << "\t" << address << "\t" << index << "\t" << TAG << "\t"
            << cache[index].VB << "\t" << cache[index].tag << "\t";

       // cout << address << " " << cache[index].VB << " " << cache[index].tag << " " << TAG <<  " " << index << endl;

        if (cache[index].VB && cache[index].tag == TAG) {
            // Hit
            hits++;
            SMAT += cacheAT;
            cout << "Hit\n";
        }
        else {
            // Miss
            misses++;
            cache[index].VB = true;
            cache[index].tag = TAG;
            //cout << "TAG " << TAG << endl;
            SMAT += memAT;
            cout << "Miss\n";
        }

    }

    // Calculate final results
    hitRatio = static_cast<float>(hits) / memAdds.size();
    missRatio = static_cast<float>(misses) / memAdds.size();
    AMAT = cacheAT + missRatio * memAT;

    // Display results
    cout << "\nResults:\n";
    cout << "Hits: " << hits << "\n";
    cout << "Misses: " << misses << "\n";
    cout << "Hit Ratio: " << hitRatio << "\n";
    cout << "Miss Ratio: " << missRatio << "\n";
    cout << "Average Memory Access Time (AMAT): " << AMAT << " cycles\n";
}

// Driver Program
int main() {
    cout << "Enter memoryBits, memory access time, cache size in bytes, cache line size in bytes, and cache access time:\n";
    cin >> memoryBits >> memAT >> S >> L >> cacheAT;
    readFile("C:/Users/HP/OneDrive/Desktop/Cachesim.txt");
    cacheSim();

    return 0;
}
