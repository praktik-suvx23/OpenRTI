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
#include "StringUtils.h"
#include <iostream>

std::wstring StringUtils::str2wstr(const std::string& s)
{
        return std::wstring(s.begin(), s.end());
}

std::string StringUtils::wstr2str(const std::wstring& s)
{
        return std::string(s.begin(), s.end());
}


