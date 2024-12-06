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
    int tag = -1; // Cache tag
};

// Memory Inputs
int memoryBits; // Memory address bits
int memAT; // Memory access time

// Cache Inputs
int numLevels_data, numLevels_instr; // Number of cache levels for data and instruction
vector<int> cacheSizes_data, cacheSizes_instr; // Cache sizes for data and instruction
vector<int> cacheLineSizes_data, cacheLineSizes_instr; // Cache line sizes for data and instruction
vector<int> cacheATs_data, cacheATs_instr; // Cache access times for data and instruction

vector<int> hits_data, hits_instr, misses_data, misses_instr; // Hits and misses per cache level

// Hit ratios, miss ratios, and average memory access time per cache level
vector<float> hitRatios_data, hitRatios_instr, missRatios_data, missRatios_instr, AMATs_data, AMATs_instr; 

vector<int> instructionMemAdds; // Instruction memory addresses
vector<int> dataMemAdds; // Data memory addresses

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
            memAdds.push_back(stoi(address)); // Store memory accesses
        }
    }
    inputFile.close();
}

// Cache Simulation Function
void cacheSim() {
    
    // Initializing our values for data cache
    hits_data.assign(numLevels_data, 0);
    misses_data.assign(numLevels_data, 0);
    hitRatios_data.assign(numLevels_data, 0);
    missRatios_data.assign(numLevels_data, 0);
    AMATs_data.assign(numLevels_data, 0);
    
    // Initializing our values for instruction cache
    hits_instr.assign(numLevels_instr, 0);
    misses_instr.assign(numLevels_instr, 0);
    hitRatios_instr.assign(numLevels_instr, 0);
    missRatios_instr.assign(numLevels_instr, 0);
    AMATs_instr.assign(numLevels_instr, 0);

    // Initialize cache levels for data cache
    vector<vector<CacheLine> > caches_data(numLevels_data);
    for (int i = 0; i < numLevels_data; i++) {
        int numLines_data = cacheSizes_data[i] / cacheLineSizes_data[i];
        caches_data[i] = vector<CacheLine>(numLines_data);
    }
    
    // Initialize cache levels for instruction cache
    vector<vector<CacheLine> > caches_instr(numLevels_instr);
    for (int i = 0; i < numLevels_instr; i++) {
        int numLines_instr = cacheSizes_instr[i] / cacheLineSizes_instr[i];
        caches_instr[i] = vector<CacheLine>(numLines_instr);
    }

    // Output for tracing data cache
    cout << "Tracing Data Cache:" << endl;
    cout << left << setw(8) << "Access"
         << setw(12) << "Address"
         << setw(8) << "Index"
         << setw(8) << "Tag"
         << setw(6) << "VB"
         << setw(8) << "CTag"
         << setw(10) << "Result"
         << setw(8) << "Level"
         << setw(12) << "Type" << endl;

    // Process data cache accesses
    for (size_t i = 0; i < dataMemAdds.size(); i++) {
        int address = dataMemAdds[i];
        bool hit = false;

        for (int level = 0; level < numLevels_data; level++) {
            int cacheLines = cacheSizes_data[level] / cacheLineSizes_data[level];
            int index = (address / cacheLineSizes_data[level]) % cacheLines;
            int tag = (address / cacheLineSizes_data[level]) / cacheLines;

            // Trace the access
            cout << setw(8) << i + 1
                 << setw(12) << address
                 << setw(8) << index
                 << setw(8) << tag
                 << setw(6) << caches_data[level][index].VB
                 << setw(8) << caches_data[level][index].tag;

            if (caches_data[level][index].VB && caches_data[level][index].tag == tag) {
                cout << setw(10) << "Hit"
                     << setw(8) << level + 1
                     << setw(12) << "Data" << endl;
                hits_data[level]++;
                hit = true;
                break;
            } else {
                cout << setw(10) << "Miss"
                     << setw(8) << level + 1
                     << setw(12) << "Data" << endl;
                misses_data[level]++;
            }

            // Update cache on miss at current level
            if (!hit && level == numLevels_data - 1) {
                caches_data[level][index].VB = true;
                caches_data[level][index].tag = tag;
            }
        }
    }

    // Output for tracing instruction cache
    cout << "\nTracing Instruction Cache:" << endl;
    cout << left << setw(8) << "Access"
         << setw(12) << "Address"
         << setw(8) << "Index"
         << setw(8) << "Tag"
         << setw(6) << "VB"
         << setw(8) << "CTag"
         << setw(10) << "Result"
         << setw(8) << "Level"
         << setw(12) << "Type" << endl;

    // Process instruction cache accesses
    for (size_t i = 0; i < instructionMemAdds.size(); i++) {
        int address = instructionMemAdds[i];
        bool hit = false;

        for (int level = 0; level < numLevels_instr; level++) {
            int cacheLines = cacheSizes_instr[level] / cacheLineSizes_instr[level];
            int index = (address / cacheLineSizes_instr[level]) % cacheLines;
            int tag = (address / cacheLineSizes_instr[level]) / cacheLines;

            // Trace the access
            cout << setw(8) << i + 1
                 << setw(12) << address
                 << setw(8) << index
                 << setw(8) << tag
                 << setw(6) << caches_instr[level][index].VB
                 << setw(8) << caches_instr[level][index].tag;

            if (caches_instr[level][index].VB && caches_instr[level][index].tag == tag) {
                cout << setw(10) << "Hit"
                     << setw(8) << level + 1
                     << setw(12) << "Instruction" << endl;
                hits_instr[level]++;
                hit = true;
                break;
            } else {
                cout << setw(10) << "Miss"
                     << setw(8) << level + 1
                     << setw(12) << "Instruction" << endl;
                misses_instr[level]++;
            }

            // Update cache on miss at current level
            if (!hit && level == numLevels_instr - 1) {
                caches_instr[level][index].VB = true;
                caches_instr[level][index].tag = tag;
            }
        }
    }

    // Calculations for data and instruction cache AMATs
    for (int level = 0; level < numLevels_data; level++) {
        int totalAccesses = dataMemAdds.size();
        hitRatios_data[level] = static_cast<float>(hits_data[level]) / totalAccesses;
        missRatios_data[level] = static_cast<float>(misses_data[level]) / totalAccesses;
        AMATs_data[level] = cacheATs_data[level] + missRatios_data[level] * memAT;
    }

    for (int level = 0; level < numLevels_instr; level++) {
        int totalAccesses = instructionMemAdds.size();
        hitRatios_instr[level] = static_cast<float>(hits_instr[level]) / totalAccesses;
        missRatios_instr[level] = static_cast<float>(misses_instr[level]) / totalAccesses;
        AMATs_instr[level] = cacheATs_instr[level] + missRatios_instr[level] * memAT;
    }

    // Print results
    cout << "\nData Cache Simulation Results:\n";
    cout << left << setw(8) << "Level"
         << setw(10) << "Hits"
         << setw(10) << "Misses"
         << setw(12) << "Hit Ratio"
         << setw(12) << "Miss Ratio"
         << "AMAT (cycles)" << endl;
    for (int level = 0; level < numLevels_data; level++) {
        cout << setw(8) << level + 1
             << setw(10) << hits_data[level]
             << setw(10) << misses_data[level]
             << setw(12) << hitRatios_data[level]
             << setw(12) << missRatios_data[level]
             << AMATs_data[level] << endl;
    }

    cout << "\nInstruction Cache Simulation Results:\n";
    cout << left << setw(8) << "Level"
         << setw(10) << "Hits"
         << setw(10) << "Misses"
         << setw(12) << "Hit Ratio"
         << setw(12) << "Miss Ratio"
         << "AMAT (cycles)" << endl;
    for (int level = 0; level < numLevels_instr; level++) {
        cout << setw(8) << level + 1
             << setw(10) << hits_instr[level]
             << setw(10) << misses_instr[level]
             << setw(12) << hitRatios_instr[level]
             << setw(12) << missRatios_instr[level]
             << AMATs_instr[level] << endl;
    }
}

