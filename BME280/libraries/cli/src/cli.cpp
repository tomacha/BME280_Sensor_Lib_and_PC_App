/**
 * @file cli.cpp
 * @authors Krzysztof Półchłopek, Michał Tomacha
 * @version 0.1
 * @date 2022-06-05
 */

#include "cli.hpp"
#include <cstring>

#define CLI_LOGS_EN

#ifdef CLI_LOGS_EN
#define LOG(_msg) this->_write((uint8_t*) _msg, sizeof(_msg))
#else
#define LOG(_msg)
#endif 



namespace CLI
{
    cli_status cli::_find_command(std::string *command, cli_command_handler_t *handler)
    {
        for (auto cmd : *this->commands)
        {
            if (cmd.command == *command)
            {
                *handler = cmd.cmd_handler;
                return STATUS_OK;
            }
        }
        return STATUS_ERROR;
    }

    cli::cli(cli_status (*write_fp) (uint8_t *, uint32_t), std::vector<cli_command_t> *commands, cli_process_mode mode)
    {
        this->_write = write_fp;

        this->commands = commands;
        this->command_buff.clear();

        this->working_mode = mode;
    }

    cli::~cli()
    {
    }

    cli_status cli::cli_receiveChar(char c)
    {

        if ((c == '\n') || (c == '\r'))
        {
            this->command_buff.push_back('\0');
            // check if this command is empty
            if (this->command_buff.length() < 2)
            {
                this->command_buff.clear();
            }
            else
            {
                this->cmd_pending_number += 1;

                LOG("\n\r");

                if (this->working_mode == MODE_AUTO)
                {
                    this->cli_processCommand();
                }
            }
        }
        else
        {
            this->command_buff.push_back(c);
            // LOG(&c);
        }

        return STATUS_OK;
    }

    cli_status cli::cli_processCommand(void)
    {
        
        cli_status status;
        cli_command_handler_t cmd_handler = NULL;

        std::string command;
        std::vector<std::string> argv;

        if (this->cmd_pending_number > 0)
        {
            command.clear();
            // Get command from queue
            size_t pos = this->command_buff.find('\0');           
            command = this->command_buff.substr(0, pos);
            this->command_buff.erase(0, pos + 1);

            // Create vector of arguments
            do
            {
                pos = command.find(' ');
                if (pos == std::string::npos)
                {
                    pos = command.find('\0');
                }
                
                argv.push_back(command.substr(0, pos));
                command.erase(0,pos + 1);
            } while (pos != std::string::npos);

            status = _find_command(&argv[0], &cmd_handler);
            if (status != STATUS_OK)
            {   
                LOG("Could not find command\n\r");
                this->_write((uint8_t*) argv[0].data(), argv[0].length());
                LOG("\n\r");
            }
            else
            {
                this->_write((uint8_t*) "cli_processCommand\n\r", sizeof("cli_processCommand\n\r"));
                status = cmd_handler(&argv);
                if (status == STATUS_OK)
                {
                    this->_write((uint8_t*) "CLI Command_OK\n", sizeof("CLI Command_OK\n") - 1);
                }
                else
                {
                    this->_write((uint8_t*) "CLI Command_ERROR\n", sizeof("CLI Command_ERROR\n") - 1);
                }
                
            }
            this->cmd_pending_number -= 1;
        }
        return STATUS_OK;
    }

} // namespace CLI

