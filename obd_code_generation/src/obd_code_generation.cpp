/******************************************************************************
 * Copyright (C) 2017 by Yifan Jiang                                          *
 * jiangyi@student.ethz.com                                                   *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 ******************************************************************************/

/*
* generating object dictionary initialization code for the MCS firmware
* based on a user input file
*/

#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdexcept"
#include "string.h"
#include "stdint.h"
#include "iostream"
#include "fstream"
#include "iterator"
#include "sstream"
#include "vector"
#include "algorithm"

using namespace std;


typedef struct ObjInfoTypeDef {
    int Idx;
    int LineNumber;
    vector<string> WordVec;

    ObjInfoTypeDef (int idx, int ln, vector<string> vec) :
       Idx (idx),
       LineNumber(ln),
       WordVec (vec)
    {}

    bool operator< ( const ObjInfoTypeDef& s ) const
    {
        return Idx < s.Idx;
    }

    bool operator> ( const ObjInfoTypeDef& s ) const
    {
        return Idx > s.Idx;
    }

} ObjInfo;

int main(int argc, char **argv)
{
  // open input file
  std::ifstream InputFile("/home/yifan/catkin_ws/src/mcs/obd_code_generation/src/input.txt");

  // extract nonempty lines input vector
  vector<string> InputLines;
  vector<int> InputLineNum;
  int line_count = 0;
  string line;
  while(std::getline(InputFile, line)){
    line_count += 1;
    if( (!line.empty()) && (line[0]!='#')){
      InputLines.push_back(line);
      InputLineNum.push_back(line_count);
    }
  }

  // extract words from each line
  vector<string> Words;
  vector<vector<string>> InputWordVec;
  for(int i=0; i<InputLines.size(); i++){
      Words.clear();
      istringstream iss(InputLines[i]);
      copy(istream_iterator<string>(iss),
           istream_iterator<string>(),
           back_inserter(Words));

      InputWordVec.push_back(Words);
  }

  // extract object indexes
  vector<int> Idx;
  for(int i=0; i<InputWordVec.size(); i++){
    if( ((InputWordVec[i]).size()!=4) ||
        (((InputWordVec[i])[0])[0]!='0') || (((InputWordVec[i])[0])[1]!='x') ||
        (((InputWordVec[i])[1])[0]!='0') || (((InputWordVec[i])[1])[1]!='x') ){
      cout << "ERROR: input file format error (line "<< InputLineNum[i]<< ")" << endl;
      return 0;
    }

    int index = stoi ((InputWordVec[i])[0],nullptr,16);
    int subindex = stoi ((InputWordVec[i])[1],nullptr,16);

    if((index<=0)|(index>0xFFFF)){
      cout << "ERROR: object index out of range (line "<< InputLineNum[i]<< ")" << endl;
      return 0;
    }

    if((subindex<0)|(subindex>0xFF)){
      cout << "ERROR: object subindex out of range (line "<< InputLineNum[i]<< ")" << endl;
      return 0;
    }

    Idx.push_back(index*16*16+subindex);
  }

  vector<ObjInfo> ObjList;
  for(int i=0; i<Idx.size(); i++){
    ObjInfo item(Idx[i], InputLineNum[i], InputWordVec[i]);
    ObjList.push_back(item);
  }

  // sord object indexes
  vector<ObjInfo> ObjList_sorted = ObjList;
  sort(ObjList_sorted.begin(), ObjList_sorted.end());

  // check for repeated indexs
  for(int i=0; i<ObjList.size(); i++){
    auto target = equal_range(ObjList_sorted.begin(), ObjList_sorted.end(), ObjList[i]);

    if((target.first) != (target.second-1)){
      cout << "ERROR: object indexes must be unique (line "<< InputLineNum[i]<< ")" << endl;
      return 0;
    }
  }

  // print out for debugging
  for(auto a : ObjList_sorted){
    cout << a.Idx << " " << a.LineNumber << " ";
    for(auto b : a.WordVec){
      cout << b << " ";
    }
    cout << endl;
  }



  // examle section
  vector<string> vec;
  string test= "    this is my testing string.haha hehe 0x0032";
  istringstream iss(test);
  copy(istream_iterator<string>(iss),
       istream_iterator<string>(),
       back_inserter(vec));




return 0;
}
