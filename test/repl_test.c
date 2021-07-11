#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "minunit.h"
#include "../src/builtins.h"
#include "../src/env.h"
#include "../src/printer.h"
#include "../src/repl.h"

#define LINE_LENGTH 2048

char *scenario_name = NULL;
char *input = NULL;
char *output = NULL;

static int starts_with(const char *str, const char *pre)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}

static char *trim(char *input)
{
    int length_of_input = strlen(input);

    if (length_of_input == 0)
        return input;

    int start_of_input = 0;
    while (start_of_input < length_of_input && input[start_of_input] < 33)
        start_of_input += 1;

    int end_of_input = length_of_input - 1;
    while (end_of_input > 0 && input[end_of_input] < 33)
        end_of_input -= 1;

    if (start_of_input == length_of_input)
    {
        free(input);
        return strdup("");
    }
    else if (start_of_input == 0 && end_of_input == length_of_input - 1)
    {
        return input;
    }
    else
    {
        int length_of_result = end_of_input - start_of_input + 1;

        char *result = malloc(length_of_result + 1);
        strncpy(result, input + start_of_input, length_of_result);
        result[length_of_result] = '\0';
        free(input);
        return result;
    }
}

static char *drop_left(char *text, size_t n)
{
    if (n == 0)
        return text;
    if (n >= strlen(text))
    {
        free(text);
        return strdup("");
    }
    else
    {
        char *result = strdup(text + n);
        free(text);
        return result;
    }
}

static char *join_separated_with_newline(char *current, char *rest)
{
    if (current == NULL)
    {
        return rest;
    }
    else
    {
        char *result = (char *)malloc(strlen(current) + strlen("\n") + strlen(rest) + 1);
        strcpy(result, current);
        strcat(result, "\n");
        strcat(result, rest);
        free(current);
        free(rest);
        return result;
    }
}

static void validateTest(Value *env)
{
    if (input != NULL)
    {
        if (output == NULL)
        {
            output = strdup("");
        }
        tests_run += 1;
        if (VERBOSE_MINUNIT > 0)
        {
            scenario_name = scenario_name == NULL ? strdup(input) : scenario_name;
            printf("  Test: %s\n", scenario_name);
        }

        Value *rv = Repl_rep("**test**", input, env);
        char *repl_result = (IS_SUCCESSFUL(rv)) ? rv->strV : Printer_prStr(rv, 1, " ")->strV;
        if (strcmp(repl_result, output) == 0)
        {
            tests_passed += 1;
        }
        else
        {
            printf("Failed: %s\n", scenario_name);
            printf("  Input:    | %s\n", input);
            printf("  Output:   | %s\n", repl_result);
            printf("  Expected: | %s\n", output);
        }

        free(scenario_name);
        scenario_name = NULL;
        free(input);
        input = NULL;
        free(repl_result);
        repl_result = NULL;
        free(output);
        output = NULL;
    }
}

int main(int argc, char *argv[])
{
    value_initialise();
    
    if (argc != 2)
    {
        printf("Error: expected a single input file.\n");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Error: file does not exist: %s.\n", argv[1]);
        return 1;
    }

    printf("| %s\n", argv[1]);

    Value *env = builtins_initialise_environment();
    {
        char cwd[1024];
        getcwd(cwd, 1024);
        strcat(cwd, "/home");
        env_add_binding(env, mkSymbol("*source-name*"), mkString(cwd));
    }

    while (1)
    {
        char *line = NULL;
        size_t line_length = 0;
        int characters_read = getline(&line, &line_length, fp);

        if (characters_read == -1)
        {
            validateTest(env);
            break;
        }
        else
        {
            line = trim(line);
            if (strlen(line) == 0)
                continue;
            else if (starts_with(line, ";;"))
            {
                // ignore - comment
                free(line);
            }
            else if (starts_with(line, ";"))
            {
                if (output != NULL)
                {
                    validateTest(env);
                    scenario_name = trim(drop_left(line, 1));
                }
                else
                    scenario_name = join_separated_with_newline(scenario_name, trim(drop_left(line, 1)));
            }
            else if (starts_with(line, ">"))
            {
                output = join_separated_with_newline(output, trim(drop_left(line, 1)));
            }
            else
            {
                if (output != NULL)
                {
                    validateTest(env);

                    input = line;
                }
                else
                    input = join_separated_with_newline(input, line);
            }
        }
    }

    if (tests_passed == tests_run)
    {
        printf(". ALL TESTS PASSED\n");
    }
    printf(". Run: %d   Passed: %d   Failed: %d\n", tests_run, tests_passed, (tests_run - tests_passed));
    return (tests_passed == tests_run) ? 0 : 1;
}
