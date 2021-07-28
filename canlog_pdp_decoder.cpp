#include <string.h>
#include <fstream>
#include <map>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

//Rough C++ program to decode raw can logs into a csv file
//Data from: https://github.com/wpilibsuite/allwpilib/blob/master/hal/src/main/native/athena/PDP.cpp

union PdpStatus1
{
	uint8_t data[8];
	struct Bits
	{
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

union PdpStatus2
{
	uint8_t data[8];
	struct Bits
	{
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

union PdpStatus3
{
	uint8_t data[8];
	struct Bits
	{
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

union PdpStatusEnergy
{
	uint8_t data[8];
	struct Bits
	{
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

enum PdpDataPoints
{
	RESERVED_FIRST, //must be first
	Timestamp,
	BusVoltage,
	TotalCurrent,
	TotalPower,
	Channel1,
	Channel2,
	Channel3,
	Channel4,
	Channel5,
	Channel6,
	Channel7,
	Channel8,
	Channel9,
	Channel10,
	Channel11,
	Channel12,
	Channel13,
	Channel14,
	Channel15,
	Channel16,
	Temperature,
	BatteryInternalResistance,
	RESERVED_LAST //must be last
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

struct CanMsg
{
	double timestamp;
	uint32_t arb_id;
	uint8_t data[8];
};

enum class PdpStatusData
{
	PdpStatusDataNone,
	PdpStat1,
	PdpStat2,
	PdpStat3,
	PdpStatEnergy
};
struct decodedData
{
	PdpStatusData which_data;
	union
	{
		PdpStatus1 PdpStat1;
		PdpStatus2 PdpStat2;
		PdpStatus3 PdpStat3;
		PdpStatusEnergy PdpStatEnergy;
	};
};
//Status to CSV Column mappings
// const std::map<uint32_t, int> dataToColMap =
// {
//     {0x50, Stat1},
//     {0x51, Stat2},
//     {0x52, Stat3},
//     {0x5D, StatEnergy}
// };

CanMsg decodeCanDataFromLine(std::string &rawLine);
std::vector<char> StringHexToBytes(const std::string &hex);
std::map<PdpDataPoints, double> decodeDataFromCanMsg(CanMsg &msg);
std::vector<char> StringHexToBytes(const std::string &hex);
int hexToDecimal(const int decimal);
void writeDataToFile(std::map<PdpDataPoints, double> &dataMap, std::ofstream &outFile);
std::string map_to_string(std::map<PdpDataPoints, double> &map);

int hexToDecimal(const int decimal)
{
	int hex;

	std::stringstream stream;
	stream << decimal;
	stream >> std::hex >> hex;

	return hex;
}

int main(int argc, char *argv[])
{
	//usage: first arg is the filename

	std::string inputFileName = "candump_july23 - Copy.log"; //todo: not hardcode this
	if (argc > 1)
	{
		std::string inputFileName(argv[1]);
	}

	std::ifstream inFile(inputFileName);

	std::cout << "Opening file: " + inputFileName << std::endl;

	if (!inFile.is_open())
		throw std::runtime_error("Could not open file");

	std::ofstream outFile(inputFileName + "decoded.csv"); //TODO: make this an arg?

	std::string line;
	CanMsg canMsg;

	if (inFile.good())
	{
		// Extract the first line in the file
		while (std::getline(inFile, line))
		{
			// Create a stringstream from line
			std::stringstream ss(line);
			CanMsg msg = decodeCanDataFromLine(line);
			std::map<PdpDataPoints, double> decodedData = decodeDataFromCanMsg(msg);
			writeDataToFile(decodedData, outFile);
		}
	}
	inFile.close();
	outFile.close();
	// system("pause");
	return 0;
}

CanMsg decodeCanDataFromLine(std::string &rawLine)
{
	const int num_cols_log = 3;
	CanMsg result = {0};
	std::istringstream ss(rawLine);
	std::string cols[num_cols_log] = {""};

	std::string segment;

	for (int i = 0; i < num_cols_log; i++)
	{
		if (ss >> segment)
		{
			cols[i] = segment;
		}
	}

	result.timestamp = std::stod(cols[0].substr(1, cols[0].size() - 2)); //strip out the open and close brackets

	std::stringstream rawArbId_data(cols[2]);
	std::vector<std::string> arbId_data;

	while (std::getline(rawArbId_data, segment, '#'))
	{
		arbId_data.push_back(segment);
	}

	if (arbId_data.size() > 1)
	{
		result.arb_id = std::stoul(arbId_data[0]);

		std::vector<char> hexData = StringHexToBytes(arbId_data[1]);

		for (int i = 0; i < hexData.size() && i < sizeof(result.data); i++)
		{
			result.data[i] = hexData[i];
		}
	}
	return result;
}

std::map<PdpDataPoints, double> decodeDataFromCanMsg(CanMsg &msg)
{
	std::map<PdpDataPoints, double> result = {};

	result[PdpDataPoints::Timestamp] = msg.timestamp;

	uint32_t api_id = (hexToDecimal(msg.arb_id) >> 0x6) & 0x3FF;

	if (api_id == 0x50)
	{
		// std::cout << 1 << std::endl;
		PdpStatus1 pdpStatus1;
		memcpy(pdpStatus1.data, msg.data, sizeof(msg.data));
		result[PdpDataPoints::Channel1] = ((static_cast<uint32_t>(pdpStatus1.bits.chan1_h8) << 2) |
										   pdpStatus1.bits.chan1_l2) *
										  0.125;
		result[PdpDataPoints::Channel2] = ((static_cast<uint32_t>(pdpStatus1.bits.chan2_h6) << 4) |
										   pdpStatus1.bits.chan2_l4) *
										  0.125;
		result[PdpDataPoints::Channel3] = ((static_cast<uint32_t>(pdpStatus1.bits.chan3_h4) << 6) |
										   pdpStatus1.bits.chan3_l6) *
										  0.125;
		result[PdpDataPoints::Channel4] = ((static_cast<uint32_t>(pdpStatus1.bits.chan4_h2) << 8) |
										   pdpStatus1.bits.chan4_l8) *
										  0.125;
		result[PdpDataPoints::Channel5] = ((static_cast<uint32_t>(pdpStatus1.bits.chan5_h8) << 2) |
										   pdpStatus1.bits.chan5_l2) *
										  0.125;
		result[PdpDataPoints::Channel6] = ((static_cast<uint32_t>(pdpStatus1.bits.chan6_h6) << 4) |
										   pdpStatus1.bits.chan6_l4) *
										  0.125;
	}
	else if (api_id == 0x51)
	{
		// std::cout << 2 << std::endl;
		PdpStatus2 pdpStatus2;
		memcpy(pdpStatus2.data, msg.data, sizeof(msg.data));
		result[PdpDataPoints::Channel7] = ((static_cast<uint32_t>(pdpStatus2.bits.chan7_h8) << 2) |
										   pdpStatus2.bits.chan7_l2) *
										  0.125;
		result[PdpDataPoints::Channel8] = ((static_cast<uint32_t>(pdpStatus2.bits.chan8_h6) << 4) |
										   pdpStatus2.bits.chan8_l4) *
										  0.125;
		result[PdpDataPoints::Channel9] = ((static_cast<uint32_t>(pdpStatus2.bits.chan9_h4) << 6) |
										   pdpStatus2.bits.chan9_l6) *
										  0.125;
		result[PdpDataPoints::Channel10] = ((static_cast<uint32_t>(pdpStatus2.bits.chan10_h2) << 8) |
											pdpStatus2.bits.chan10_l8) *
										   0.125;
		result[PdpDataPoints::Channel11] = ((static_cast<uint32_t>(pdpStatus2.bits.chan11_h8) << 2) |
											pdpStatus2.bits.chan11_l2) *
										   0.125;
		result[PdpDataPoints::Channel12] = ((static_cast<uint32_t>(pdpStatus2.bits.chan12_h6) << 4) |
											pdpStatus2.bits.chan12_l4) *
										   0.125;
	}
	else if (api_id == 0x52)
	{
		// std::cout << 3 << std::endl;
		PdpStatus3 pdpStatus3;
		memcpy(pdpStatus3.data, msg.data, sizeof(msg.data));

		result[PdpDataPoints::Channel13] = ((static_cast<uint32_t>(pdpStatus3.bits.chan13_h8) << 2) |
											pdpStatus3.bits.chan13_l2) *
										   0.125;
		result[PdpDataPoints::Channel14] = ((static_cast<uint32_t>(pdpStatus3.bits.chan14_h6) << 4) |
											pdpStatus3.bits.chan14_l4) *
										   0.125;
		result[PdpDataPoints::Channel15] = ((static_cast<uint32_t>(pdpStatus3.bits.chan15_h4) << 6) |
											pdpStatus3.bits.chan15_l6) *
										   0.125;
		result[PdpDataPoints::Channel16] = ((static_cast<uint32_t>(pdpStatus3.bits.chan16_h2) << 8) |
											pdpStatus3.bits.chan16_l8) *
										   0.125;

		result[PdpDataPoints::BusVoltage] = pdpStatus3.bits.busVoltage * 0.05 + 4.0;
		//Decode temperature
		result[PdpDataPoints::Temperature] = pdpStatus3.bits.temp * 1.03250836957542 - 67.8564500484966;
		//Decode battery internal resistance
		result[PdpDataPoints::BatteryInternalResistance] = pdpStatus3.bits.internalResBattery_mOhms;
	}
	else if (api_id == 0x5D)
	{
		// std::cout << 4 << std::endl;
		PdpStatusEnergy pdpStatusEnergy;
		memcpy(pdpStatusEnergy.data, msg.data, sizeof(msg.data));

		//Decode total current
		uint32_t raw;
		raw = pdpStatusEnergy.bits.TotalCurrent_125mAperunit_h8;
		raw <<= 4;
		raw |= pdpStatusEnergy.bits.TotalCurrent_125mAperunit_l4;
		result[PdpDataPoints::TotalCurrent] = 0.125 * raw;

		//Decode total power
		raw = 0;
		raw = pdpStatusEnergy.bits.Power_125mWperunit_h4;
		raw <<= 8;
		raw |= pdpStatusEnergy.bits.Power_125mWperunit_m8;
		raw <<= 4;
		raw |= pdpStatusEnergy.bits.Power_125mWperunit_l4;
		result[PdpDataPoints::TotalPower] = 0.125 * raw;
	}
	return result;
}

std::vector<char> StringHexToBytes(const std::string &hex)
{
	std::vector<char> bytes;

	for (unsigned int i = 0; i < hex.length(); i += 2)
	{
		std::string byteString = hex.substr(i, 2);
		char byte = (char)strtol(byteString.c_str(), NULL, 16);
		bytes.push_back(byte);
	}

	return bytes;
}

std::string map_to_string(std::map<PdpDataPoints, double> &map)
{
	std::string out;

	for (PdpDataPoints i = PdpDataPoints::RESERVED_FIRST; i < PdpDataPoints::RESERVED_LAST; i = PdpDataPoints(i + 1))
	{
		std::map<PdpDataPoints, double>::const_iterator pos = map.find(PdpDataPoints(i));

		if (pos != map.end())
		{
			std::cout << "hmm" << pos->second << std::endl;

			out += std::to_string(pos->second);
		}

		out += ",";
	}

	out += "\n";

	// std::cout << "map_to_string " << out << std::endl;

	return out;
}

void writeDataToFile(std::map<PdpDataPoints, double> &dataMap, std::ofstream &fileHandle)
{
	std::string s = "";

	for (int i = (int)PdpDataPoints::RESERVED_FIRST + 1; i < (int)PdpDataPoints::RESERVED_LAST; i++)
	{
		if (dataMap.find((PdpDataPoints)i) != dataMap.end())
		{
			std::cout << "adding " << (PdpDataPoints)i << " = " << dataMap[(PdpDataPoints)i] << std::endl;

			s += dataMap[(PdpDataPoints)i];
		}

		if (i != (int)PdpDataPoints::RESERVED_LAST - 1)
		{
			s += ",";
		}
	}

	std::cout << map_to_string(dataMap) << std::endl;
	fileHandle << map_to_string(dataMap) << std::endl;
}
