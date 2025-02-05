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

#include "CarFactory.h"
#include "StringUtils.h"

using namespace std;
using namespace std::experimental::filesystem;

CarFactory::CarFactory(const std::string& carConfigDir) : _carConfigDir(carConfigDir) {}

CarPtrMap CarFactory::getCars() const
{
    CarPtrMap cars;

    if (exists(_carConfigDir)) {
        int id = 0;
        directory_iterator dir_end;
        directory_iterator dir_iter(_carConfigDir);

        for (; dir_iter != dir_end; ++dir_iter, ++id) {
            path file(dir_iter->path().filename());
            string extension(file.extension().string());

            if (extension == ".car") { // only parse .car files
                string configFile = _carConfigDir.string() + "/" + file.string();
                CarProperties prop(configFile);

                if (prop.failedToLoad()) { // config file wasn't parsed correctly
                    continue;
                }

                CarPtr car(new Car(id, prop));
                cars[id] = car;
            }
        }
    }

    return cars;
}

void CarFactory::setCarConfigDir(const string& carConfigDir)
{
    _carConfigDir = path(carConfigDir);
}
