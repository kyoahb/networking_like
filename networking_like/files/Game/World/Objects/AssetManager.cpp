#include "AssetManager.h"

bool AssetManager::models_loaded = false;
std::unordered_map<int, raylib::Model> AssetManager::models = {};