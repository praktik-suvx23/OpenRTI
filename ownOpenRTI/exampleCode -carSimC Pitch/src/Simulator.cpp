/*
 * Copyright (C) 2012-2021  Pitch Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

#include "Simulator.h"
#include "StringUtils.h"


using namespace std;
using Milliseconds = std::chrono::duration<double, std::milli>;
using Clock = std::chrono::steady_clock;


Simulator::Simulator(const SimulatorConfig& configuration) 
    : _frameRate(configuration.getFrameRate()), 
    _scenarioDir(configuration.getScenarioDir()),
    _timeScaleFactor(1.0),
    _simulating(false)
{ 
	/* void */ 
}

Simulator::~Simulator()
{ 
    /* void */ 
}

void Simulator::simulate()
{
    if (_frameRate <= 0) {
        throw (new runtime_error("Framerate should be a positive integer. Check simulator configuration."));
    }

    Milliseconds totalLoopTime = Milliseconds(1000) / _frameRate;
    Milliseconds advanceTimeStep;
    {
        std::unique_lock<std::mutex> lock(_timeScaleFactorMutex);
        advanceTimeStep = totalLoopTime * _timeScaleFactor;
    }

    std::unique_lock<std::mutex> lock(_simLoopMutex); 
    while (isSimulating()) {
        auto startTime = Clock::now();

        advanceTime(static_cast<unsigned int>(advanceTimeStep.count()));

        // notify the listeners
        SimulatorListenerSet::iterator iter(_simulatorListeners.begin());
        for (; iter != _simulatorListeners.end(); ++iter) {
            (*iter)->updatedState(this);
        }

        Milliseconds elapsedTime = Clock::now() - startTime;
        Milliseconds sleepTime = totalLoopTime - elapsedTime;
        
        if (sleepTime.count() > 0) {
            this_thread::sleep_for(sleepTime);
        }
    }
}

void Simulator::advanceTime(unsigned int timeStep)
{ 
    std::unique_lock<std::mutex> lock(_carsMutex);
    // really big counter so we shouldn't have to worry about wrapping
    _scenarioTime += timeStep; 
        
    CarPtrMap::iterator iter(_cars.begin()); 
    for (; iter != _cars.end(); ++iter) {
        (iter->second)->drive(timeStep, _scenario->getEndPosition());
    }
}

void Simulator::loadScenario(ScenarioPtr scenario, double initialFuelLevel)
{	
    {
        std::unique_lock<std::mutex> lock(_carsMutex);

        // Reset all Cars' states according to the scenario
        CarPtrMap::iterator iter(_cars.begin()); 
        for (; iter != _cars.end(); ++iter) { 
            (iter->second)->setFuelLevel(initialFuelLevel);
            (iter->second)->setPosition(scenario->getStartPosition());
            (iter->second)->setDriving(true);
        }
        _scenarioTime = 0;
    }

    std::unique_lock<std::mutex> lock(_scenarioMutex);
    _scenario = scenario;
}

bool Simulator::isSimulating() const
{
    std::unique_lock<std::mutex> lock(_simulatingMutex);
    return _simulating;
}

void Simulator::setSimulating(bool shouldSimulate)
{
    {
        std::unique_lock<std::mutex> lock(_simulatingMutex);
        _simulating = shouldSimulate;
    }

    {
        std::unique_lock<std::mutex> lock(_threadMutex);
        if (shouldSimulate) {
            if (_thread && _thread->joinable()) { // already running
                return;
            } else {
                _thread = ThreadPtr(new std::thread(&Simulator::simulate, this));
            }
        
        } else if (_thread) {
            _thread->join(); 
        }
    }
}

bool Simulator::loadScenario(const std::wstring& scenario, double initialFuelLevel)
{
    ScenarioPtr ptr(new Scenario(_scenarioDir + "/" + StringUtils::wstr2str(scenario) + ".scenario"));
    if (ptr->failedToLoad()) {
        return false;
    } else {
        loadScenario(ptr, initialFuelLevel);
        return true;
    }
}

bool Simulator::hasScenario() const
{ 
    std::unique_lock<std::mutex> lock(_scenarioMutex);
    return (_scenario? true : false);
}

void Simulator::addListener(SimulatorListener* listener)
{
    std::unique_lock<std::mutex> lock(_listenersMutex);
    _simulatorListeners.insert(listener);
}

void Simulator::removeListener(SimulatorListener* listener)
{
    std::unique_lock<std::mutex> lock(_listenersMutex);
    _simulatorListeners.erase(listener);
}

void Simulator::addCars(const CarPtrMap& cars)
{ 
    std::unique_lock<std::mutex> lock(_carsMutex);
    
    CarPtrMap::const_iterator car_iter(cars.begin());
    for (; car_iter != cars.end(); ++car_iter) {
        int id(car_iter->first);
        CarPtr car(new Car(*(car_iter->second)));
        _cars[id] = car;
    }
}

CarPtrMap Simulator::getCars()
{
    std::unique_lock<std::mutex> lock(_carsMutex);
    return _cars; // copy the map
}

CarPtr Simulator::getCar(int id)
{
    std::unique_lock<std::mutex> lock(_carsMutex);
    return _cars[id];
}

void Simulator::setTimeScaleFactor(double factor)
{
    std::unique_lock<std::mutex> lock(_timeScaleFactorMutex);
    _timeScaleFactor = factor;
}
