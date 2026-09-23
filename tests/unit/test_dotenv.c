/**
* @file test_dotenv.c
*
* @brief all unit tests for the ceriousdb / dotenv module
*        ('src/dotenv/dotenv.h', 'src/dotenv/dotenv.c')
*
* @author 1810vincent (val <3)
*
* @date 09/2026
*/
#define TESTING_DEFINE
#include "testsuite/testing.h"
#include "../../src/dotenv/dotenv.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define ENV_VALID_PATH "tests/unit/test_envfiles/.env_valid"
#define ENV_NO_OVERRIDE_PATH "tests/unit/test_envfiles/.env_no_overwrite"
#define ENV_EMPTY_PATH "tests/unit/test_envfiles/.env_empty"
#define ENV_CORRUPTED_VALID_FIRST_PATH "tests/unit/test_envfiles/.env_corrupted_valid_first"
#define ENV_CORRUPTED_VALID_LAST_PATH "tests/unit/test_envfiles/.env_corrupted_valid_last"
#define ENV_CORRUPTED_VALID_SURROUND_PATH "tests/unit/test_envfiles/.env_corrupted_valid_surround"
#define ENV_CORRUPTED1_PATH "tests/unit/test_envfiles/.env_corrupted1"
#define ENV_CORRUPTED2_PATH "tests/unit/test_envfiles/.env_corrupted2"
#define ENV_CORRUPTED3_PATH "tests/unit/test_envfiles/.env_corrupted3"
#define ENV_CORRUPTED_ENGINEERED_PATH "tests/unit/test_envfiles/.env_corrupted_engineered"
#define ENV_CORRUPTED_LENGTH_PATH "tests/unit/test_envfiles/.env_corrupted_length"

#define DO_OVERWRITE 1
#define DONT_OVERWRITE 0


/* calls get_env with NULL name -> should return fallback string */
void test_get_env_name_null(void) {
    const char* fallback = "null";
    test_assert((strcmp(get_env(NULL, fallback), fallback) == 0), true);
}

/* calls get_env with NULL name and NULL fallback -> should return fallback (NULL) */
void test_get_env_name_null_fallback_null(void) {
    const char* fallback = NULL;
    test_assert((get_env(NULL, fallback) == fallback), true);
}

/* calls get_env with invalid name -> should return fallback string */
void test_get_env_invalid_name(void) {
    const char* fallback = "invalid";
    test_assert((strcmp(get_env("invalidname", fallback), fallback) == 0), true);
}


/* calls load_dotenv_f with NULL -> should return -1 */
void test_load_dotenv_f_filepath_null(void) {
    test_assert((load_dotenv_f(NULL, DO_OVERWRITE) == (int32_t)-1), true);
}

/* calls load_dotenv_f with invalid filepath -> should return -1 */
void test_load_dotenv_f_invalid_filepath(void) {
    test_assert((load_dotenv_f("/tests/null/.env", DO_OVERWRITE) == (int32_t)-1), true);
}

/* calls load_dotenv_f with, on unix-like systems invalid, filepath because of unix' case sensitivity-> should return -1 on unix-like systems, 0 on windows */
void test_load_dotenv_f_invalid_path_case_sensitivity(void) {
#if !defined(_WIN32) && !defined(_MSC_VER)
    test_assert((load_dotenv_f(".ENV", DO_OVERWRITE) == (int32_t)-1), true);
#else
    test_assert((load_dotenv_f(".ENV", DO_OVERWRITE) == (int32_t)0), true);
#endif
}


/* calls load_dotenv_f with empty .env file ".env_empty" -> should return 0 */
void test_load_dotenv_f_empty(void) {
    test_assert((load_dotenv_f(ENV_EMPTY_PATH, DO_OVERWRITE) == (int32_t)0), true);
}

/* calls load_dotenv_f with (semi) corrupted .env file ".env_corrupted_valid_first" (with 1 valid env var before the invalid env var) -> should return -1 BUT the valid env var should be set & the invalid env var should not be set */
void test_load_dotenv_f_corrupted_valid_first(void) {
    test_assert((load_dotenv_f(ENV_CORRUPTED_VALID_FIRST_PATH, DO_OVERWRITE) == (int32_t)-1), true);
    test_assert((strcmp(get_env("CORRUPTED_VALID_FIRST_KEY", ""), "CORRUPTED_VALID_FIRST_VALUE") == 0), true);
    test_assert((strcmp(get_env("", ""), "CORRUPTED_INVALID_FIRST_VALUE_CORRUPT") != 0), true);
}

