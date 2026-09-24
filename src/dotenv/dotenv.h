/**
* @file dotenv.h
*
* @brief simple, unix & windows portable, thread-safe, lightweight
*        module to load environment variables from a file &
*        safely retrieve environment variables from the os
*
* @details this (python-dotenv similar) module loads environment variables
*          from a '.env' file in the working directory with load_dotenv(),
*          or from any other file with load_dotenv_f().
*          environment variables can be safely retrieved from the os
*          with get_env(name, fallback), where a fallback value
*          can be specified, that gets returned if an error occurs or
*          the environment variable could not get retrieved from the os.
*
* @warning single lines in the '.env' file passed to load_dotenv() /
*          load_dotenv_f() can not exceed the length of
*          MAX_ENV_FILE_LINE_LEN (default: 4095, exlcluding '\n byte')
*          in order to load the environment variable in this line.
*          if a line in the file exceeds MAX_ENV_FILE_LINE_LEN, it
*          will be treated as invalid syntax.
*          therefore it will skip that line & return -1.
*          per line there can only be 1 environment variable defined.
*
* @note if the '.env' file contains lines with invalid syntax,
*       load_dotenv() / load_dotenv_f() will return -1 but it will
*       still load all other valid environment variables.
*       invalid syntax may be:
*        - missing '=', e.g.: [DB_USERroot]
*        - empty name, e.g.: [=root]
*        - name with spaces without quotes, e.g.: [DB USER=root]
*        - value with spaces without quotes, e.g.: [DB_USER=root group_super]
*        - quote-surrounded name with spaces, e.g.: ["DB USER"=root] / ['DB USER'=root]
*        - mixing different quote types, e.g.: [DB_USER='root"] / [DB_USER="'root"']
*
* @note these examples are (unusual, but) syntactically valid:
*       - [  KEY_SURROUNDED_BY_SPACES  = VALUE_SURROUNDED_BY_SPACES ] -> name will be {KEY_SURROUNDED_BY_SPACES}, value will be {VALUE_SURROUNDED_BY_SPACES}
*       - [EMPTY_VALUE= ]-> value will be empty (string)
*       - [EMPTY_VALUE_AND_COMMENT=# comment] -> value will be empty (string)
*       - ['"DOUBLE_QUOTING_KEY"'=""DOUBLE_QUOTING_VALUE""] -> name will be {"DOUBLE_QUOTING_KEY"}, value will be {"DOUBLE_QUOTING_VALUE"} 
*       - [VALUE_WITH_QUOTES_AND_SPACES="user group super"] / [VALUE_WITH_QUOTES_AND_SPACES='user group super'] -> value will be {user group super}
*
* @warning since get_env() returns a pointer to the process-internal
*          memory of environment variables, if get_env() is called and
*          this memory gets altered, the pointer could potentially
*          point to another entry or invalid memory!
*          accessing memory through this pointer at that stage is
*          undefined behaviour!
*          to avoid this, only use this module with a single thread
*          (e.g. with the main thread at initialization) or copy
*          the value to a local buffer.
*          
*
* @author 1810vincent (val <3)
*
* @date 09/2026
*/
#ifndef C_DOTENV_H
#define C_DOTENV_H


#define MAX_ENV_FILE_LINE_LEN 4095


#include <stdint.h>
#include <stdbool.h>


/**
* @brief loads environment variables from the '.env' file in the working directory
*
* @note if the '.env' file contains lines with a length exceeding
*       MAX_ENV_FILE_LINE_LEN or any other invalid syntax, load_dotenv
*       will return -1 but it will still load all other valid environment variables.
*
* @param[in] overwrite  specifies if an already existing environment variable's
*                       value should be overwritten by the new value
*
* @return returns 0 on success, or -1 on error
*/
int32_t load_dotenv(bool overwrite);

/**
* @brief loads environment variables from the file specified by filepath
*
* @note if the file contains lines with a length exceeding
*       MAX_ENV_FILE_LINE_LEN or any other invalid syntax, load_dotenv
*       will return -1 but it will still load all other valid environment variables.
*
* @param[in] filepath   the file's path
* @param[in] overwrite  specifies if an already existing environment variable's
*                       value should be overwritten by the new value
*
* @return returns 0 on success, or -1 on error
*/
int32_t load_dotenv_f(const char* filepath, bool overwrite);


/**
* @brief safely retrieves an environment variable's value specified
*        by name from the os, or return fallback on error
*
* @param[in] name       the name of the environment variable
* @param[in] fallback   the fallback string that gets returned if error occurs
*
* @return returns the value of the environment variable on success,
*         or the fallback string on error
*/
const char* get_env(const char* name, const char* fallback);

#endif /* C_DOTENV_H */