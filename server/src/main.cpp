#include "main.hpp"

int main(int argc, char** argv)
{
    std::string args_str;
    for (int i = 1; i < argc; i++)
    {
        args_str += argv[i];

        if (i + 1 < argc)
        {
            args_str += " ";
        }
    }

    auto port = parse_arg(args_str, "port");
    if (port == std::nullopt)
    {
        help();
        return 1;
    };

    auto connstring = parse_arg(args_str, "connstring");
    if (connstring == std::nullopt)
    {
        help();
        return 1;
    }

    server::config::set_conn_string(connstring.value());

    std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;
    const auto localhost_listener = U("http://localhost:") + converter.from_bytes(port.value()) + U("/");
    
    request_handler = std::make_unique<server::request_handler>(localhost_listener);
    request_handler->open().wait();

    ucout << utility::string_t(U("Listening for requests at: ")) << localhost_listener << std::endl;
    std::cout << "Press ENTER to exit." << std::endl;

    std::string line;
    std::getline(std::cin, line);

    on_shutdown();

    return 0;
}

std::optional<std::string> parse_arg(std::string const& args, std::string arg_name)
{
    const auto arg_pos = args.find(arg_name);
    if (arg_pos == std::string::npos)
    {
        return std::nullopt;
    }

    const auto equals_pos = args.find("=", arg_pos);
    if (equals_pos == std::string::npos)
    {
        return std::nullopt;
    }

    const auto first_quotation = args.find("<", equals_pos);
    if (first_quotation == std::string::npos)
    {
        return std::nullopt;
    }

    const auto second_quotation = args.find(">", first_quotation);    
    if (second_quotation == std::string::npos)
    {
        return std::nullopt;
    }

    std::string_view result;
    result = args;
    return std::string(result.substr(first_quotation + 1, second_quotation - first_quotation - 1));
}

void help()
{
    std::cout << " Use --port=<uint> --connstring=<str>";
}

void on_shutdown()
{
    request_handler->close().wait();
}
