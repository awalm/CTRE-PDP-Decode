#include <string>
#include <fstream>
#include <map>
#include <iostream>
#include <sstream>

using namespace std;

//Rough C++ program to decode raw can logs into a csv file 
//Data from: https://github.com/wpilibsuite/allwpilib/blob/master/hal/src/main/native/athena/PDP.cpp

union PdpStatus1 {
  uint8_t data[8];
  struct Bits {
    unsigned chan1_h8 : 8;
    unsigned chan2_h6 : 6;
    unsigned chan1_l2 : 2;
    unsigned chan3_h4 : 4;
    unsigned chan2_l4 : 4;
    unsigned chan4_h2 : 2;
    unsigned chan3_l6 : 6;
    unsigned chan4_l8 : 8;
    unsigned chan5_h8 : 8;
    unsigned chan6_h6 : 6;
    unsigned chan5_l2 : 2;
    unsigned reserved4 : 4;
    unsigned chan6_l4 : 4;
  } bits;
};

union PdpStatus2 {
  uint8_t data[8];
  struct Bits {
    unsigned chan7_h8 : 8;
    unsigned chan8_h6 : 6;
    unsigned chan7_l2 : 2;
    unsigned chan9_h4 : 4;
    unsigned chan8_l4 : 4;
    unsigned chan10_h2 : 2;
    unsigned chan9_l6 : 6;
    unsigned chan10_l8 : 8;
    unsigned chan11_h8 : 8;
    unsigned chan12_h6 : 6;
    unsigned chan11_l2 : 2;
    unsigned reserved4 : 4;
    unsigned chan12_l4 : 4;
  } bits;
};

union PdpStatus3 {
  uint8_t data[8];
  struct Bits {
    unsigned chan13_h8 : 8;
    unsigned chan14_h6 : 6;
    unsigned chan13_l2 : 2;
    unsigned chan15_h4 : 4;
    unsigned chan14_l4 : 4;
    unsigned chan16_h2 : 2;
    unsigned chan15_l6 : 6;
    unsigned chan16_l8 : 8;
    unsigned internalResBattery_mOhms : 8;
    unsigned busVoltage : 8;
    unsigned temp : 8;
  } bits;
};

union PdpStatusEnergy {
  uint8_t data[8];
  struct Bits {
    unsigned TmeasMs_likelywillbe20ms_ : 8;
    unsigned TotalCurrent_125mAperunit_h8 : 8;
    unsigned Power_125mWperunit_h4 : 4;
    unsigned TotalCurrent_125mAperunit_l4 : 4;
    unsigned Power_125mWperunit_m8 : 8;
    unsigned Energy_125mWPerUnitXTmeas_h4 : 4;
    unsigned Power_125mWperunit_l4 : 4;
    unsigned Energy_125mWPerUnitXTmeas_mh8 : 8;
    unsigned Energy_125mWPerUnitXTmeas_ml8 : 8;
    unsigned Energy_125mWPerUnitXTmeas_l8 : 8;
  } bits;
};

//API ID mappings
// const std::map<uint32_t, int> api_id_map = 
// {
//     {0x50, PdpStatus1}, 
//     {0x51, PdpStatus2}, 
//     {0x52, PdpStatus3},
//     {0x5D, PdpStatusEnergy}
// };

//TODO:
//Read Log file
//Decode each one
//call decode on it

struct CanMsg {
  uint32_t arb_id;
  uint8_t data[8];
};

//Status to CSV Column mappings
// const std::map<uint32_t, int> dataToColMap = 
// {
//     {0x50, Stat1}, 
//     {0x51, Stat2}, 
//     {0x52, Stat3},
//     {0x5D, StatEnergy}
// };

int main(int argc, char *argv[]) {
  //usage: first arg is the filename

  std::ifstream inFile(argv[1]);
  if(!inFile.is_open()) throw std::runtime_error("Could not open file");

    std::string line, colname;
    CanMsg canMsg;

    // Read the column names
    if(inFile.good())
    {
        // Extract the first line in the file
        std::getline(inFile, line);

        // Create a stringstream from line
        std::stringstream ss(line);

        
    }
  inFile.close();
  cout << "Hello World!";
  return 0;
}


void decodeCanDataFromLine(CanMsg &canMsg, string &rawLine)
{
  
}

void writePdpStatusData1ToFile();