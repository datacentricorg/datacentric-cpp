#include <dc/cli/precompiled.hpp>
#include <dc/cli/commands/Run.hpp>

void SetupRun(CLI::App& app)
{
    auto opt = std::make_shared<RunOptions>();

    CLI::App* run_command = app.add_subcommand("run", "Execute handler.");
    run_command->add_option("-s,--source", opt->Source, "Source")->required();
    run_command->add_option("-e,--environment", opt->Environment, "Environment")->required();
    run_command->add_option("-d,--dataset", opt->Dataset, "Dataset")->required();
    run_command->add_option("-k,--key", opt->Key, "Key")->required();
    run_command->add_option("-t,--type", opt->Type, "Type")->required();
    run_command->add_option("-n,--handler", opt->Handler, "Handler")->required();
    run_command->add_option("-a,--arguments", opt->Arguments, "Handler arguments");

    run_command->callback([opt]() { Run(*opt); });
}

void Run(RunOptions const& opt)
{
    std::cout << "Source: " << opt.Source << std::endl;
    std::cout << "Environment: " << opt.Environment << std::endl;
    std::cout << "Dataset: " << opt.Dataset << std::endl;
    std::cout << "Key: " << opt.Key << std::endl;
    std::cout << "Type: " << opt.Type << std::endl;
    std::cout << "Handler: " << opt.Handler << std::endl;
    for (auto& arg : opt.Arguments)
    {
        std::cout << "Argument: " << arg << std::endl;
    }
}
