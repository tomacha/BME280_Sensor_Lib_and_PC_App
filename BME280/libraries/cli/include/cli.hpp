/**
 * @file cli.hpp
 * @authors Krzysztof Półchłopek, Michał Tomacha
 * @version 0.1
 * @date 2022-06-05
 * 
 * @brief Library for handling communcation with PC
 */

#ifndef _CLI_HPP_
#define _CLI_HPP_

#include <cstdint>
#include <vector>
#include <string>

#define CLI_COMMAND_END_CHAR        ('\n')
#define CLI_COMMAND_MAX_ARGS        8

namespace CLI
{
    enum cli_status
    {
        STATUS_OK           = 0,
        STATUS_ERROR        = 1,

        STATUS_TOP
    };

    enum cli_process_mode
    {
        MODE_FORCED         = 0,
        MODE_AUTO           = 1,

        MODE_TOP
    };

    typedef cli_status (*cli_command_handler_t) (std::vector<std::string> *argv);

    typedef struct 
    {
        const std::string command;
        cli_command_handler_t cmd_handler;
    } cli_command_t;
    
    class cli
    {
    private:
        cli_process_mode working_mode;

        std::string command_buff;
        size_t cmd_pending_number;
        std::vector<cli_command_t> *commands;
        
        cli_status (* _write) (uint8_t *data, uint32_t data_len);

        cli_status _find_command(std::string *command, cli_command_handler_t *handler);

    public:

        cli(cli_status (*write_fp) (uint8_t *, uint32_t), std::vector<cli_command_t> *commands, cli_process_mode mode);
        ~cli();
        cli_status cli_receiveChar(char c);
        cli_status cli_processCommand(void);

    };
    
    
} // namespace CLI


#endif