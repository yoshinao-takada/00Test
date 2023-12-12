#define _MAIN_C
#include "common.h"
#include "commands.h"

static int g_QuitRequest = 0;

static const CommandEntry_t CMDTBL[] =
{
    { "c", MyClose, &g_IoParams, &g_IoParams, NoneParser, NULL, NULL },
    { "ob", MyOpenBlk, &g_IoParams.fd, &g_OpenParams,
        MyOpenBlkParser, &g_OpenParams, g_buffer },
    { "onb", MyOpenNBlk, &g_IoParams.fd, &g_OpenParams,
        MyOpenNBlkParser, &g_OpenParams, g_buffer },
    { "r", MyRead, g_buffer, &g_IoParams,
        MyReadParser, &g_IoParams, g_buffer },
    { "w", MyWrite, NULL, &g_IoParams, 
        MyWriteParser, &g_IoParams, g_buffer },
    { "q", MyQuit, &g_QuitRequest, NULL, NoneParser, NULL, NULL },
    { "h", MyHelp, NULL, NULL, NoneParser, NULL, NULL },
    { NULL, NoneCommand, NULL, g_buffer, NoneParser, NULL, NULL } // end element
};

/*!
\brief trim new line character at the end of line if exists.
Note: fgets() keeps a newline character in each input.
trimEndOfLine() is applied to remove the new line character
after fgets() is called.
\param buffer [in,out] character buffer of fgets().
*/
void
trimEndOfLine(char* buffer)
{
    for (; *buffer != '\0'; buffer++)
    {
        if (*buffer == '\n')
        {
            *buffer = '\0';
            break;
        }
    }
}

int
main(int argc, const char* argv[])
{
    int err = EXIT_SUCCESS;
    const char* getsResult = NULL;
    CommandEntry_cpt ce = NULL;
    while (g_QuitRequest == 0)
    {
        fputs("test0.exe> ", stdout);
        memset(g_buffer, '\0', ARRAYSIZE(g_buffer));
        getsResult = fgets(g_buffer, G_BUFFER_SIZE, stdin);
        if (getsResult != g_buffer)
        {
            err = errno;
            fprintf(stderr,"Fail in gets(), errno = 0x%04x(%d)\n",
                err, err);
            break;
        }
        trimEndOfLine(g_buffer);
        for (ce = CMDTBL; ce->name; ce++)
        {
            if (strncmp(ce->name, g_buffer, strlen(ce->name)) == 0)
            {
                break;
            }
        }
        if (EXIT_SUCCESS != ce->parser(ce->parseOut, ce->parseIn))
        {

            continue;
        }
        ce->handler(ce->out, ce->in);
    }
    // close the file if it is opened.
    if (g_IoParams.fd > 0)
    {
        close(g_IoParams.fd);
        g_IoParams.fd = -1;
    }
    return err;
}
