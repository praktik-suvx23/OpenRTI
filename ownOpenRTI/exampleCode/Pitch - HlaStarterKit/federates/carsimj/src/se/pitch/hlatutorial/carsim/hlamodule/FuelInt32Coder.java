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

import hla.rti1516e.encoding.DecoderException;
import hla.rti1516e.encoding.EncoderFactory;
import hla.rti1516e.encoding.HLAinteger32BE;


class FuelInt32Coder {

   private final HLAinteger32BE _coder;

   FuelInt32Coder(EncoderFactory factory) {
      _coder = factory.createHLAinteger32BE();
   }

   byte[] encode(int fuelLevel) {
      _coder.setValue(fuelLevel);
      return _coder.toByteArray();
   }

   int decode(byte[] fuelInt) throws DecoderException {
      _coder.decode(fuelInt);
      return _coder.getValue();
   }
}
