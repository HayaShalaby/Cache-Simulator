#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip> // For setting width of columns

using namespace std;

// Cache Line Structure
struct cacheLine {
    bool VB = false; // Valid bit (false: invalid, true: valid)
    int tag = -1;    // Tag (initialized to -1 for unused cache line)
};

// Memory Inputs
int memoryBits;  // Memory address bits
int memAT;       // Memory access time

// Cache Inputs
int S;           // Cache size in bytes
int L;           // Cache line size in bytes
int cacheAT;     // Cache access time

// Program Outputs
float hitRatioInst;
float missRatioInst;
float AMATInst;      // Average Memory Access Time for instructions
float hitRatioData;
float missRatioData;
float AMATData;      // Average Memory Access Time for data
int hitsInst = 0;    // Instruction cache hits
int missesInst = 0;  // Instruction cache misses
int hitsData = 0;    // Data cache hits
int missesData = 0;  // Data cache misses

vector<int> instructionMemAdds; // Instruction memory addresses
vector<int> dataMemAdds;       // Data memory addresses

// Function to read memory addresses from a file
void readInstructionFile(const string& filePath) {
    ifstream inputFile(filePath);
    if (!inputFile) {
        cerr << "Error opening instruction file: " << filePath << endl;
        return;
    }

    string line, address;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        while (getline(ss, address, ',')) {
            instructionMemAdds.push_back(stoi(address));  // Store instruction accesses
        }
    }
    inputFile.close();
}

void readDataFile(const string& filePath) {
    ifstream inputFile(filePath);
    if (!inputFile) {
        cerr << "Error opening data file: " << filePath << endl;
        return;
    }

    string line, address;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        while (getline(ss, address, ',')) {
            dataMemAdds.push_back(stoi(address));  // Store data accesses
        }
    }
    inputFile.close();
}

// Cache Simulation Function
void cacheSim() {
    int cacheLines = S / L;  // Number of cache lines
    int index, TAG;

    vector<cacheLine> instCache(cacheLines);  // Instruction cache
    vector<cacheLine> dataCache(cacheLines);  // Data cache

    // Output the trace with improved column formatting
    cout << left << setw(6) << "Access"
         << setw(12) << "Address"
         << setw(8) << "Index"
         << setw(8) << "Tag"
         << setw(6) << "VB"
         << setw(8) << "CTag"
         << setw(10) << "Result"
         << setw(12) << "Type" << endl;

    // Loop over instruction addresses first
    for (int i = 0; i < instructionMemAdds.size(); i++) {
        int address = instructionMemAdds[i];
        
        // Calculate the index and tag for instruction cache
        index = (address / L) % cacheLines;
        TAG = (address / L) / cacheLines;

        cout << setw(6) << i + 1
             << setw(12) << address
             << setw(8) << index
             << setw(8) << TAG
             << setw(6) << instCache[index].VB
             << setw(8) << instCache[index].tag
             << setw(10) << (instCache[index].VB && instCache[index].tag == TAG ? "Hit" : "Miss")
             << setw(12) << "Instruction" << endl;

        if (instCache[index].VB && instCache[index].tag == TAG) {
            // Instruction cache hit
            hitsInst++;
        } else {
            // Instruction cache miss
            missesInst++;
            instCache[index].VB = true;
            instCache[index].tag = TAG;
        }
    }

    // Loop over data addresses next
    for (int i = 0; i < dataMemAdds.size(); i++) {
        int address = dataMemAdds[i];
        
        // Calculate the index and tag for data cache
        index = (address / L) % cacheLines;
        TAG = (address / L) / cacheLines;

        cout << setw(6) << i + 1
             << setw(12) << address
             << setw(8) << index
             << setw(8) << TAG
             << setw(6) << dataCache[index].VB
             << setw(8) << dataCache[index].tag
             << setw(10) << (dataCache[index].VB && dataCache[index].tag == TAG ? "Hit" : "Miss")
             << setw(12) << "Data" << endl;

        if (dataCache[index].VB && dataCache[index].tag == TAG) {
            // Data cache hit
            hitsData++;
        } else {
            // Data cache miss
            missesData++;
            dataCache[index].VB = true;
            dataCache[index].tag = TAG;
        }
    }

    // Calculate hit ratio, miss ratio, and AMAT for both instruction and data caches
    hitRatioInst = static_cast<float>(hitsInst) / instructionMemAdds.size();
    missRatioInst = static_cast<float>(missesInst) / instructionMemAdds.size();
    AMATInst = cacheAT + missRatioInst * memAT;

    hitRatioData = static_cast<float>(hitsData) / dataMemAdds.size();
    missRatioData = static_cast<float>(missesData) / dataMemAdds.size();
    AMATData = cacheAT + missRatioData * memAT;

    // Display the final results for both instruction and data caches
    cout << "\nResults for Instruction Cache:\n";
    cout << "Hits: " << hitsInst << "\n";
    cout << "Misses: " << missesInst << "\n";
    cout << "Hit Ratio: " << hitRatioInst << "\n";
    cout << "Miss Ratio: " << missRatioInst << "\n";
    cout << "Average Memory Access Time (AMAT): " << AMATInst << " cycles\n";

    cout << "\nResults for Data Cache:\n";
    cout << "Hits: " << hitsData << "\n";
    cout << "Misses: " << missesData << "\n";
    cout << "Hit Ratio: " << hitRatioData << "\n";
    cout << "Miss Ratio: " << missRatioData << "\n";
    cout << "Average Memory Access Time (AMAT): " << AMATData << " cycles\n";
}

// Main Driver Program
int main() {
    // User-friendly prompts with input validation
    cout << "Enter the following parameters for the cache simulation:\n";

    // Input for memory bits
    while (true) {
        cout << "Memory address bits (16 to 40): ";
        cin >> memoryBits;
        if (memoryBits >= 16 && memoryBits <= 40) break;
        else cout << "Invalid input. Memory address bits must be between 16 and 40." << endl;
    }

    // Input for memory access time
    while (true) {
        cout << "Memory access time (50 to 200 cycles): ";
        cin >> memAT;
        if (memAT >= 50 && memAT <= 200) break;
        else cout << "Invalid input. Memory access time must be between 50 and 200 cycles." << endl;
    }

    // Input for cache size and line size
    while (true) {
        cout << "Cache size (must be divisible by cache line size, e.g., 1024 bytes): ";
        cin >> S;
        cout << "Cache line size (must divide cache size, e.g., 64 bytes): ";
        cin >> L;
        if (S % L == 0) break;
        else cout << "Cache size must be divisible by cache line size!" << endl;
    }

    // Input for cache access time
    while (true) {
        cout << "Cache access time (10 to 50 cycles): ";
        cin >> cacheAT;
        if (cacheAT >= 10 && cacheAT <= 50) break;
        else cout << "Invalid input. Cache access time must be between 10 and 50 cycles." << endl;
    }

    // Get file paths for instruction and data accesses
    string instructionFile, dataFile;
    cout << "Enter the file path for the instruction access sequence: ";
    cin >> instructionFile;
    cout << "Enter the file path for the data access sequence: ";
    cin >> dataFile;

    // Read the memory addresses from both files
    readInstructionFile(instructionFile);
    readDataFile(dataFile);

    // Simulate the cache and compute the results
    cacheSim();

    return 0;
}
