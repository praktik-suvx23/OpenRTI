#ifndef ACTIVEMISSILE_H
#define ACTIVEMISSILE_H

#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>

/*
    This is only a proof of concept for a missile class that can be used in the MissileManagerFederate.
    The ActiveMissile class is suppose to be called when a ship wants to fire a missile.

    TODO:
    - Better flight path.
        - Add altitude in the calculation
        - Add more parameters to the constructor. Use all HLAinteractionRoot.FireMissile parameters
        - Add flight path. An array with X number of coordinates to follow?
    - Add some sort of ship size, so the missile knows what kind of ship to look for when locking on.
    - Add a lock on distance, so the missile knows when to lock on to the target.
    - Add some sort of sensor to detect targets when flying.
    - Add max travel distance for the missile.

    Discuss:
    - How should the missiles be handled? 
        - Should they calculate a time to reach target and that's that? (Might start like that, just for POC)
        - Should the missile update it's position during flight?
        - Should the missile get a set path to target?
        - Should the missile be a separate class or a method in the MissileManagerFederate?

    EXTRA:
    Look at comment in 'MissileManagerHelper.h'. Point 3.1
*/

class ClassActiveMissile {
    int id;
    double startX, startY;
    double targetX, targetY;
    double speed;
    std::atomic<bool> isFlying;
    std::thread missileThread;

    void fly() {
        std::cout << "Missile " << id << " launched from (" << startX << ", " << startY << ") to (" << targetX << ", " << targetY << ") at speed " << speed << std::endl;
        // Improve this! Use speed and distance to calculate time to target.
        // This is just a simple example.
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        isFlying = false;
        std::cout << "Missile " << id << " reached target (" << targetX << ", " << targetY << ")" << std::endl;
    }

public:
    // Constructor
    ClassActiveMissile(int id, double startX, double startY, double targetX, double targetY, double speed)
        : id(id), startX(startX), startY(startY), targetX(targetX), targetY(targetY), speed(speed), isFlying(true) {}

    /* Move constructor. Required for preformance reasons. 
        Transfer the vector object instead of copying.*/
    ClassActiveMissile(ClassActiveMissile&& other) noexcept
        : id(other.id), startX(other.startX), startY(other.startY), targetX(other.targetX), targetY(other.targetY), speed(other.speed), isFlying(other.isFlying.load()), missileThread(std::move(other.missileThread)) {}

    // Move assignment operator
    ClassActiveMissile& operator=(ClassActiveMissile&& other) noexcept {
        if (this != &other) {
            id = other.id;
            startX = other.startX;
            startY = other.startY;
            targetX = other.targetX;
            targetY = other.targetY;
            speed = other.speed;
            isFlying = other.isFlying.load();
            missileThread = std::move(other.missileThread);
        }
        return *this;
    }

    // Delete copy constructor and copy assignment operator
    ClassActiveMissile(const ClassActiveMissile&) = delete;
    ClassActiveMissile& operator=(const ClassActiveMissile&) = delete;

    void launch() {
        missileThread = std::thread(&ClassActiveMissile::fly, this);
    }

    void join() {
        if (missileThread.joinable()) {
            missileThread.join();
        }
    }

    bool isFlyingStatus() const {
        return isFlying.load();
    }
};

#endif