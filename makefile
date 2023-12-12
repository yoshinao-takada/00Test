CC=gcc
.PHONY	:	test0
.PHONY	:	commandsUT
.PHONY	:	crcUT
.PHONY	:	CRCCli
.PHONY	:	CRCSvr
.PHONY	:	clean
CRC_TRACE_FLAG = -DCRC_TRACE_ON
#CRC_TRACE_FLAG =

test0	:	test0.exe
commandsUT	:	commandsUT.exe
crcUT	:	crcUT.exe
CRCCli	:	CRCCli.exe
CRCSvr	:	CRCSvr.exe

test0.exe	:	test0.o commands.o
	$(CC) -o $@ $^

commandsUT.exe	:	commandsUT.o commands.o
	$(CC) -o $@ $^

crcUT.exe	:	crcUT.o CRC.o
	$(CC) -o $@ $^

CRCCli.exe	:	CRCCli.o CRC.o CRCCommon.o
	$(CC) -o $@ $^

CRCSvr.exe	:	CRCSvr.o CRC.o CRCCommon.o
	$(CC) -o $@ $^

test0.o	:	test0.c commands.h common.h
	$(CC) -c -g $<

commandsUT.o	:	commandsUT.c commands.h common.h
	$(CC) -c -g $<

commands.o	:	commands.c commands.h common.h
	$(CC) -c -g $<

crcUT.o	:	crcUT.c CRC.h
	$(CC) $(CRC_TRACE_FLAG) -c -g $<

CRC.o	:	CRC.c CRC.h
	$(CC) $(CRC_TRACE_FLAG) -c -g $<

CRCCommon.o	:	CRCCommon.c CRCCommon.h
	$(CC) -c -g $<

CRCCli.o	:	CRCCli.c
	$(CC) -c -g $<

CRCSvr.o	:	CRCSvr.c
	$(CC) -c -g $<

clean	:
	rm *.o
	rm *.exe