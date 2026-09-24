/**
* @file testing.h
*
* @brief simplistic, ultra lightweight, unix & windows portable,
*        thread-safe, header-only assertion (unit) test-framework
*        for c code with eye-catching evaluation
*
* @details this standalone (unit) test-framework header-only-module
*          provides the only 2 necessary macros to perform simple
*          tests on any amount (< UINT64_MAX) of boolean expressions.
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
*          in order to start a complete test run, call test_run(test_function)
*          with a pointer to test_function where tests are implemented.
*          inside test_function, call test_assert() to test
*          the boolean expression.
*          after all tests inside of test_function have been evaluated,
*          test_run() will return the amount of failed tests in
*          test_function, or UINT64_MAX on error.
*          the module will automatically print the resulting evaluation
*          of how many tests have passed and failed in the test run.
*
* @note only access the 2 macros of this header. all variables and
*       functions prefixed with 'testing_internal_' are
*       module-internal-only and should never be accessed!
*
* @note the framework is fully thread-safe. multiple threads can safely 
*       be created outside and inside test_function. test_assert()
*       can safely be called concurrently from all of them.
*
* @warning since this test-framework's test_assert() function is non-aborting,
*          it does not protect from any exceptions or segmentation faults
*          after calling test_assert() on a (previously) false expression.
*          only pass valid memory accesses into test_assert()!
*
* @author 1810vincent (val <3)
*
* @date 09/2026
*/
#ifndef UNITTEST_TESTING_H
#define UNITTEST_TESTING_H


/**
* @brief evaluates the boolean expression
*
* @details evaluates the boolean expression, prints the evaluation &
*          registers the outcome internally.
*
* @note non-aborting:
*       this function will never abort the thread!
*
* @param[in] expression the boolean expression to be evaluated
* @param[in] print_expr specifies whether expression should be printed
*                       in the evaluation printing.
*                       if false, '...' will be printed instead.
*
* @note NULL passed as expression counts as a false expression
*/
#define test_assert(expression, print_expr) testing_internal_test_assert_v((expression), (const char*)__FILE__, (uint64_t)__LINE__, (const char*)#expression, (print_expr))


/**
* @brief runs all tests within test_function
*
* @details calls test_function, prints a resulting evaluation of all tests
*          (test_assert() calls) made in test_function & returns the
*          amount of failed tests
*
* @param[in] test_function the test implementing function which will be called
*
* @return returns the amount of failed tests, or UINT64_MAX on error
*/
#define test_run(test_function) testing_internal_test_run_v((test_function), (const char*)__FILE__)

/* -------------------------------------------------------------------------------------------------------------------------------- */


#include <stdint.h>
#include <stdbool.h>


#define TESTING_INTERNAL_RESET  "\33[0m"
#define TESTING_INTERNAL_CYAN   "\33[36m"
#define TESTING_INTERNAL_GREEN  "\33[32m"
#define TESTING_INTERNAL_RED    "\33[31m"


#ifndef TESTING_DEFINE

    extern uint64_t testing_internal_passed;
    extern uint64_t testing_internal_failed;


    extern void testing_internal_test_assert_v(bool expression, const char* file, uint64_t line, const char* expr_str, bool print_expr);


    extern uint64_t testing_internal_test_run_v(void (*test_function)(void), const char* file);


