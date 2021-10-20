#include<fstream>
#include<iostream>
#include<math.h>
#include<sstream>
#include<string>
#include<vector>

using namespace std;

vector<unsigned long long> addrs;
vector<string> behaviors;
vector<unsigned long long> targets;

void getBranches(char* fileName){
  unsigned long long addr;
  string behavior, line;
  unsigned long long target;

  ifstream infile(fileName);

  while(getline(infile, line)){
    stringstream s(line);
    s >> hex >> addr >> behavior >> hex >> target;

    addrs.push_back(addr);
    behaviors.push_back(behavior);
    targets.push_back(target);
  }

  infile.close();
}

int alwaysTaken(){
  int result = 0;

  for(int i = 0; i < (int)behaviors.size(); i++) {

    if(behaviors[i] == "T") {
      result++;
    }
  }

  return result;
}

int bimodal1Bit(int tablePower){
  int result = 0;
  int tableSize = pow(2, tablePower);

  int history[tableSize];
  for(int i = 0; i < tableSize; i++){
    history[i] = 1;
  }
  int mask = 0;
  for(int i = 0; i < tablePower; i++){
    mask = mask << 1;
    mask++;
  }

  for(int i = 0; i < (int)behaviors.size(); i++) {

    int tableBlock = addrs[i] & mask;
    if(behaviors[i] == "T"){
      if(history[tableBlock] == 1){
        result++;
      } else {
        history[tableBlock] = 1;
      }
    } else {
      if(history[tableBlock] == 1){
        history[tableBlock] = 0;
      } else {
        result++;
      }
    }
  }

  return result;
}

int get2BitChange(int current, bool taken){
  if(taken){
    switch(current){
      case 0:
        return 1;
      case 1:
        return 10;
      case 10:
        return 11;
      case 11:
        return 11;
    }
  } else {
    switch(current){
      case 0:
        return 0;
      case 1:
        return 0;
      case 10:
        return 1;
      case 11:
        return 10;
    }
  }
  return -1;
}

int bimodal2Bit(int tablePower){
  int result = 0;
  int tableSize = pow(2, tablePower);

  int history[tableSize];
  for(int i = 0; i < tableSize; i++){
    history[i] = 11;
  }
  int mask = 0;
  for(int i = 0; i < tablePower; i++){
    mask = mask << 1;
    mask++;
  }

  for(int i = 0; i < (int)behaviors.size(); i++) {

    int tableBlock = addrs[i] & mask;
    if(behaviors[i] == "T"){
      if(history[tableBlock] == 10 || history[tableBlock] == 11){
        result++;
      }
    } else {
      if(history[tableBlock] == 1 || history[tableBlock] == 0){
        result++;
      }
    }
    history[tableBlock] = get2BitChange(history[tableBlock], behaviors[i] == "T");
  }

  return result;
}

int gshare(int historyLength){
  int globalHistory = 0;
  int result = 0;

  int history[2048];
  for(int i = 0; i < 2048; i++){
    history[i] = 11;
  }

  int mask = 0;
  for(int i = 0; i < 11; i++){
    mask = mask << 1;
    mask++;
  }

  for(int i = 0; i < (int)behaviors.size(); i++) {

    int checker = addrs[i] & mask;
    checker = checker ^ globalHistory;

    if(behaviors[i] == "T"){
      if(history[checker] == 11 || history[checker] == 10){
        result++;
      }
    } else {
      if(history[checker] == 0 || history[checker] == 1){
        result++;
      }
    }
    int change = get2BitChange(history[checker], behaviors[i] == "T");
    history[checker] = change;

    globalHistory = globalHistory << 1;
    if(behaviors[i] == "T"){
      globalHistory++;
    }
    globalHistory = globalHistory % int(pow(2, historyLength));
  }

  return result;
}

