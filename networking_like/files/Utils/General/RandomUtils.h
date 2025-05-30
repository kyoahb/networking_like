#include <random>

class RandomUtils {
public:
    // Get a static random engine (seeded once)
    static std::mt19937& engine() {
        static std::random_device rd;
        static std::mt19937 eng(rd());
        return eng;
    }

    static int random_int(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine());
    }

    static float random_float(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(engine());
    }

    static std::string random_string(size_t length, bool use_lowercase = true, bool use_uppercase = true, bool use_numbers = true) {
        static constexpr char lowercase[] = "abcdefghijklmnopqrstuvwxyz";
        static constexpr char uppercase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        static constexpr char numbers[] = "0123456789";

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

        std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);
        std::string result(length, '\0');
        for (size_t i = 0; i < length; ++i) {
            result[i] = charset[dist(engine())];
        }
        return result;
    }
};
