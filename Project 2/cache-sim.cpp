#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<pair<char, unsigned int>> getData(char* fileName){
  vector<pair<char, unsigned int>> result;
  string line;

  ifstream infile(fileName);
  while(getline(infile, line)){
    stringstream s(line);
    pair<char, unsigned int> curData;
    s >> curData.first >> hex >> curData.second;
    result.push_back(curData);
  }

  return result;
}

int directMapped(vector<pair<char, unsigned int>> data, int cacheSize){
  int powNum = pow(2, cacheSize), result = 0, mask = (1 << 10) - 1;
  unsigned int cache[powNum];
  int validBits[powNum];

  for(int i = 0; i < powNum; i++){
    validBits[i] = 0;
  }

  for(int i = 0; i < data.size(); i++){
    int block = (data[i].second / 32) % (powNum / 32);
    unsigned int remainder = data[i].second & (~mask);
    if(validBits[block] == 0 || cache[block] != remainder){
      cache[block] = remainder;
      validBits[block] = 1;
    } else {
      result++;
    }
  }

  return result;
}

int setAssociative(vector<pair<char, unsigned int>> data, int associativity){
  int powNum = pow(2, 14), result = 0, numSets = (powNum / 32) / associativity;
  unsigned int cache[numSets][associativity];
  int validBits[numSets][associativity];
  vector<vector<int>> leastUsed;
  int mask = (1 << (5 + (int)log2(numSets))) - 1;

  for(int i = 0; i < numSets; i++){
    for(int j = 0; j < associativity; j++){
      validBits[i][j] = 0;
    }
    vector<int> curSet;
    leastUsed.push_back(curSet);
  }

  for(int i = 0; i < data.size(); i++){
    int block = (data[i].second / 32) % (powNum / 32);
    int setNum = block % (numSets);
    unsigned int remainder = data[i].second & (~mask);
    bool stored = false;
    for(int j = 0; j < associativity; j++){
      if(validBits[setNum][j] == 0){
        validBits[setNum][j] = 1;
        cache[setNum][j] = remainder;
        stored = true;
        leastUsed[setNum].push_back(j);
        break;
      }
      if(validBits[setNum][j] != 0 && cache[setNum][j] == remainder){
        result++;
        stored = true;
        leastUsed[setNum].erase(remove(leastUsed[setNum].begin(), leastUsed[setNum].end(), j), leastUsed[setNum].end());
        leastUsed[setNum].push_back(j);
        break;
      }
    }
    if(!stored){
      cache[setNum][leastUsed[setNum][0]] = remainder;
      leastUsed[setNum].push_back(leastUsed[setNum][0]);
      leastUsed[setNum].erase(leastUsed[setNum].begin());
    }
  }

  return result;
}

vector<int> updateHeat(vector<int> heatBits, int index){
  int max = heatBits.size() - 1, min = 0;
  int prevMax = -1, prevMin = -1;
  int runs = (int)log2(heatBits.size() + 1);
  for(int i = 0; i < runs; i++){
    prevMax = max;
    prevMin = min;
    int checkIndex = (max + min) / 2;
    if(index <= checkIndex){
      heatBits[checkIndex] = 0;
      max = checkIndex;
    } else {
      heatBits[checkIndex] = 1;
      min = checkIndex;
    }
  }
  return heatBits;
}

int hotCold(vector<pair<char, unsigned int>> data){
  int result = 0, associativity = 512;
  unsigned int cache[associativity];
  int validBits[associativity];
  int mask = (1 << 5) - 1;
  vector<int> heatBits;

  for(int i = 0; i < associativity; i++){
    validBits[i] = 0;
    if(i != associativity - 1){
      heatBits.push_back(0);
    }
  }

  for(int i = 0; i < data.size(); i++){
    unsigned int remainder = data[i].second & (~mask);
    bool stored = false;
    for(int j = 0; j < associativity; j++){
      if(validBits[j] != 0 && cache[j] == remainder){
        result++;
        stored = true;
        heatBits = updateHeat(heatBits, j);
        break;
      }
    }
    if(!stored){
      int rangeMin = 0;
      int rangeMax = associativity - 1;
      while(rangeMin != rangeMax){
        int checkHeat = (rangeMin + rangeMax) / 2;
        if(heatBits[checkHeat] == 0){
          rangeMin = checkHeat + 1;
          heatBits[checkHeat] = 1;
        } else {
          rangeMax = checkHeat;
          heatBits[checkHeat] = 0;
        }
      }
      cache[rangeMin] = remainder;
      validBits[rangeMin] = 1;
    }
  }

  return result;
}

int noWrite(vector<pair<char, unsigned int>> data, int associativity){
  int powNum = pow(2, 14), result = 0, numSets = (powNum / 32) / associativity;
  unsigned int cache[numSets][associativity];
  int validBits[numSets][associativity];
  vector<vector<int>> leastUsed;
  int mask = (1 << (5 + (int)log2(numSets))) - 1;

  for(int i = 0; i < numSets; i++){
    for(int j = 0; j < associativity; j++){
      validBits[i][j] = 0;
    }
    vector<int> curSet;
    leastUsed.push_back(curSet);
  }

  for(int i = 0; i < data.size(); i++){
    int block = (data[i].second / 32) % (powNum / 32);
    int setNum = block % (numSets);
    unsigned int remainder = data[i].second & (~mask);
    bool stored = false;
    bool loaded = (data[i].first == 'L');
    for(int j = 0; j < associativity; j++){
      if(loaded && validBits[setNum][j] == 0){
        validBits[setNum][j] = 1;
        cache[setNum][j] = remainder;
        stored = true;
        leastUsed[setNum].push_back(j);
        break;
      }
      if(validBits[setNum][j] != 0 && cache[setNum][j] == remainder){
        result++;
        stored = true;
        leastUsed[setNum].erase(remove(leastUsed[setNum].begin(), leastUsed[setNum].end(), j), leastUsed[setNum].end());
        leastUsed[setNum].push_back(j);
        break;
      }
    }
    if(!stored && loaded){
      cache[setNum][leastUsed[setNum][0]] = remainder;
      leastUsed[setNum].push_back(leastUsed[setNum][0]);
      leastUsed[setNum].erase(leastUsed[setNum].begin());
    }
  }

  return result;
}

