#ifndef TEMPSTRUCT_H
#define TEMPSTRUCT_H

#include <string>

enum class Team {
    UNSIGNED,
    Blue,
    Red
};

struct ConfirmHandshake {
    std::wstring shooterID;
    int32_t missilesLoaded;
    bool shooterReceivedOrder;
    std::wstring targetID;
    int32_t missilesLocked;
};

struct InitialHandshake {
    std::wstring shooterID;
    int32_t missilesLoaded;
    std::wstring targetID;
    int32_t maxMissilesRequired;
    int32_t missilesCurrentlyTargeting;
    double distanceToTarget;

    bool operator==(const InitialHandshake& other) const {
        return shooterID == other.shooterID &&
               missilesLoaded == other.missilesLoaded &&
               targetID == other.targetID &&
               maxMissilesRequired == other.maxMissilesRequired &&
               missilesCurrentlyTargeting == other.missilesCurrentlyTargeting &&
               distanceToTarget == other.distanceToTarget;
    }
};

inline void hash_combine(std::size_t& seed, std::size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std {
    template <>
    struct hash<InitialHandshake> {
        std::size_t operator()(const InitialHandshake& hs) const {
            std::size_t seed = 0;
            hash_combine(seed, std::hash<std::wstring>()(hs.shooterID));
            hash_combine(seed, std::hash<int32_t>()(hs.missilesLoaded));
            hash_combine(seed, std::hash<std::wstring>()(hs.targetID));
            hash_combine(seed, std::hash<int32_t>()(hs.maxMissilesRequired));
            hash_combine(seed, std::hash<int32_t>()(hs.missilesCurrentlyTargeting));
            hash_combine(seed, std::hash<double>()(hs.distanceToTarget));
            return seed;
        }
    };
}



#endif // TEMPSTRUCT_H