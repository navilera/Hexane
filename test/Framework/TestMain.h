#ifndef _TEST_MAIN_
#define _TEST_MAIN_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
 
#define REGISTER_SUITE_FUNC(suiteName, init_func) \
    __attribute__ ((__constructor__)) \
    void RegisterSuite ## suiteName(void) \
    { TestSuite_t* suite = (TestSuite_t*)malloc(sizeof(TestSuite_t)); memset(suite, 0, sizeof(TestSuite_t)); suite->initFunc = init_func; AddTestSuite(suite); }

#define TESTCASEINDENT  "  "
#define TESTCASEMSGINDENT "    "

#define ASSERTMSG_INT_FAIL(expect, actual)  "FAIL! expect 0x%lx actual 0x%lx\n", (uint64_t)expect, (uint64_t)actual
#define ASSERTMSG_STR_FAIL(expect, actual)  "FAIL! expect %s actual %s", expect, actual

#define ASSERT(cond, msgfmt) if(!(cond)){printf("\n" TESTCASEMSGINDENT msgfmt);} return (cond);

struct _test_suite_t_;
typedef void (*InitSuite_t)(struct _test_suite_t_ *);
typedef bool (*TestCaseFunc_t)(void);

typedef struct _test_case_t_
{
    char*           name;
    TestCaseFunc_t  testFunc;
    bool            result;
    struct _test_case_t_ *next;
} TestCase_t;

typedef struct _test_suite_t_
{
    InitSuite_t     initFunc;
    TestCaseFunc_t  preTestFunc;
    TestCaseFunc_t  postTestFunc;
    TestCase_t*     testCaseList;
    char*           name;
    uint32_t        totalTestCaseNum;
    struct _test_suite_t_ *next;
} TestSuite_t;

typedef struct _test_unit_t_
{
    TestSuite_t* testSuitsList;
    uint32_t     totalSuites;
    uint32_t     totalCases;
    uint32_t     totalPass;
    uint32_t     totalFail;
} TestUnit_t;

void AddTestSuite(TestSuite_t* testSuite);
TestCase_t* MakeTestCase(TestCaseFunc_t testFunc, char* name);
TestCase_t* AddTestCaseObj(TestSuite_t* testSuite, TestCase_t* testCase);
TestCase_t* AddTestCase(TestSuite_t* testSuite, TestCaseFunc_t testFunc, char* name);

#endif

