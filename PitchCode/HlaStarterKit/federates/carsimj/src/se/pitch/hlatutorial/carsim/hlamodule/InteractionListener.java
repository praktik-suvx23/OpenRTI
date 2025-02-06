/*
 * Copyright (C) 2012  Pitch Technologies
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
package se.pitch.hlatutorial.carsim.hlamodule;


public interface InteractionListener {

   /**
    * Called when a SetScenario interaction is received by the HlaInterface
    *
    * @param scenarioName     ScenarioName parameter value
    * @param initialFuelLevel InitialFuelLevel parameter value
    */
   void receivedSetScenario(String scenarioName, int initialFuelLevel);

   /**
    * Called when a Start interaction is received by the HlaInterface
    */
   void receivedStart(float timeScaleFactor);

   /**
    * Called when a Stop interaction is received by the HlaInterface
    */
   void receivedStop();
}
