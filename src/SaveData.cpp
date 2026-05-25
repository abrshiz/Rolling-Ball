#include "SaveData.h"
#include <fstream>
#include <iostream>

// Struct for the file header
struct SaveHeader {
    char magic[4]; // to make sure it's our save file
    int version;
    int highScore;
    bool hasSavedGame;
    int currentScore;
    float difficultyMultiplier;
    float playerX;
    float playerY;
    float playerVelocity;
    int obstacleCount;
};

void SaveSystem::Save(int highScore, int currentScore, float diff, float px, float py, float pvel, const std::vector<ObstacleData>& obs) {
    // writing binary so people can't edit it with notepad easily
    std::ofstream out("save.dat", std::ios::binary);
    if (!out.is_open()) return;

    SaveHeader header = {
        {'S','A','V','E'},
        3, // version 3 
        highScore,
        true,
        currentScore,
        diff,
        px,
        py,
        pvel,
        (int)obs.size()
    };

    // dump header
    out.write(reinterpret_cast<const char*>(&header), sizeof(SaveHeader));
    
    // dump all obstacles
    for(const auto& o : obs) {
        out.write(reinterpret_cast<const char*>(&o), sizeof(ObstacleData));
    }
    
    out.close();
}

bool SaveSystem::Load(int& highScore, bool& hasSaved, int& currentScore, float& diff, float& px, float& py, float& pvel, std::vector<ObstacleData>& obs) {
    std::ifstream in("save.dat", std::ios::binary);
    if (!in.is_open()) {
        // no save file yet
        highScore = 0;
        hasSaved = false;
        return false;
    }

    SaveHeader header;
    in.read(reinterpret_cast<char*>(&header), sizeof(SaveHeader));

    // check magic bytes
    if (in.gcount() != sizeof(SaveHeader) || header.magic[0] != 'S' || header.magic[1] != 'A' || header.magic[2] != 'V' || header.magic[3] != 'E') {
        highScore = 0;
        hasSaved = false;
        return false;
    }

    // if structure changed, ignore old saves
    if (header.version != 3) {
        highScore = 0;
        hasSaved = false;
        return false;
    }

    // copy data out
    highScore = header.highScore;
    hasSaved = header.hasSavedGame;
    currentScore = header.currentScore;
    diff = header.difficultyMultiplier;
    px = header.playerX;
    py = header.playerY;
    pvel = header.playerVelocity;

    obs.clear();
    for(int i=0; i<header.obstacleCount; ++i) {
        ObstacleData o;
        in.read(reinterpret_cast<char*>(&o), sizeof(ObstacleData));
        obs.push_back(o);
    }
    
    in.close();
    return true;
}
