/**
* @file dotenv.h
*
* @brief simple, unix & windows portable, lightweight module to load
*        environment variables from a file & safely retrieve
*        environment variables from the os
*
* @details this (python-dotenv similar) module loads environment variables
*          from a '.env' file in the working directory with load_dotenv,
*          or from any other file with load_dotenv_f.
*          all environment variables can be safely retrieved from the os
*          with get_env(name, fallback), where a fallback value
*          can be specified, that gets returned if an error occurs or
*          the environment variable could not get retrieved from the os.
*
* @warning single lines in the '.env' file passed to load_dotenv /
*          load_dotenv_f can not exceed the length of
*          MAX_ENV_FILE_LINE_LEN (default: 4096).
*          if a line in the file exceed the MAX_ENV_FILE_LINE_LEN it
*          will be treated as invalid syntax and therefore skip that line
*
* @warning if the '.env' file contains lines with invalid syntax the
*          load_dotenv functions will return -1 BUT it will still load
*          all other valid environment variables.
*          invalid syntax may be:
*           - missing '=', example: [DB_USERroot]
*           - empty name, example: [=root]
*           - name with spaces, example: [DB USER=root]
*           - value with spaces, example: [DB_USER=user group super]
*           - quote-surrounded name with spaces, examples: ["DB USER"=root], ['DB USER'=root]
*           - mixing different quote types, examples: [DB_USER='root"], [DB_USER="'root"']
*
* @note these examples are (unusual, but) syntactically valid:
*       - [  KEY_SURROUNDED_BY_SPACES  = VALUE_SURROUNDED_BY_SPACES ] -> name will be (KEY_SURROUNDED_BY_SPACES), value will be (VALUE_SURROUNDED_BY_SPACES)
*       - [EMPTY_VALUE= ]-> value will be an empty string ("")
*       - [EMPTY_VALUE_AND_COMMENT=# comment] -> value will be an empty string ("")
*       - ['"DOUBLE_QUOTING_KEY"'=""DOUBLE_QUOTING_VALUE""] -> name will be ("DOUBLE_QUOTING_KEY"), value will be ("DOUBLE_QUOTING_VALUE") 
*       - [VALUE_WITH_QUOTES_AND_SPACES="user group super"] / [VALUE_WITH_QUOTES_AND_SPACES='user group super'] -> value will be (user group super)
*
* @author 1810vincent (val <3)
*
* @date 09/2026
*/
#ifndef C_DOTENV_H
#define C_DOTENV_H


#include <stdint.h>
#include <stdbool.h>


#define MAX_ENV_FILE_LINE_LEN 4096


/**
* @brief loads environment variables from the '.env' file in the working directory
*
* @note if the '.env' file contains lines with invalid syntax, load_dotenv
*       will return -1 but it will still load all other valid environment variables.
*
* @param[in] overwrite  specifies if an already existing value
*                       should be overwritten by the new value
*
* @return returns 0 on success, or -1 on error
*/
int32_t load_dotenv(bool overwrite);

/**
* @brief loads environment variables from the file specified by path
*
* @note if the file contains lines with invalid syntax, load_dotenv_f
*       will return -1 but it will still load all other valid environment variables.
*
* @param[in] file       the file's path
* @param[in] overwrite  specifies if an already existing value
*                       should be overwritten by the new value
*
* @return returns 0 on success, or -1 on error
*/
int32_t load_dotenv_f(const char* file, bool overwrite);


/**
* @brief safely gets an environment variable's value specified by name
*        from the os, or return fallback on error
*
* @param[in] name       the name of the environment variable
* @param[in] fallback   the fallback string that gets returned if error occurs
*
* @return returns the value of the environment variable on success,
*         or the fallback string on error
*/
const char* get_env(const char* name, const char* fallback);

#endif /* C_DOTENV_H */