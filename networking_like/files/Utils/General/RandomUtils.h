#pragma once
#include "Utils/Imports/common.h"

class RandomUtils {
public:
	static int random_int(int min, int max) {
		return min + rand() % (max - min + 1);
	};

	static float random_float(float min, float max) {
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	};

    static std::string random_string(size_t length, bool use_lowercase = true, bool use_uppercase = true, bool use_numbers = true) {
        // Predefined character sets
        static constexpr char lowercase[] = "abcdefghijklmnopqrstuvwxyz";
        static constexpr char uppercase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        static constexpr char numbers[] = "0123456789";

        // Cache for each combination (2^3 = 8)
        struct CharsetCache {
            std::string charset;
            bool valid = false;
        };
        static CharsetCache caches[8];

        int cache_index = (use_lowercase ? 1 : 0) | (use_uppercase ? 2 : 0) | (use_numbers ? 4 : 0);
        CharsetCache& cache = caches[cache_index];

        if (!cache.valid) {
            cache.charset.clear();
            if (use_lowercase) cache.charset += lowercase;
            if (use_uppercase) cache.charset += uppercase;
            if (use_numbers)   cache.charset += numbers;
            cache.valid = true;
        }

        const std::string& charset = cache.charset;
        if (charset.empty() || length == 0)
            return "";

        std::string result(length, '\0');
        for (size_t i = 0; i < length; ++i) {
            result[i] = charset[rand() % charset.size()];
        }
        return result;
    }

};