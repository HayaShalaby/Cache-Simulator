#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>

using namespace std;

// Cache Line Structure
struct CacheLine {
    bool VB = false; // Valid bit
    int tag = -1;    // Cache tag
};

// Memory Inputs
int memoryBits;  // Memory address bits
int memAT;       // Memory access time

// Cache Inputs
int numLevels;                // Number of cache levels
vector<int> cacheSizes;       // Cache sizes for each level
vector<int> cacheLineSizes;   // Cache line sizes for each level
vector<int> cacheATs;         // Cache access times for each level

// Simulation Metrics
vector<int> hits, misses;     // Hits and misses per cache level
vector<float> hitRatios, missRatios, AMATs; // Metrics per cache level

// Address Sequences
vector<int> instructionMemAdds; // Instruction memory addresses
vector<int> dataMemAdds;        // Data memory addresses

// Function to read memory addresses from a file
void readFile(const string& filePath, vector<int>& memAdds) {
    ifstream inputFile(filePath);
    if (!inputFile) {
        cerr << "Error opening file: " << filePath << endl;
        exit(1);
    }
    string line, address;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        while (getline(ss, address, ',')) {
            memAdds.push_back(stoi(address));  // Store memory accesses
        }
    }
    inputFile.close();
}

// Cache Simulation Function
void cacheSim() {
    // Initialize vectors for metrics
    hits.assign(numLevels, 0);
    misses.assign(numLevels, 0);
    hitRatios.assign(numLevels, 0);
    missRatios.assign(numLevels, 0);
    AMATs.assign(numLevels, 0);

    // Initialize cache levels
    vector<vector<CacheLine>> caches(numLevels);
    for (int i = 0; i < numLevels; i++) {
        int numLines = cacheSizes[i] / cacheLineSizes[i];
        caches[i] = vector<CacheLine>(numLines);
    }

    // Output column headers for tracing
    cout << left << setw(8) << "Access"
         << setw(12) << "Address"
         << setw(8) << "Index"
         << setw(8) << "Tag"
         << setw(6) << "VB"
         << setw(8) << "CTag"
         << setw(10) << "Result"
         << setw(8) << "Level"
         << setw(12) << "Type" << endl;

    // Process instruction and data accesses
    for (int pass = 0; pass < 2; pass++) {
        const vector<int>& memAdds = (pass == 0) ? instructionMemAdds : dataMemAdds;
        string accessType = (pass == 0) ? "Instruction" : "Data";

        for (size_t i = 0; i < memAdds.size(); i++) {
            int address = memAdds[i];
            bool hit = false;

            for (int level = 0; level < numLevels; level++) {
                int cacheLines = cacheSizes[level] / cacheLineSizes[level];
                int index = (address / cacheLineSizes[level]) % cacheLines;
                int tag = (address / cacheLineSizes[level]) / cacheLines;

                // Trace the access
                cout << setw(8) << i + 1
                     << setw(12) << address
                     << setw(8) << index
                     << setw(8) << tag
                     << setw(6) << caches[level][index].VB
                     << setw(8) << caches[level][index].tag;

                if (caches[level][index].VB && caches[level][index].tag == tag) {
                    cout << setw(10) << "Hit"
                         << setw(8) << level + 1
                         << setw(12) << accessType << endl;
                    hits[level]++;
                    hit = true;
                    break;
                } else {
                    cout << setw(10) << "Miss"
                         << setw(8) << level + 1
                         << setw(12) << accessType << endl;
                    misses[level]++;
                }

                // Update cache on miss at current level
                if (!hit && level == numLevels - 1) {
                    caches[level][index].VB = true;
                    caches[level][index].tag = tag;
                }
            }
        }
    }

    // Calculate metrics
    for (int level = 0; level < numLevels; level++) {
        int totalAccesses = instructionMemAdds.size() + dataMemAdds.size();
        hitRatios[level] = static_cast<float>(hits[level]) / totalAccesses;
        missRatios[level] = static_cast<float>(misses[level]) / totalAccesses;
        AMATs[level] = cacheATs[level] + missRatios[level] * memAT;
    }

    // Print summary results
    cout << "\nCache Simulation Results:\n";
    cout << left << setw(8) << "Level"
         << setw(10) << "Hits"
         << setw(10) << "Misses"
         << setw(12) << "Hit Ratio"
         << setw(12) << "Miss Ratio"
         << "AMAT (cycles)" << endl;
    for (int level = 0; level < numLevels; level++) {
        cout << setw(8) << level + 1
             << setw(10) << hits[level]
             << setw(10) << misses[level]
             << setw(12) << hitRatios[level]
             << setw(12) << missRatios[level]
             << AMATs[level] << endl;
    }
}

// Main Driver Function
int main() {
    // Input for memory and cache parameters
    cout << "Enter memory address bits (16 to 40): ";
    cin >> memoryBits;
    cout << "Enter memory access time (50 to 200 cycles): ";
    cin >> memAT;

    cout << "Enter number of cache levels: ";
    cin >> numLevels;

    cacheSizes.resize(numLevels);
    cacheLineSizes.resize(numLevels);
    cacheATs.resize(numLevels);

    for (int i = 0; i < numLevels; i++) {
        cout << "Level " << i + 1 << " cache size (bytes): ";
        cin >> cacheSizes[i];
        cout << "Level " << i + 1 << " cache line size (bytes): ";
        cin >> cacheLineSizes[i];
        cout << "Level " << i + 1 << " access time (cycles): ";
        cin >> cacheATs[i];
    }

    // Read memory addresses
    string instructionFile, dataFile;
    cout << "Instruction memory address file: ";
    cin >> instructionFile;
    cout << "Data memory address file: ";
    cin >> dataFile;

    readFile(instructionFile, instructionMemAdds);
    readFile(dataFile, dataMemAdds);

    // Run the simulation
    cacheSim();
    return 0;
}
