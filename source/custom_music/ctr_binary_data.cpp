#include <cstring>
#include "ctr_binary_data.hpp"

// Handler

BinaryDataHandler::BinaryDataHandler() {
}

BinaryDataHandler::~BinaryDataHandler() = default;

bool BinaryDataHandler::SuccessfullyInitialized() {
    return init;
}

// Reader

BinaryDataReader::BinaryDataReader(FS_Archive archive_, std::string filePath_) {
    if (!R_SUCCEEDED(FSUSER_OpenFile(&fsHandle, archive_, fsMakePath(PATH_ASCII, filePath_.c_str()), FS_OPEN_READ,
                                     FS_ATTRIBUTE_ARCHIVE))) {
        return;
    }
    FSFILE_GetSize(fsHandle, &fileSize);

    rawBytes.resize(fileSize);
    FSFILE_Read(fsHandle, NULL, 0, rawBytes.data(), fileSize);
    FSFILE_Close(fsHandle);

    init = true;
}

BinaryDataReader::~BinaryDataReader() = default;

u64 BinaryDataReader::GetFileSize() {
    return fileSize;
}

u8 BinaryDataReader::ReadByte() {
    u8 value = *(u8*)(rawBytes.data() + position);
    position += sizeof(u8);

    return value;
}

std::vector<char> BinaryDataReader::ReadChars(u32 count) {
    std::vector<char> chars(count, 0);

    memcpy(chars.data(), rawBytes.data() + position, count);
    position += count;

    return chars;
}

std::vector<u8> BinaryDataReader::ReadBytes(u32 count) {
    std::vector<u8> bytes(count, 0);

    memcpy(bytes.data(), rawBytes.data() + position, count);
    position += count;

    return bytes;
}

s16 BinaryDataReader::ReadS16() {
    s16 value = *(s16*)(rawBytes.data() + position);
    position += sizeof(s16);

    return value;
}

u16 BinaryDataReader::ReadU16() {
    return (u16)ReadS16();
}

s32 BinaryDataReader::ReadS32() {
    s32 value = *(s32*)(rawBytes.data() + position);
    position += sizeof(s32);

    return value;
}

u32 BinaryDataReader::ReadU32() {
    return (u32)ReadS32();
}

std::vector<u8> BinaryDataReader::ReadAll() {
    position = 0;
    return rawBytes;
}

// Writer

BinaryDataWriter::BinaryDataWriter(u32 expectedSize) {
    rawBytes.resize(expectedSize, 0);
    init = true;
}

BinaryDataWriter::~BinaryDataWriter() = default;

void BinaryDataWriter::Write(char buffer) {
    Write((u8)buffer);
}

void BinaryDataWriter::Write(u8 buffer) {
    SizeCheck(sizeof(buffer));
    *(u8*)(rawBytes.data() + position) = buffer;
    UpdatePosition(sizeof(buffer));
}

void BinaryDataWriter::Write(std::vector<char>& buffer) {
    SizeCheck(buffer.size());
    memcpy(rawBytes.data() + position, buffer.data(), buffer.size());
    UpdatePosition(buffer.size());
}

void BinaryDataWriter::Write(std::vector<u8>& buffer) {
    SizeCheck(buffer.size());
    memcpy(rawBytes.data() + position, buffer.data(), buffer.size());
    UpdatePosition(buffer.size());
}

void BinaryDataWriter::Write(s16 buffer) {
    SizeCheck(sizeof(buffer));
    *(s16*)(rawBytes.data() + position) = buffer;
    UpdatePosition(sizeof(buffer));
}

void BinaryDataWriter::Write(u16 buffer) {
    Write((s16)buffer);
}

void BinaryDataWriter::Write(s32 buffer) {
    SizeCheck(sizeof(buffer));
    *(s32*)(rawBytes.data() + position) = buffer;
    UpdatePosition(sizeof(buffer));
}

void BinaryDataWriter::Write(u32 buffer) {
    Write((s32)buffer);
}

void BinaryDataWriter::CreateFile(FS_Archive archive_, std::string filePath_) {
    if (!R_SUCCEEDED(FSUSER_OpenFile(&fsHandle, archive_, fsMakePath(PATH_ASCII, filePath_.c_str()),
                                     FS_OPEN_WRITE | FS_OPEN_CREATE, FS_ATTRIBUTE_ARCHIVE))) {
        return;
    }
    u32 bytesWritten = 0;
    FSFILE_Write(fsHandle, &bytesWritten, 0, rawBytes.data(), totalSize, FS_WRITE_UPDATE_TIME);
    FSFILE_Close(fsHandle);
}

void BinaryDataWriter::SizeCheck(u32 bufferSize) {
    if (position + bufferSize > rawBytes.size()) {
        // Reallocation required
        rawBytes.resize(position + bufferSize);
    }
}

void BinaryDataWriter::UpdatePosition(u32 newPos) {
    position += newPos;
    if (totalSize < position) {
        totalSize = position;
    }
}
