#pragma once

#include <iostream>
#include <optional>

#include "server/request_handler.hpp"

std::unique_ptr<server::request_handler> request_handler;

int main(int argc, char** argv);

void on_shutdown();
void help();

std::optional<std::string> parse_arg(std::string const& args, std::string arg_name);