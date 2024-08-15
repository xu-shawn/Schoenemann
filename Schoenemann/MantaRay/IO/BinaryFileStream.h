//
// Copyright (c) 2023 MantaRay authors. See the list of authors for more details.
// Licensed under MIT.
//

#ifndef MANTARAY_BINARYFILESTREAM_H
#define MANTARAY_BINARYFILESTREAM_H

#include <array>
#include <cstdint>
#include <sstream>  // Include for std::stringstream
#include "DataStream.h"

namespace MantaRay
{

    /// \brief A binary file stream.
    /// \details This class is a wrapper around std::fstream that provides a binary view of the file or an in-memory data stream.
    class BinaryFileStream : public DataStream<std::ios::binary | std::ios::in>
    {

    private:
        std::string Path;
        std::stringstream MemoryStream;

    public:
        /// \brief The BinaryFileStream constructor for file paths.
        /// \param path The path to the file.
        /// \details This constructor opens the file in binary read mode.
        __attribute__((unused)) explicit BinaryFileStream(const std::string& path)
            : DataStream(path), Path(path)  // Initialize base class and Path
        {
        }

        /// \brief The BinaryFileStream constructor for in-memory data.
        /// \param data Pointer to the binary data.
        /// \param length The length of the binary data.
        /// \details This constructor initializes the stream from in-memory binary data.
        BinaryFileStream(const unsigned char* data, size_t length)
            : DataStream(""), Path("") // Initialize base class with an empty string since no file is used
        {
            MemoryStream.write(reinterpret_cast<const char*>(data), length);
            MemoryStream.seekg(0); // Rewind to start
        }

        /// \brief Convert the stream to write mode.
        /// \details This function closes the stream and reopens it in write mode.
        void WriteMode()
        {
            if (!Path.empty()) {
                // Weird C++ issue workaround for file-based operations.
                this->Stream.close();
                this->Stream.open(Path, std::ios::binary | std::ios::out);
            }
            else {
                // Reset the memory stream for writing (not typical, but here for completeness).
                MemoryStream.clear(); // Clear any error flags
                MemoryStream.seekp(0); // Rewind to start
            }
        }

        /// \brief Read an array from the stream.
        /// \tparam T The type of the array.
        /// \tparam Size The size of the array.
        /// \param array The array to read into.
        /// \details This function reads an array from the stream, either from a file or from memory.
        template<typename T, size_t Size>
        void ReadArray(std::array<T, Size>& array)
        {
            if (!Path.empty()) {
                // If a path is provided, read from file
                this->Stream.read(reinterpret_cast<char*>(&array), sizeof(array));
            }
            else {
                // Otherwise, read from memory
                MemoryStream.read(reinterpret_cast<char*>(&array), sizeof(array));
            }
        }

        /// \brief Write an array to the stream.
        /// \tparam T The type of the array.
        /// \tparam Size The size of the array.
        /// \param array The array to write.
        /// \details This function writes an array to the stream, either to a file or to memory.
        template<typename T, size_t Size>
        void WriteArray(const std::array<T, Size>& array)
        {
            if (!Path.empty()) {
                // If a path is provided, write to file
                this->Stream.write(reinterpret_cast<const char*>(&array), sizeof(array));
            }
            else {
                // Otherwise, write to memory
                MemoryStream.write(reinterpret_cast<const char*>(&array), sizeof(array));
            }
        }

    };

} // namespace MantaRay

#endif // MANTARAY_BINARYFILESTREAM_H
