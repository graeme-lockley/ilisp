#ifndef MINUNIT_H
#define MINUNIT_H

/*
 * Based on: http://www.jera.com/techinfo/jtns/jtn002.html
 */

#define mu_assert(test, message) \
    do                           \
    {                            \
        if (!(test))             \
            return message;      \
    } while (0)

#define mu_assert_label(test) mu_assert(test, #test)

#define mu_run_test(test)       \
    do                          \
    {                           \
        char *message = test(); \
        tests_run++;            \
        if (message)            \
        {                       \
            return message;     \
        }                       \
        else                    \
        {                       \
            tests_passed += 1;  \
        }                       \
    } while (0)

extern int tests_run;
extern int tests_passed;

#endif /* !MINUNIT_H */