/* calls load_dotenv_f with (semi) corrupted .env file ".env_corrupted_valid_last" (with 1 valid env var after the invalid env var) -> should return -1 BUT the valid env var should be set & the invalid env var should not be set */
void test_load_dotenv_f_corrupted_valid_last(void) {
    test_assert((load_dotenv_f(ENV_CORRUPTED_VALID_LAST_PATH, DO_OVERWRITE) == (int32_t)-1), true);
    test_assert((strcmp(get_env("CORRUPTED_VALID_LAST_KEY", ""), "CORRUPTED_VALID_LAST_VALUE") == 0), true);
    test_assert((strcmp(get_env("", ""), "CORRUPTED_INVALID_LAST_VALUE_CORRUPT") != 0), true);
}

/* calls load_dotenv_f with (semi) corrupted .env file ".env_corrupted_valid_surround" (with 1 invalid env var surrounded by 2 valid env vars) -> should return -1 BUT the valid env vars should be set & the invalid env var should not be set */
void test_load_dotenv_f_corrupted_valid_surround(void) {
    test_assert((load_dotenv_f(ENV_CORRUPTED_VALID_SURROUND_PATH, DO_OVERWRITE) == (int32_t)-1), true);
    test_assert((strcmp(get_env("CORRUPTED_VALID_SURROUND_KEY1", ""), "CORRUPTED_VALID_SURROUND_VALUE1") == 0), true);
    test_assert((strcmp(get_env("CORRUPTED_VALID_SURROUND_KEY2", ""), "CORRUPTED_VALID_SURROUND_VALUE2") == 0), true);
    test_assert((strcmp(get_env("", ""), "CORRUPTED_INVALID_SURROUND_VALUE_CORRUPT") != 0), true);
}

/* calls load_dotenv_f with corrupted (invalid) .env file ".env_corrupt1" -> should return -1 & not set invalid env var */
void test_load_dotenv_f_corrupted1(void) {
    test_assert((load_dotenv_f(ENV_CORRUPTED1_PATH, DO_OVERWRITE) == (int32_t)-1), true);
    test_assert((strcmp(get_env("", ""), "CORRUPTED_EMPTY_KEY_VALUE") != 0), true);
}

/* calls load_dotenv_f with corrupted (invalid) .env file ".env_corrupt2" -> should return -1 & not set invalid env var */
void test_load_dotenv_f_corrupted2(void) {
    test_assert((load_dotenv_f(ENV_CORRUPTED2_PATH, DO_OVERWRITE) == (int32_t)-1), true);
    test_assert((strcmp(get_env("CORRUPTED_NO_EQUALS_SIGN_KEY", ""), "CORRUPTED_NO_EQUALS_SIGN_VALUE") != 0), true);
}