#else /* TESTING_DEFINE */

    #if defined(_WIN32) || defined(_WIN64)

        #include <windows.h>


        static SRWLOCK testing_internal_run_mutex = SRWLOCK_INIT;
        static SRWLOCK testing_internal_assert_mutex = SRWLOCK_INIT;

        #define testing_internal_lock_run()      AcquireSRWLockExclusive(&testing_internal_run_mutex)
        #define testing_internal_unlock_run()    ReleaseSRWLockExclusive(&testing_internal_run_mutex)

        #define testing_internal_lock_assert()   AcquireSRWLockExclusive(&testing_internal_assert_mutex)
        #define testing_internal_unlock_assert() ReleaseSRWLockExclusive(&testing_internal_assert_mutex)

        
        static DWORD testing_internal_old_mode = 0;

        static inline void testing_internal_enable_windows_colors(void) {
            HANDLE term_handle = GetStdHandle(STD_OUTPUT_HANDLE);
            if (term_handle == INVALID_HANDLE_VALUE)
                return;
            if (!GetConsoleMode(term_handle, &testing_internal_old_mode))
                return;
            DWORD new_mode = testing_internal_old_mode | 0x0004;
            SetConsoleMode(term_handle, new_mode);
            return;
        }

        static inline void testing_internal_restore_windows_mode(void) {
            HANDLE term_handle = GetStdHandle(STD_OUTPUT_HANDLE);
            if (term_handle == INVALID_HANDLE_VALUE)
                return;
            SetConsoleMode(term_handle, testing_internal_old_mode);
            return;
        }


    #else

        #include <pthread.h>


        static pthread_mutex_t testing_internal_run_mutex = PTHREAD_MUTEX_INITIALIZER;
        static pthread_mutex_t testing_internal_assert_mutex = PTHREAD_MUTEX_INITIALIZER;

        #define testing_internal_lock_run()      pthread_mutex_lock(&testing_internal_run_mutex)
        #define testing_internal_unlock_run()    pthread_mutex_unlock(&testing_internal_run_mutex)

        #define testing_internal_lock_assert()   pthread_mutex_lock(&testing_internal_assert_mutex)
        #define testing_internal_unlock_assert() pthread_mutex_unlock(&testing_internal_assert_mutex)

    #endif


    #include <stdio.h>
    #include <inttypes.h>
    #include <string.h>


    uint64_t testing_internal_passed = (uint64_t)0ULL;
    uint64_t testing_internal_failed = (uint64_t)0ULL;


    static const char* testing_internal_strip_path(const char* path) {
        if (!path)
            return NULL;
        const char* last_slash = strrchr(path, (int)'/');
        const char* last_backslash = strrchr(path, (int)'\\');
        const char* last = NULL;
        if (last_slash && last_backslash)
            last = (last_slash > last_backslash) ? last_slash : last_backslash;
        else if (last_slash)
            last = last_slash;
        else if (last_backslash)
            last = last_backslash;
        else
            return path;
        return ++last;
    }

    void testing_internal_test_assert_v(bool expression, const char* file, uint64_t line, const char* expr_str, bool print_expr) {
        if (!file || !expr_str)
            return;
        if (!print_expr)
            expr_str = "...";
        testing_internal_lock_assert();
        if (expression) {
            testing_internal_passed++;
            printf("%s:%" PRIu64 ": test assertion %spassed%s ('%s')\n", testing_internal_strip_path(file), line, TESTING_INTERNAL_GREEN, TESTING_INTERNAL_RESET, expr_str);
            fflush(stdout);
        }
        else {
            testing_internal_failed++;
            printf("%s:%" PRIu64 ": test assertion %sfailed%s ('%s')\n", testing_internal_strip_path(file), line, TESTING_INTERNAL_RED, TESTING_INTERNAL_RESET, expr_str);
            fflush(stdout);
        }
        testing_internal_unlock_assert();
        return;
    }


    static inline void testing_internal_test_start(const char* file) {
        if (!file)
            file = "...";
        printf("\n%s--- tests of file: %s%s\n", TESTING_INTERNAL_CYAN, file, TESTING_INTERNAL_RESET);
        return;
    }

    static inline uint64_t testing_internal_test_end(void) {
        printf("\ntests passed: %s%" PRIu64 "%s | failed: %s%" PRIu64 "%s\n%s---%s\n", TESTING_INTERNAL_GREEN, testing_internal_passed, TESTING_INTERNAL_RESET, TESTING_INTERNAL_RED, testing_internal_failed, TESTING_INTERNAL_RESET, TESTING_INTERNAL_CYAN, TESTING_INTERNAL_RESET);
        return testing_internal_failed;
    }

    uint64_t testing_internal_test_run_v(void (*test_function)(void), const char* file) {
        if (!test_function || !file)
            return UINT64_MAX;
        testing_internal_lock_run();
    #if defined(_WIN32) || defined(_WIN64)
        testing_internal_enable_windows_colors();
    #endif
        testing_internal_passed = (uint64_t)0ULL;
        testing_internal_failed = (uint64_t)0ULL;
        testing_internal_test_start(file);
        test_function();
        uint64_t r = testing_internal_test_end();
    #if defined(_WIN32) || defined(_WIN64)
        testing_internal_restore_windows_mode();
    #endif
        testing_internal_unlock_run();
        return r;
    }

#endif /* TESTING_DEFINE */

#endif /* UNITTEST_TESTING_H */