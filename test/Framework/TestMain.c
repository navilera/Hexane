#include "TestMain.h"

TestUnit_t testUnit = {0};


static void InitTestUnit(void);
static void RunTest(void);
static void ReportTestResult(void);

int main(int argc, char* argv[])
{
    InitTestUnit();
    RunTest();
    ReportTestResult();

    return 0;
}

void AddTestSuite(TestSuite_t* testSuite)
{
    // Add to head
    testSuite->next = testUnit.testSuitsList;
    testUnit.testSuitsList = testSuite;
}

TestCase_t* MakeTestCase(TestCaseFunc_t testFunc, char* name)
{
    TestCase_t* testCase;
    testCase = (TestCase_t*)malloc(sizeof(TestCase_t));
    memset(testCase, 0, sizeof(TestCase_t));

    testCase->name = (char*)malloc(strlen(name) + 1);
    memset(testCase->name, 0, strlen(name) + 1);
    strncpy(testCase->name, name, strlen(name));

    testCase->testFunc = testFunc;

    return testCase;
}

TestCase_t* AddTestCaseObj(TestSuite_t* testSuite, TestCase_t* testCase)
{
	testSuite->totalTestCaseNum++;

    // Add to tail
    if(testSuite->testCaseList == NULL)
    {
        testSuite->testCaseList = testCase;
        testCase->next = NULL;
        return testCase;
    }

    TestCase_t* testCasePtr;
    for(testCasePtr = testSuite->testCaseList ; testCasePtr->next != NULL ; testCasePtr = testCasePtr->next);
    testCasePtr->next = testCase;
    testCase->next = NULL;

    return testCase;
}

TestCase_t* AddTestCase(TestSuite_t* testSuite, TestCaseFunc_t testFunc, char* name)
{
    TestCase_t* testCase = MakeTestCase(testFunc, name);
    return AddTestCaseObj(testSuite, testCase);
}

static void InitTestUnit(void)
{
    TestSuite_t* testSuitePtr;
    int testSuiteCount = 0;

    for(testSuitePtr = testUnit.testSuitsList; testSuitePtr != NULL; testSuitePtr = testSuitePtr->next)
    {
        if(testSuitePtr->initFunc != NULL)
        {
            printf("====== Initialize Test Suite.. %d\n", ++testSuiteCount);
            testSuitePtr->initFunc(testSuitePtr);
            printf("Test suite name: %s\n", testSuitePtr->name);
            printf("This test suite has %d test cases. Initialize done.\n", testSuitePtr->totalTestCaseNum);
        }
    }
}

static void RunTest(void)
{
   TestSuite_t* testSuitePtr;
    for(testSuitePtr = testUnit.testSuitsList; testSuitePtr != NULL; testSuitePtr = testSuitePtr->next)
    {
        uint32_t caseCount = 0;
        bool anyFail = false;
        TestCase_t* testCasePtr;

        testUnit.totalSuites++;

        printf("==== Test suite [%s] Start ====\n", testSuitePtr->name);
        if(testSuitePtr->preTestFunc != NULL)
        {
            printf("===== PreTest Start =====\n");
            testSuitePtr->preTestFunc();
            printf("===== PreTest Done =====\n");
        }

        for(testCasePtr = testSuitePtr->testCaseList ; testCasePtr != NULL ; testCasePtr = testCasePtr->next)
        {
            printf(TESTCASEINDENT "Case %d [%s] ... ", caseCount++, testCasePtr->name);

            if(testCasePtr->testFunc != NULL)
            {
                testUnit.totalCases++;

                testCasePtr->result = testCasePtr->testFunc();
                if(testCasePtr->result)
                {
                    printf("PASS\n");
                    testUnit.totalPass++;
                }
                else
                {
                    printf(TESTCASEINDENT "FAIL\n");
                    anyFail = true;
                    testUnit.totalFail++;
                }
            }
        }

        if(testSuitePtr->postTestFunc != NULL)
        {
            printf("===== PostTest Start =====\n");
            testSuitePtr->postTestFunc();
            printf("===== PostTest Done =====\n");
        }
        printf("==== Test suite [%s] %s ====\n", testSuitePtr->name, (anyFail?"Fail":"Pass!"));
    }
}

static void ReportTestResult(void)
{
	printf("------------------------------\n");
	printf("Report\n");
	printf("------------------------------\n");
	printf("Total.....................%04d\n", testUnit.totalCases);
	printf("Pass......................%04d\n", testUnit.totalPass);
	printf("Fail......................%04d\n", testUnit.totalFail);
}