/* calls load_dotenv_f with corrupted (invalid) .env file ".env_corrupt3" -> should return -1 & not set invalid env var */
void test_load_dotenv_f_corrupted3(void) {
    test_assert((load_dotenv_f(ENV_CORRUPTED3_PATH, DO_OVERWRITE) == (int32_t)-1), true);
    test_assert((strcmp(get_env("CORRUPTED SPACE KEY KEY", ""), "CORRUPTED_SPACE_KEY_VALUE") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED", ""), "CORRUPTED_SPACE_KEY_VALUE") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED_SPACE_VALUE_KEY", ""), "CORRUPTED SPACE VALUE VALUE") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED SPACE KEY QUOTE KEY", ""), "CORRUPTED_SPACE_KEY_QUOTE_VALUE") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED SPACE KEY SINGLE QUOTE", ""), "CORRUPTED_SPACE_KEY_SINGLE_QUOTE_VALUE") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED_MIXED_QUOTE_TYPES_KEY1", ""), "CORRUPTED_MIXED_QUOTE_TYPES_VALUE1") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED_MIXED_QUOTE_TYPES_KEY1", ""), "'CORRUPTED_MIXED_QUOTE_TYPES_VALUE1\"") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED_MIXED_QUOTE_TYPES_KEY2", ""), "CORRUPTED_MIXED_QUOTE_TYPES_VALUE2") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED_MIXED_QUOTE_TYPES_KEY2", ""), "\"CORRUPTED_MIXED_QUOTE_TYPES_VALUE2'") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED_MIXED_DOUBLE_QUOTES_TYPES_KEY1", ""), "CORRUPTED_MIXED_QUOTES_TYPES_VALUE1") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED_MIXED_DOUBLE_QUOTES_TYPES_KEY1", ""), "'\"CORRUPTED_MIXED_QUOTES_TYPES_VALUE1'\"") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED_MIXED_DOUBLE_QUOTES_TYPES_KEY1", ""), "\"CORRUPTED_MIXED_QUOTES_TYPES_VALUE1'") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED_MIXED_DOUBLE_QUOTES_TYPES_KEY2", ""), "CORRUPTED_MIXED_QUOTES_TYPES_VALUE2") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED_MIXED_DOUBLE_QUOTES_TYPES_KEY2", ""), "\"'CORRUPTED_MIXED_QUOTES_TYPES_VALUE2\"'") != 0), true);
    test_assert((strcmp(get_env("CORRUPTED_MIXED_DOUBLE_QUOTES_TYPES_KEY2", ""), "'CORRUPTED_MIXED_QUOTES_TYPES_VALUE2\"") != 0), true);
}

/* calls load_dotenv_f with a heavily corrupted .env file ".env_corrupted_engineered" -> should return -1 and not set env var */
void test_load_dotenv_f_corrupted_engineered(void) {
    test_assert((load_dotenv_f(ENV_CORRUPTED_ENGINEERED_PATH, DO_OVERWRITE) == (int32_t)-1), true);
    test_assert((strcmp(get_env("CORRUPTED_ENGINEERED_KEY", ""), "CORRUPTED ENGINEERED VALUE") != 0), true);
}

/* calls load_dotenv_f with corrupted (invalid) .env file ".env_corrupt_length" with a single line that exceeds the max line length -> should return -1 and not set the env var */
void test_load_dotenv_f_corrupted_length(void) {
    test_assert((load_dotenv_f(ENV_CORRUPTED_LENGTH_PATH, DO_OVERWRITE) == (int32_t)-1), true);
    test_assert((strcmp(get_env("CORRUPTED_EXCEED_MAX_LINE_LENGTH_KEY", ""), "THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG_THIS_VALUE_IS_TOO_LONG")
    != 0), false);
}


/* calls load_dotenv_f with valid .env file ".env_valid" -> should return 0 */
void test_load_dotenv_f_valid(void) {
    test_assert((load_dotenv_f(ENV_VALID_PATH, DO_OVERWRITE) == (int32_t)0), true);
}


/* checks for valid key nr. 1 */
void test_load_dotenv_f_valid_key1(void) {
    test_assert((strcmp(get_env("EXAMPLEKEY1", ""), "EXAMPLEVALUE1") == 0), true);
}

/* checks for valid key nr. 2 */
void test_load_dotenv_f_valid_key2(void) {
    test_assert((strcmp(get_env("EXAMPLE_KEY2", ""), "EXAMPLE_VALUE2") == 0), true);
}

/* checks for valid key nr. 3 */
void test_load_dotenv_f_valid_key3(void) {
    test_assert((strcmp(get_env("EXAMPLE_KEY_3", ""), "EXAMPLE_VALUE_3") == 0), true);
}

/* checks for valid key nr. 4 */
void test_load_dotenv_f_valid_key4(void) {
    test_assert((strcmp(get_env("EXAMPLE_DOT_KEY", ""), ".EXAMPLE_DOT_VALUE") == 0), true);
}

/* checks for valid key nr. 5 */
void test_load_dotenv_f_valid_key5(void) {
    test_assert((strcmp(get_env("EXAMPLE_COMMENT_KEY", ""), "EXAMPLE_COMMENT_VALUE") == 0), true);
}

/* checks for valid key nr. 6 */
void test_load_dotenv_f_valid_key6(void) {
    test_assert((strcmp(get_env("EXAMPLE_KEY_SPACE_KEY", ""), "EXAMPLE_KEY_SPACE_VALUE") == 0), true);
}

