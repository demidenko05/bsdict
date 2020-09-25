include ../Make.Rules

all: BsError BsI18N BsLog BsFioWrap BsFatalLog BsDataSet BsStrings BsIntSet tst_BsLog tst_BsLogNthr tst_BsDataSet

BsError: BsError.c BsError.h
	$(CC) -I. -c $@.c -o $@.o $(CFLAGS)

BsI18N: BsI18N.c BsI18N.h
	$(CC) -I. -c $@.c -o $@.o $(CFLAGS)

BsLog: BsLog.c BsLog.h BsError
	$(CC) -I. -c $@.c -o $@.o $(CFLAGS)

BsFatalLog: BsFatalLog.c BsFatalLog.h BsLog
	$(CC) -I. -c $@.c -o $@.o $(CFLAGS)

BsFioWrap: BsFioWrap.c BsFioWrap.h BsLog
	$(CC) -I. -c $@.c -o $@.o $(CFLAGS)

BsDataSet: BsDataSet.c BsDataSet.h BsFatalLog
	$(CC) -I. -c $@.c -o $@.o $(CFLAGS)

BsStrings: BsStrings.c BsStrings.h BsFatalLog
	$(CC) -I. -c $@.c -o $@.o $(CFLAGS)

BsIntSet: BsIntSet.c BsIntSet.h BsFatalLog
	$(CC) -I. -c $@.c -o $@.o $(CFLAGS)

tst_BsLog: tst_BsLog.c BsLog
	$(CC) -I. -c $@.c -o $@.o $(CFLAGS)
	$(LD) $@.o BsError.o BsLog.o -o $@ $(LDFLAGS) -pthread

tst_BsLogNthr: tst_BsLogNthr.c BsLog.c BsLog.h
	$(CC) -I. -c $@.c -o $@.o $(CFLAGS)
	$(LD) $@.o BsError.o BsLog.o -o $@ $(LDFLAGS) -pthread

tst_BsDataSet: tst_BsDataSet.c BsFatalLog
	$(CC) -I. -c $@.c -o $@.o $(CFLAGS)
	$(LD) $@.o BsError.o BsLog.o BsFatalLog.o BsDataSet.o BsStrings.o -o $@ $(LDFLAGS) -pthread

test: all
	./tst_BsLog
	./tst_BsLogNthr
	./tst_BsDataSet

clean:
	rm -f *.o *.log *.lg2 tst_BsLog tst_BsLogNthr tst_BsDataSet
