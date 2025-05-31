#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/chrono.hpp>

namespace SerializationUtils {

    /**
     * Serializes any Cereal-compatible object to a binary string
     *
     * @tparam T Type of object to serialize
     * @param obj The object to serialize
     * @return std::string Serialized binary data
     */
    template<typename T>
    std::string serialize(const T& obj) {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive archive(ss);
            archive(obj);
        }
        return ss.str();
    }

    /**
     * Deserializes a binary string to an object of type T
     *
     * @tparam T Type to deserialize into
     * @param data Binary serialized data
     * @param size Size of the data in bytes
     * @return T The deserialized object
     */
    template<typename T>
    T deserialize(const char* data, size_t size) {
        T obj;
        std::stringstream ss;
        ss.write(data, size);
        {
            cereal::BinaryInputArchive archive(ss);
            archive(obj);
        }
        return obj;
    }

    /**
     * Deserializes a binary string into an existing object
     *
     * @tparam T Type to deserialize into
     * @param data Binary serialized data
     * @param size Size of the data in bytes
     * @param obj Reference to object to populate
     * @return bool True if deserialization was successful
     */
    template<typename T>
    bool deserialize_into(const char* data, size_t size, T& obj) {
        try {
            std::stringstream ss;
            ss.write(data, size);
            {
                cereal::BinaryInputArchive archive(ss);
                archive(obj);
            }
            return true;
        }
        catch (const std::exception& e) {
            // Handle deserialization error
            return false;
        }
    }

    /**
     * Deserializes a binary string into an existing object
     *
     * @tparam T Type to deserialize into
     * @param serialized_data Binary serialized data as a string
     * @param obj Reference to object to populate
     * @return bool True if deserialization was successful
     */
    template<typename T>
    bool deserialize_into(const std::string& serialized_data, T& obj) {
        return deserialize_into<T>(serialized_data.c_str(), serialized_data.size(), obj);
    }

    /**
     * Serializes a vector of objects
     *
     * @tparam T Type of objects in the vector
     * @param objects Vector of objects to serialize
     * @return std::string Serialized binary data
     */
    template<typename T>
    std::string serialize_vector(const std::vector<T>& objects) {
        return serialize(objects);
    }

    /**
     * Deserializes a binary string to a vector of objects
     *
     * @tparam T Type of objects in the vector
     * @param data Binary serialized data
     * @param size Size of the data in bytes
     * @return std::vector<T> Vector of deserialized objects
     */
    template<typename T>
    std::vector<T> deserialize_vector(const char* data, size_t size) {
        return deserialize<std::vector<T>>(data, size);
    }

    /**
     * Deserializes a binary string to a vector of objects
     *
     * @tparam T Type of objects in the vector
     * @param serialized_data Binary serialized data as a string
     * @return std::vector<T> Vector of deserialized objects
     */
    template<typename T>
    std::vector<T> deserialize_vector(const std::string& serialized_data) {
        return deserialize<std::vector<T>>(serialized_data);
    }
}