int tournament(){
  int result = 0;
  int tableSize = 2048;
  int globalHistory = 0;

  int bimodalHistory[tableSize];
  for(int i = 0; i < tableSize; i++){
    bimodalHistory[i] = 11;
  }

  int gshareHistory[tableSize];
  for(int i = 0; i < tableSize; i++){
    gshareHistory[i] = 11;
  }

  int tournamentHistory[tableSize];
  for(int i = 0; i < tableSize; i++){
    tournamentHistory[i] = 0;
  }

  int mask = 0;
  for(int i = 0; i < 11; i++){
    mask = mask << 1;
    mask++;
  }

  for(int i = 0; i < (int)behaviors.size(); i++){
    bool gshareBool = false;
    bool bimodalBool = false;
    bool tournamentBool = false;

    int gshareChecker = addrs[i] & mask;
    gshareChecker = gshareChecker ^ globalHistory;

    if(gshareHistory[gshareChecker] == 11 || gshareHistory[gshareChecker] == 10){
      gshareBool = true;
    }
    int gshareChange = get2BitChange(gshareHistory[gshareChecker], behaviors[i] == "T");
    gshareHistory[gshareChecker] = gshareChange;

    globalHistory = globalHistory << 1;
    if(behaviors[i] == "T"){
      globalHistory++;
    }
    globalHistory = globalHistory % tableSize;

    int tableBlock = addrs[i] & mask;
    if(bimodalHistory[tableBlock] == 10 || bimodalHistory[tableBlock] == 11){
      bimodalBool = true;
    }
    bimodalHistory[tableBlock] = get2BitChange(bimodalHistory[tableBlock], behaviors[i] == "T");

    if(tournamentHistory[tableBlock] == 0 || tournamentHistory[tableBlock] == 1){
      tournamentBool = gshareBool;
    } else {
      tournamentBool = bimodalBool;
    }

    if((tournamentBool == true && behaviors[i] == "T") || (tournamentBool == false && behaviors[i] != "T")){
      result++;
    }
    if(bimodalBool != gshareBool){
      if((bimodalBool == true && behaviors[i] == "T") || (bimodalBool == false && behaviors[i] != "T")){
        tournamentHistory[tableBlock] = get2BitChange(tournamentHistory[tableBlock], true);
      } else {
        tournamentHistory[tableBlock] = get2BitChange(tournamentHistory[tableBlock], false);
      }
    }
  }
  return result;
}

vector<int> BTB(){
  vector<int> result;
  int correct = 0;
  int tableSize = 512;
  int taken = 0;

  int history[tableSize];
  for(int i = 0; i < tableSize; i++){
    history[i] = 1;
  }
  unsigned long long buffer[tableSize];
  for(int i = 0; i < tableSize; i++){
    buffer[i] = 0;
  }
  int mask = 0;
  for(int i = 0; i < 9; i++){
    mask = mask << 1;
    mask++;
  }

  for(int i = 0; i < (int)behaviors.size(); i++) {
    int tableBlock = addrs[i] & mask;
    if(history[tableBlock] == 1){
      taken++;
      if(targets[i] == buffer[tableBlock]){
        correct++;
      }
    }
    if(behaviors[i] == "T"){
      buffer[tableBlock] = targets[i];
      history[tableBlock] = 1;
    } else {
      history[tableBlock] = 0;
    }
  }

  result.push_back(correct);
  result.push_back(taken);
  return result;
}

int main(int argc, char** argv){

  if(argc != 3){
    cout << "Usage: ./predictors [input file name] [output file name]" << endl;
    return 1;
  }

  char* fileName = argv[1];

  getBranches(fileName);

  int numBranches = behaviors.size();
  int numAlwaysTaken = alwaysTaken();
  int bimodal1Bits[7];
  int bimodal2Bits[7];
  int adding = 4;
  for(int i = 0; i < 7; i++){
    if(i == 2){
      adding++;
    }
    bimodal1Bits[i] = bimodal1Bit(i + adding);
    bimodal2Bits[i] = bimodal2Bit(i + adding);
  }
  int numsGShare[9];
  for(int i = 3; i < 12; i++){
    numsGShare[i - 3] = gshare(i);
  }
  int tournamentResult = tournament();
  vector<int> buffer = BTB();

  ofstream outputFile(argv[2]);
  if(outputFile.is_open()){
    outputFile << numAlwaysTaken << "," << numBranches << "; " << endl;
    outputFile << numBranches - numAlwaysTaken << "," << numBranches << "; " << endl;
    for(int i = 0; i < 7; i++){
      outputFile << bimodal1Bits[i] << "," << numBranches << "; ";
    }
    outputFile << endl;
    for(int i = 0; i < 7; i++){
      outputFile << bimodal2Bits[i] << "," << numBranches << "; ";
    }
    outputFile << endl;
    for(int i = 0; i < 9; i++){
      outputFile << numsGShare[i] << "," << numBranches << "; ";
    }
    outputFile << endl;
    outputFile << tournamentResult << "," << numBranches << "; " << endl;
    outputFile << buffer[0] << "," << buffer[1] << "; " << endl;

  } else {
    cout << "Unable to open " << argv[2] << endl;
  }

  return 0;
}
