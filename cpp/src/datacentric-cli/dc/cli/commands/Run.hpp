/*
Copyright (C) 2013-present The DataCentric Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <cli/CLI11.hpp>

struct run_options
{
    std::string source;
    std::string environment;
    std::string dataset;
    std::string key;
    std::string type;
    std::string handler;
    std::vector<std::string> arguments;
};

void setup_run(CLI::App& app);
void run(run_options const& opt);