/* checks for valid key nr. 7 */
void test_load_dotenv_f_valid_key7(void) {
    test_assert((strcmp(get_env("EXAMPLE_VALUE_SPACE_KEY", ""), "EXAMPLE_VALUE_SPACE_VALUE") == 0), true);
}

/* checks for valid key nr. 8 */
void test_load_dotenv_f_valid_key8(void) {
    test_assert((strcmp(get_env("EXAMPLE_KEY_N_VALUE_SPACE_KEY", ""), "EXAMPLE_KEY_N_VALUE_SPACE_VALUE") == 0), true);
}

/* checks for valid key nr. 9 */
void test_load_dotenv_f_valid_key9(void) {
    test_assert((strcmp(get_env("EXAMPLE_KEY_N_VALUE_DOUBLE_SPACE_KEY", ""), "EXAMPLE_KEY_N_VALUE_DOUBLE_SPACE_VALUE") == 0), true);
}

/* checks for valid key nr. 10 */
void test_load_dotenv_f_valid_key10(void) {
    test_assert((strcmp(get_env("EXAMPLE_KEY_QUOTE_MARKS_KEY", ""), "EXAMPLE_KEY_QUOTE_MARKS_VALUE") == 0), true);
}

/* checks for valid key nr. 11 */
void test_load_dotenv_f_valid_key11(void) {
    test_assert((strcmp(get_env("EXAMPLE_VALUE_QUOTE_MARKS_KEY", ""), "EXAMPLE_VALUE_QUOTE_MARKS_VALUE") == 0), true);
}

/* checks for valid key nr. 12 */
void test_load_dotenv_f_valid_key12(void) {
    test_assert((strcmp(get_env("EXAMPLE_KEY_N_VALUE_QUOTE_MARKS_KEY", ""), "EXAMPLE_KEY_N_VALUE_QUOTE_MARKS_VALUE") == 0), true);
}

/* checks for valid key nr. 13 */
void test_load_dotenv_f_valid_key13(void) {
    test_assert((strcmp(get_env("EXAMPLE_KEY_SINGLE_QUOTE_MARKS_KEY", ""), "EXAMPLE_KEY_SINGLE_QUOTE_MARKS_VALUE") == 0), true);
}

/* checks for valid key nr. 14 */
void test_load_dotenv_f_valid_key14(void) {
    test_assert((strcmp(get_env("EXAMPLE_VALUE_SINGLE_QUOTE_MARKS_KEY", ""), "EXAMPLE_VALUE_SINGLE_QUOTE_MARKS_VALUE") == 0), true);
}

/* checks for valid key nr. 15 */
void test_load_dotenv_f_valid_key15(void) {
    test_assert((strcmp(get_env("EXAMPLE_KEY_N_VALUE_SINGLE_QUOTE_MARKS_KEY", ""), "EXAMPLE_KEY_N_VALUE_SINGLE_QUOTE_MARKS_VALUE") == 0), true);
}

/* checks for valid key nr. 16 */
void test_load_dotenv_f_valid_key16(void) {
    test_assert((strcmp(get_env("EXAMPLE_VALUE_QUOTE_MARKS_SPACE_KEY", ""), "EXAMPLE VALUE QUOTE MARKS SPACE VALUE") == 0), true);
}

/* checks for valid key nr. 17 */
void test_load_dotenv_f_valid_key17(void) {
    test_assert((strcmp(get_env("EXAMPLE_VALUE_SINGLE_QUOTE_MARKS_SPACE_KEY", ""), "EXAMPLE VALUE SINGLE QUOTE MARKS VALUE") == 0), true);
}

/* checks for valid key nr. 18 */
void test_load_dotenv_f_valid_key18(void) {
    test_assert((strcmp(get_env("EXAMPLE_EMPTY_VALUE_KEY", ""), "") == 0), true);
}

/* checks for valid key nr. 19 */
void test_load_dotenv_f_valid_key19(void) {
    test_assert((strcmp(get_env("EXAMPLE_EMPTY_VALUE_COMMENT_KEY", ""), "") == 0), true);
}

