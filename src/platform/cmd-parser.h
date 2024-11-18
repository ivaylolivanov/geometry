#include <string.h>
#include <cctype>

enum OutputFormat
{
    OutputFormat_None = 0,
    OutputFormat_SP   = 1
};

struct CommandLine
{
    bool Help;
    OutputFormat Format;
    const char* HelpMessage;
};

static const char HELP_MESSAGE[] =
    "USAGE: <binary-file> -f sp [-h]\n"
    "  -h    Prints this message\n"
    "  -f    Expects value 'sp', otherwise prints this message\n";

// TODO: Implement longer option name parsing!
void ParseCmd(int arguments_count, char** arguments, CommandLine* cmd)
{
    char short_option_help   = 'h';
    char short_option_format = 'f';

    cmd->HelpMessage = HELP_MESSAGE;

    for (int i = 1; i < arguments_count; i += 2)
    {
        char* option_name = arguments[i];
        int argument_char = 0;
        if (option_name[argument_char] != '-')
            continue;

        ++argument_char;
        if (/*(option_name[argument_char] != '-')
               &&*/ (option_name[argument_char] != short_option_help)
            && (option_name[argument_char] != short_option_format))
            continue;

        if ((option_name[argument_char] == short_option_help)
            && (option_name[argument_char + 1] == '\0'))
        {
            cmd->Help = true;
            break;
        }

        if ((option_name[argument_char] == short_option_format)
            && (option_name[argument_char + 1] == '\0'))
        {
            if (arguments_count < 3)
            {
                cmd->Help = true;
                break;
            }

            char* format_value = arguments[i + 1];
            for (int j = 0; format_value[j]; ++j)
                format_value[j] = tolower(format_value[j]);

            cmd->Format = OutputFormat_None;
            if (strcmp(format_value, "sp"))
                cmd->Help = true;
            else
                cmd->Format = OutputFormat_SP;

            break;
        }
    }
}
