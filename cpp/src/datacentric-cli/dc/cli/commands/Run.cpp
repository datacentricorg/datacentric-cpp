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

#include <dc/cli/precompiled.hpp>
#include <dc/cli/commands/run.hpp>

void setup_run(CLI::App& app)
{
    auto opt = std::make_shared<run_options>();

    CLI::App* run_command = app.add_subcommand("run", "Execute handler.");
    run_command->add_option("-s,--source", opt->source, "source")->required();
    run_command->add_option("-e,--environment", opt->environment, "environment")->required();
    run_command->add_option("-d,--dataset", opt->dataset, "dataset")->required();
    run_command->add_option("-k,--key", opt->key, "key")->required();
    run_command->add_option("-t,--type", opt->type, "type")->required();
    run_command->add_option("-n,--handler", opt->handler, "handler")->required();
    run_command->add_option("-a,--arguments", opt->arguments, "handler arguments");

    run_command->callback([opt]() { run(*opt); });
}

void run(run_options const& opt)
{
    std::cout << "source: " << opt.source << std::endl;
    std::cout << "environment: " << opt.environment << std::endl;
    std::cout << "dataset: " << opt.dataset << std::endl;
    std::cout << "key: " << opt.key << std::endl;
    std::cout << "type: " << opt.type << std::endl;
    std::cout << "handler: " << opt.handler << std::endl;
    for (auto& arg : opt.arguments)
    {
        std::cout << "argument: " << arg << std::endl;
    }
}
