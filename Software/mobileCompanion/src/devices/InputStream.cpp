#include "InputStream.hpp"

#include <cstdint>
#include <iostream>
#include <sstream>

namespace devices
{

auto readNPrint = []<typename T, typename Out = T>(std::ifstream& in, std::string const& name) {
    static T buffer;
    in.read((char*)&buffer, sizeof(T));
    if(not name.empty())
        std::cout << name << ": " << (Out)buffer << std::endl;
    return buffer;
};
auto read = []<typename T, typename Out = T>(std::ifstream& in) {
    static T buffer;
    in.read((char*)&buffer, sizeof(T));
    return buffer;
};

FileInputStream::FileInputStream(std::string const& fileName)
{
    file.open(fileName, std::ios::in | std::ios::binary);
    if(!file)
        throw std::runtime_error("Failed to open " + fileName);

    size_t headerSize = 9;
    {
        formatVersion = readNPrint.template operator()<uint16_t>(file, "fileFormatVersion");
        switch(formatVersion)
        {
            case 1:
                headerSize = 9;
                break;
            case 2:
                headerSize = 10;
                break;
            case 3:
                headerSize = 14;
                break;
            case 4:
                headerSize = read.template operator()<uint16_t>(file);
                break;
            default:
                throw std::runtime_error("Unknown file format version " + std::to_string(formatVersion));
        }

        fftWidth = 1024;
    }
    size_t readHeaderBytes = 0;

    readHeaderBytes += 4;
    readNPrint.template operator()<uint32_t>(file, "timestamp");

    readHeaderBytes += 2;
    batchSize = readNPrint.template operator()<uint16_t>(file, "binCount");

    if(formatVersion == 1)
    {
        dataType = DataType::UINT8;
        buffer.resize(batchSize);
    }
    else
    {
        size_t dataSize = 1;
        readHeaderBytes += 1;
        dataSize = readNPrint.template operator()<u_int8_t, int>(file, "dataSize");
        if(dataSize == 1)
        {
            dataType = DataType::UINT8;
            buffer.resize(batchSize);
        }
        else
        {
            dataType = DataType::FLOAT;
        }
    }

    readHeaderBytes += 1;
    readNPrint.template operator()<bool>(file, "isIQ");

    readHeaderBytes += 2;
    sampleRate = readNPrint.template operator()<uint16_t>(file, "sampleRate");

    if(formatVersion >= 3)
    {
        readHeaderBytes += 4;
        uint32_t sn = read.template operator()<uint32_t>(file);
        uint8_t* snp = (uint8_t*)&sn;

        printf("Teensy Id: %02x-%02x-%02x-%02x\n", snp[0], snp[1], snp[2], snp[3]);
        std::cout.flush();
    }
    if(formatVersion >= 4)
    {
        readHeaderBytes += 4;
        read.template operator()<uint32_t>(file); // sensor runtime at time of file creation
    }

    if(headerSize != readHeaderBytes)
        throw std::runtime_error("Either too much or too few header bytes have been read.");
}

bool FileInputStream::getNextBatch(std::vector<float>& data, size_t& timestamp)
{
    if(file.eof())
    {
        std::cerr << "Reached end of file" << std::endl;
        return false;
    }
    if(data.size() != batchSize)
        throw std::runtime_error("data array for getNextBatch has wrong size");

    if(formatVersion == 1)
    {
        return getNextBatch_version1(data, timestamp);
    }

    // all others are the same here
    return getNextBatch_version2(data, timestamp);
}

bool FileInputStream::getNextBatch_version1(std::vector<float>& data, size_t& timestamp)
{
    timestamp = read.template operator()<uint32_t>(file);

    switch(dataType)
    {
        case DataType::UINT8:
            file.read((char*)buffer.data(), batchSize);
            if(file.eof() && file.fail())
                return false;

            for(size_t i = 0; i < batchSize; i++)
                data[i] = -buffer[i];
            break;

        case DataType::FLOAT:
            file.read((char*)data.data(), batchSize * 4);
            if(file.eof() && file.fail())
                return false;
            break;
    }

    return true;
}

bool FileInputStream::getNextBatch_version2(std::vector<float>& data, size_t& timestamp)
{
    timestamp = read.template operator()<uint32_t>(file);

    auto binCount = read.template operator()<u_int32_t>(file);
    if(file.eof() && file.fail())
        return false;

    if(batchSize != binCount)
    {
        std::cerr << "Bin count does not match in batch " << currentBatchNumber << std::endl;
        return false;
    }

    switch(dataType)
    {
        case DataType::UINT8:
            file.read((char*)buffer.data(), batchSize);
            if(file.eof() && file.fail())
                return false;

            for(size_t i = 0; i < batchSize; i++)
                data[i] = -(int)buffer[i];
            break;

        case DataType::FLOAT:
            file.read((char*)data.data(), batchSize * 4);
            if(file.eof() && file.fail())
                return false;
            break;
    }

    uint32_t endMarker = read.template operator()<u_int32_t>(file);
    if(~endMarker != 0)
    {
        std::cerr << "Invalid End Marker in batch " << currentBatchNumber << std::endl;
        return false;
    }

    currentBatchNumber++;

    return true;
}

} // namespace devices
