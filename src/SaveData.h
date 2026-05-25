#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <vector>

// simple struct to save the platforms
struct ObstacleData {
    float x, y, width, height;
    bool isDanger;
};

class SaveSystem {
public:
    static void Save(int highScore, int currentScore, float diff, float px, float py, float pvel, const std::vector<ObstacleData>& obs);
    static bool Load(int& highScore, bool& hasSaved, int& currentScore, float& diff, float& px, float& py, float& pvel, std::vector<ObstacleData>& obs);
};

#endif