int nextPrefetch(vector<pair<char, unsigned int>> data, int associativity){
  int powNum = pow(2, 14), result = 0, numSets = (powNum / 32) / associativity;
  unsigned int cache[numSets][associativity];
  int validBits[numSets][associativity];
  vector<vector<int>> leastUsed;
  int mask = (1 << (5 + (int)log2(numSets))) - 1;

  for(int i = 0; i < numSets; i++){
    for(int j = 0; j < associativity; j++){
      validBits[i][j] = 0;
    }
    vector<int> curSet;
    leastUsed.push_back(curSet);
  }

  for(int i = 0; i < data.size(); i++){
    int block = (data[i].second / 32) % (powNum / 32);
    int setNum = block % (numSets);
    unsigned int remainder = data[i].second & (~mask);
    bool stored = false;
    for(int j = 0; j < associativity; j++){
      if(validBits[setNum][j] == 0){
        validBits[setNum][j] = 1;
        cache[setNum][j] = remainder;
        stored = true;
        leastUsed[setNum].push_back(j);
        if(j + 1 < associativity){
          validBits[setNum][j + 1] = 1;
          cache[setNum][j + 1] = remainder + 32;
          leastUsed[setNum].push_back(j + 1);
        } else {
          cache[setNum][leastUsed[setNum][0]] = remainder + 32;
          leastUsed[setNum].push_back(leastUsed[setNum][0]);
          leastUsed[setNum].erase(leastUsed[setNum].begin());
        }
        break;
      }
      if(validBits[setNum][j] != 0 && cache[setNum][j] == remainder){
        result++;
        stored = true;
        leastUsed[setNum].erase(remove(leastUsed[setNum].begin(), leastUsed[setNum].end(), j), leastUsed[setNum].end());
        leastUsed[setNum].push_back(j);
        bool storedAgain = false;
        for(int k = j + 1; k < associativity; k++){
          if(validBits[setNum][k] == 0){
            validBits[setNum][k] = 1;
            cache[setNum][k] = remainder;
            leastUsed[setNum].push_back(k);
            storedAgain = true;
            break;
          }
        }
        if(!storedAgain){
          cache[setNum][leastUsed[setNum][0]] = remainder;
          leastUsed[setNum].push_back(leastUsed[setNum][0]);
          leastUsed[setNum].erase(leastUsed[setNum].begin());
        }
        break;
      }
    }
    if(!stored){
      cache[setNum][leastUsed[setNum][0]] = remainder;
      leastUsed[setNum].push_back(leastUsed[setNum][0]);
      leastUsed[setNum].erase(leastUsed[setNum].begin());
      cache[setNum][leastUsed[setNum][0]] = remainder + 32;
      leastUsed[setNum].push_back(leastUsed[setNum][0]);
      leastUsed[setNum].erase(leastUsed[setNum].begin());
    }
  }

  return result;
}

int main(int argc, char** argv){
  if(argc != 3){
    cout << "Usage: ./cache-sim <input file> <output file>" << endl;
    exit(1);
  }

  vector<pair<char, unsigned int>> inputData = getData(argv[1]);

  unsigned int numOps = inputData.size();
  ofstream outputFile(argv[2]);
  if(outputFile.is_open()){
    outputFile << directMapped(inputData, 10) << "," << numOps << "; ";
    outputFile << directMapped(inputData, 12) << "," << numOps << "; ";
    outputFile << directMapped(inputData, 14) << "," << numOps << "; ";
    outputFile << directMapped(inputData, 15) << "," << numOps << "; " << endl;
    outputFile << setAssociative(inputData, 2) << "," << numOps << "; ";
    outputFile << setAssociative(inputData, 4) << "," << numOps << "; ";
    outputFile << setAssociative(inputData, 8) << "," << numOps << "; ";
    outputFile << setAssociative(inputData, 16) << "," << numOps << "; " << endl;
    outputFile << setAssociative(inputData, 512) << "," << numOps << "; " << endl;
    outputFile << hotCold(inputData) << "," << numOps << "; " << endl;
    outputFile << noWrite(inputData, 2) << "," << numOps << "; ";
    outputFile << noWrite(inputData, 4) << "," << numOps << "; ";
    outputFile << noWrite(inputData, 8) << "," << numOps << "; ";
    outputFile << noWrite(inputData, 16) << "," << numOps << "; " << endl;
    outputFile << nextPrefetch(inputData, 2) << "," << numOps << "; ";
    outputFile << nextPrefetch(inputData, 4) << "," << numOps << "; ";
    outputFile << nextPrefetch(inputData, 8) << "," << numOps << "; ";
    outputFile << nextPrefetch(inputData, 16) << "," << numOps << "; " << endl;
  } else {
    cout << "Unable to open " << argv[2] << endl;
  }

}
