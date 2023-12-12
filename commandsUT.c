#include "common.h"
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int MyOpenBlkParserUT()
{
    static const char* TEST_STRINGS[] =
    {
        "o1  9600  /dev/ttyS0",
        "o1 115200 /dev/ttyUSB0",
        "o1 130 /dev/ttyS1",
    };
    static const OpenParams_t REF[] =
    {
        { B9600, "/dev/ttyS0" },
        { B115200, "/dev/ttyUSB0" },
        { B0, "/dev/ttyS1" }
    };

    int err = EXIT_SUCCESS;
    for (int i = 0; i < ARRAYSIZE(TEST_STRINGS); i++)
    {
        OpenParams_t params;
        err = MyOpenBlkParser(&params, TEST_STRINGS[i]);
        if (i == 2 && err == EBADF) { err = EXIT_SUCCESS; continue; }        
        IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__, "");
        if ((REF[i].baud != params.baud) || 
            strcmp(REF[i].devname, params.devname))
        {
            err = EXIT_FAILURE;
            IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__, "");
        }
    }
    TEST_END(err, __FILE__, __FUNCTION__, __LINE__);
    return err;
}

static int MyReadParserUT()
{
    static const char* TEST_STRINGS[] =
    {
        "r -1",
        "r 0",
        "r 63",
    };
    static const IoParams_t REF[] =
    {
        {-1, -1, { '\0' }},
        {-1, 0, { '\0' }},
        {-1, 63, { '\0' }},
    };
    int err = EXIT_SUCCESS;
    for (int i = 0; i < ARRAYSIZE(TEST_STRINGS); i++)
    {
        IoParams_t params;
        err = MyReadParser(&params, TEST_STRINGS[i]);
        IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__, "");
        if (REF[i].count != params.count)
        {
            err = EXIT_FAILURE;
            IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__, "");
        }
    }
    TEST_END(err, __FILE__, __FUNCTION__, __LINE__);
    return err;
}

static int MyWriteParserUT()
{
    static const char* TEST_STRINGS[] =
    {
        "w 10 HelloWorld",
        "w 0 A\r\nBC\r\n\\DE"
    };
    static const IoParams_t REF[] = 
    {
        { -1, 10, "HelloWorld" },
        { -1, 0, "A\r\nBC\r\n\\DE"}
    };
    int err = EXIT_SUCCESS;
    for (int i = 0; i < ARRAYSIZE(TEST_STRINGS); i++)
    {
        IoParams_t params;
        err = MyWriteParser(&params, TEST_STRINGS[i]);
        IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__, "");
        if (REF[i].count != params.count ||
            strcmp(REF[i].buffer, params.buffer))
        {
            err = EXIT_FAILURE;
            IF_TEST_FAIL_BREAK(err, __FILE__, __FUNCTION__, __LINE__, "");
        }
    }
    TEST_END(err, __FILE__, __FUNCTION__, __LINE__);
    return err;
}

int main(int argc, char* argv[])
{
    int err = EXIT_SUCCESS;
    do
    {
        err = MyOpenBlkParserUT();
        if (err) break;
        err = MyReadParserUT();
        if (err) break;
        err = MyWriteParserUT();
        if (err) break;
    }
    while (0);
    TEST_END(err, __FILE__, __FUNCTION__, __LINE__);
    return err;
}