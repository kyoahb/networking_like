#pragma once

#include "Utils/Imports/common.h"

// Static class that allows model ids to be converted to .obj files locally
// Also speeds up loading of models by preloading them
class AssetManager {
public:
	
	static void loadModels() {
		models_loaded = true;
		models[1] = LoadModel("files/Assets/cube.obj"); // 1 is the default model
	}

	static raylib::Model& getModel(int id) {
		if (!models_loaded) loadModels();

		auto it = models.find(id);
		if (it != models.end()) {
			return it->second;
		}
		else {
			return getDefaultModel();
		}
	}

	static raylib::Model& getDefaultModel() {
		if (!models_loaded) loadModels();
		return models[1];
	}
	
private:
	static bool models_loaded;
	static std::unordered_map<int, raylib::Model> models;
};