// Main Driver Function
int main() {
    // Input for memory and cache parameters
    cout << "Enter memory address bits (16 to 40): ";
    cin >> memoryBits;
    cout << "Enter memory access time (50 to 200 cycles): ";
    cin >> memAT;

    // Data cache input
    cout << "Enter number of data cache levels: ";
    cin >> numLevels_data;
    cacheSizes_data.resize(numLevels_data);
    cacheLineSizes_data.resize(numLevels_data);
    cacheATs_data.resize(numLevels_data);

    for (int i = 0; i < numLevels_data; i++) {
        cout << "Level " << i + 1 << " data cache size (bytes): ";
        cin >> cacheSizes_data[i];
        cout << "Level " << i + 1 << " data cache line size (bytes): ";
        cin >> cacheLineSizes_data[i];
        cout << "Level " << i + 1 << " data cache access time (cycles): ";
        cin >> cacheATs_data[i];
    }

    // Instruction cache input
    cout << "Enter number of instruction cache levels: ";
    cin >> numLevels_instr;
    cacheSizes_instr.resize(numLevels_instr);
    cacheLineSizes_instr.resize(numLevels_instr);
    cacheATs_instr.resize(numLevels_instr);

    for (int i = 0; i < numLevels_instr; i++) {
        cout << "Level " << i + 1 << " instruction cache size (bytes): ";
        cin >> cacheSizes_instr[i];
        cout << "Level " << i + 1 << " instruction cache line size (bytes): ";
        cin >> cacheLineSizes_instr[i];
        cout << "Level " << i + 1 << " instruction cache access time (cycles): ";
        cin >> cacheATs_instr[i];
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
