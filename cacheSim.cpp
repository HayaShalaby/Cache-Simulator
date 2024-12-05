#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

// User inputs declared as global variables 

// Memory inputs
int memoryBits; 
int memAT;

// Cache inputs 
int S; // Cache size in bytes 
int L; // Cache line size in bytes 
int cacheAT;  

// Program Outputs 
vector<int> validBits;
vector<int> tags;
float hitRatio;
float missRatio;
float AMAT; // Average Memory Access Time 
float SMAT; // Sum Memory Access Time 
int hits;
int misses;

// Main structures 
// Cache -- declared later in code 

struct cacheLine{
    int index; 
    bool VB = false; // false is not valid and true is valid 
    int tag = -1; // initialized to -1 to indicate unused cache line
    int data; // might not be used 
};

vector<string> memAdds; // Memory addresses in bytes -- the size of this vector represents the number of accesses

// Reads memory addresses (accesses) from a sequence file into 
void readFile(string filePath){
    // Check if file can be opened
    ifstream inputFile(filePath);
    if (!inputFile) {
        cerr << "Error opening file: " << filePath << endl;
        return;
    }

    // Read each line 
    string line, address;
    while (getline(inputFile, line)) {
        stringstream ss(line);

        // Split different addresses at the commas to store them in the memAdds vector 
        while(getline(ss, address, ',')){
            memAdds.push_back(address); // Store the address
        }
    }
   
    inputFile.close(); // Close the file    
}

// Program function 
void cacheSim(){
    // Calculated values 
    int dispBits = log2(L);
    int cacheLines = S / L; // C 
    int indexBits = log2(cacheLines);
    int tagBits = memoryBits - dispBits - indexBits;

    vector<cacheLine> cache(cacheLines);

    string disp, index, tag;

    int dispI, indexI, tagI;

    // Loop over each memory address (access)
    for(int i = 0; i < memAdds.size(); i++){
            disp = memAdds[i].substr(0, dispBits);
            index = memAdds[i].substr(dispBits, dispBits + indexBits);
            tag = memAdds[i].substr(dispBits + indexBits, dispBits + indexBits + tagBits);

            dispI = stoi(disp);
            indexI = stoi(index);
            tagI = stoi(tag);

            for(int j = 0; j < cacheLines; j++){
                if(cache[i].index == indexI && cache[i].tag == tagI && cache[i].VB){
                    // It's a hit
                    hits++;
                    SMAT += cacheAT; 
                }
                else{
                    misses++;
                    SMAT += memAT;
                }
            }


    }
}

// Driver Program 
int main(){

    // Validate memBits 
    // Validate memAT
    // Validate cacheAT 

    return 0;
}