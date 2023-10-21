#pragma once

#include <3ds.h>
#include <string>
#include <vector>

class BinaryDataHandler {
  public:
    bool SuccessfullyInitialized();

    u32 position = 0;

  protected:
    BinaryDataHandler();
    ~BinaryDataHandler();

    bool init = false;

    Handle fsHandle = 0;
    std::vector<u8> rawBytes;
};

class BinaryDataReader : public BinaryDataHandler {
  public:
    BinaryDataReader(FS_Archive archive_, std::string filePath_);
    ~BinaryDataReader();

    u64 GetFileSize();

    u8 ReadByte();
    std::vector<char> ReadChars(u32 count);
    std::vector<u8> ReadBytes(u32 count);
    s16 ReadS16();
    u16 ReadU16();
    s32 ReadS32();
    u32 ReadU32();

    std::vector<u8> ReadAll();

  private:
    u64 fileSize;
};

class BinaryDataWriter : public BinaryDataHandler {
  public:
    BinaryDataWriter(u32 expectedSize);
    ~BinaryDataWriter();

    void Write(char buffer);
    void Write(u8 buffer);
    void Write(std::vector<char>& buffer);
    void Write(std::vector<u8>& buffer);
    void Write(s16 buffer);
    void Write(u16 buffer);
    void Write(s32 buffer);
    void Write(u32 buffer);

    /// Opens file, writes rawBytes to it, and closes file immediately after.
    void CreateFile(FS_Archive archive_, std::string filePath_);

  private:
    /// If written bytes will exceed rawBytes size, reallocate and resize.
    void SizeCheck(u32 bufferSize);
    /// Updates position cursor, and updates totalSize if passed by position.
    void UpdatePosition(u32 newPos);

    u32 totalSize = 0;
};
