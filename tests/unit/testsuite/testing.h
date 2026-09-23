/**
* @file testing.h
*
* @brief simple, ultra lightweight, zero-dependency, unix & windows
*        portable, thread-safe, header-only assertion (unit) test
*        portable for c code with eye-catching evaluation
*
* @details this standalone (unit) test framework header-only-module provides
*          the only 2 necessary macros to perform simple, yet effective
*          tests on any amount (< UINT32_MAX) of boolean expressions.
*          to use this test-framework, the macro TESTING_DEFINE
*          has to be defined in exactly 1 .c-file (of the target binary)
*          before including the header, like this:
*          '''
*          #define TESTING_DEFINE
*          #include "testing.h"
*          '''
*          if this condition is fulfilled, the header can be included and
*          its functions can be used in any number of other .c-files
*          (of the target binary) without the define.
*          in order to start 1 complete test run, call test_run(test_function)
*          with a pointer to test_function where tests are implemented.
*          inside test_function, call test_assert(expression) to test
*          the boolean expression.
*          after all test cases inside of test_function have been evaluated,
*          test_run will return the amount of failed tests in test_function,
*          or UINT32_MAX (4294967295) on error.
*          the module will automatically print the resulting evaluation
*          of how many tests have passed and failed in test_function.
*
* @note only access the 2 public macros of this header. all variables and
*       functions prefixed with an underscore ('_[...]') are internal and
*       should never be accessed!
*
* @note for best practice & thread-safety only call test_assert within
*       the test_function called by test_run.
*
* @warning since this test-framework's test_assert calls are non-aborting,
*          it does not protect from any exceptions or segmentation faults
*          after calling test_assert on a (previously) false expression.
*          only pass valid memory accesses into test_assert!
*
* @author 1810vincent (val <3)
*
* @date 09/2026
*/
#ifndef TESTSUITE_TESTING_H
#define TESTSUITE_TESTING_H


/**
* @brief evaluates the passed boolean expression
*
* @details evaluates the boolean expression, prints the evaluation &
*          registers the outcome in the testsuite
*
* @param[in] expression     the boolean expression to be evaluated
* @param[in] print_expr_str specifies whether the expression should be printed
*
* @note NULL passed as the expression counts as a false expression
*/
#define test_assert(expression, print_expr_str) _test_assert_v((expression), (const char*)__FILE__, (int32_t)__LINE__, (const char*)#expression, (print_expr_str))


/**
* @brief runs the complete test within test_function
*
* @details calls test_function, prints a resulting evaluation of all tests
*          (test_assert() calls) made in the test_function & returns the
*          amount of failed tests, or UINT32_MAX (4294967295) on error
*
* @param[in] test_function the test implementing function which will be called
*
* @return returns the amount of failed tests, or UINT32_MAX (4294967295) on error
*/
#define test_run(test_function) _test_run_v((test_function), (const char*)__FILE__)

/* -------------------------------------------------------------------------------------------------------------------------------- */


#include <stdint.h>
#include <stdbool.h>


#define RESET "\33[0m"
#define CYAN "\33[36m"
#define GREEN "\33[32m"
#define RED "\33[31m"


#ifndef TESTING_DEFINE

    extern uint32_t _passed;
    extern uint32_t _failed;


    extern void _test_assert_v(bool expression, const char* file, int32_t line, const char* expr_str, bool print_expr_str);

    extern uint32_t _test_run_v(void (*test_function)(void), const char* file);


#else /* TESTING_DEFINE */

    #if defined(_WIN32) || defined(_WIN64)

        #include <windows.h>


        static SRWLOCK global_mutex = SRWLOCK_INIT;

        #define lock()      AcquireSRWLockExclusive(&global_mutex)
        #define unlock()    ReleaseSRWLockExclusive(&global_mutex)


        static inline void _enable_windows_colors(void) {
            HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
            if (stdout_handle == INVALID_HANDLE_VALUE)
                return;
            DWORD mode = 0;
            if (!GetConsoleMode(stdout_handle, &mode))
                return;
            mode |= 0x0004;
            SetConsoleMode(stdout_handle, mode);
        }


    #else

        #include <pthread.h>


        static pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

        #define lock()      pthread_mutex_lock(&global_mutex)
        #define unlock()    pthread_mutex_unlock(&global_mutex)

    #endif


    #include <stdio.h>
    #include <string.h>


    uint32_t _passed = 0U;
    uint32_t _failed = 0U;


    static const char* _strip_path(const char* path) {
        if (!path)
            return NULL;
        const char* last_slash = strrchr(path, '/');
        const char* last_backslash = strrchr(path, '\\');
        if (last_backslash && (last_backslash > last_slash)) {
            last_slash = last_backslash;
        }
        if (!last_slash) {
            return path;
        }
        return ++last_slash;
    }

    void _test_assert_v(bool expression, const char* file, int32_t line, const char* expr_str, bool print_expr_str) {
        if (!file || !expr_str)
            return;
        if (!print_expr_str)
            expr_str = "";
        if (expression) {
            _passed++;
            printf("%s:%i: test assertion %spassed%s ('%s')\n", _strip_path(file), line, GREEN, RESET, expr_str);
        }
        else {
            _failed++;
            printf("%s:%i: test assertion %sfailed%s ('%s')\n", _strip_path(file), line, RED, RESET, expr_str);
        }
    }


    static inline void _test_start(const char* file) {
        printf("\n%s--- tests of file: %s%s\n", CYAN, file, RESET);
    }

    static inline uint32_t _test_end(void) {
        printf("\ntests passed: %s%u%s | failed: %s%u%s\n%s---%s\n", GREEN, _passed, RESET, RED, _failed, RESET, CYAN, RESET);
        return _failed;
    }

    uint32_t _test_run_v(void (*test_function)(void), const char* file) {
        if (!test_function || !file)
            return UINT32_MAX;
    #if defined(_WIN32) || defined(_WIN64)
        _enable_windows_colors();
    #endif
        lock();
        _passed = (uint32_t)0;
        _failed = (uint32_t)0;
        _test_start(file);
        test_function();
        uint32_t r = _test_end();
        unlock();
        return r;
    }

#endif /* TESTING_DEFINE */

#endif /* TESTSUITE_TESTING_H */