/* checks for valid key nr. 20/1 */
void test_load_dotenv_f_valid_key20_1(void) {
    test_assert((strcmp(get_env("EXAMPLE_OVERWRITE_KEY", ""), "EXAMPLE_OVERWRITE_VALUE1") != 0), true);
}

/* checks for valid key nr. 20/2 */
void test_load_dotenv_f_valid_key20_2(void) {
    test_assert((strcmp(get_env("EXAMPLE_OVERWRITE_KEY", ""), "EXAMPLE_OVERWRITE_VALUE2") == 0), true);
}

/* checks for valid key nr. 21 */
void test_load_dotenv_f_valid_key21(void) {
    test_assert((strcmp(get_env("\"EXAMPLE_DOUBLE_QUOTE_KEY\"", ""), "'EXAMPLE_DOUBLE_QUOTE_VALUE'") == 0), true);
}

/* checks for valid key nr. 22 */
void test_load_dotenv_f_valid_key22(void) {
    test_assert((strcmp(get_env("'EXAMPLE_MIXED_DOUBLE_QUOTE_KEY'", ""), "\"EXAMPLE_MIXED_DOUBLE_QUOTE_VALUE\"") == 0), true);
}

/* checks for valid key nr. 23 */
void test_load_dotenv_f_valid_key23(void) {
    test_assert((strcmp(get_env("'\"EXAMPLE_MIXED_TRIPLE_QUOTE_KEY\"'", ""), "\"'EXAMPLE_MIXED_TRIPLE_QUOTE_VALUE'\"") == 0), true);
}


/* checks for not overwriting with load_dotenv */
void test_load_dotenv_f_valid_no_overwrite(void) {
    test_assert((load_dotenv_f(ENV_NO_OVERRIDE_PATH, DONT_OVERWRITE) == (int32_t)0), true);
    test_assert((strcmp(get_env("NO_OVERWRITE_KEY", ""), "NO_OVERWRITE_VALUE1") == 0), true);
    test_assert((strcmp(get_env("NO_OVERWRITE_KEY", ""), "NO_OVERWRITE_VALUE2") != 0), true);
}


void test(void) {
    test_get_env_name_null();
    test_get_env_name_null_fallback_null();
    test_get_env_invalid_name();

    test_load_dotenv_f_filepath_null();
    test_load_dotenv_f_invalid_filepath();
    test_load_dotenv_f_invalid_path_case_sensitivity();

    test_load_dotenv_f_empty();
    test_load_dotenv_f_corrupted_valid_first();
    test_load_dotenv_f_corrupted_valid_last();
    test_load_dotenv_f_corrupted1();
    test_load_dotenv_f_corrupted2();
    test_load_dotenv_f_corrupted3();
    test_load_dotenv_f_corrupted_engineered();
    test_load_dotenv_f_corrupted_length();

    /* completely valid .env loaded -> test if all environment variables are set with get_env() in test_load_dotenv_f_valid_key[...] functions */
    test_load_dotenv_f_valid();

    test_load_dotenv_f_valid_key1();
    test_load_dotenv_f_valid_key2();
    test_load_dotenv_f_valid_key3();
    test_load_dotenv_f_valid_key4();
    test_load_dotenv_f_valid_key5();
    test_load_dotenv_f_valid_key6();
    test_load_dotenv_f_valid_key7();
    test_load_dotenv_f_valid_key8();
    test_load_dotenv_f_valid_key9();
    test_load_dotenv_f_valid_key10();
    test_load_dotenv_f_valid_key11();
    test_load_dotenv_f_valid_key12();
    test_load_dotenv_f_valid_key13();
    test_load_dotenv_f_valid_key14();
    test_load_dotenv_f_valid_key15();
    test_load_dotenv_f_valid_key16();
    test_load_dotenv_f_valid_key17();
    test_load_dotenv_f_valid_key18();
    test_load_dotenv_f_valid_key19();
    test_load_dotenv_f_valid_key20_1();
    test_load_dotenv_f_valid_key20_2();
    test_load_dotenv_f_valid_key21();
    test_load_dotenv_f_valid_key22();
    test_load_dotenv_f_valid_key23();

    test_load_dotenv_f_valid_no_overwrite();
}


int main(void) {
    if (test_run(test) == UINT32_MAX)
        printf("error in test_run!\n");
    return 0;
}