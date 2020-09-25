include ../Make.Rules

all: BsDicWordDsl.o BsDicFrmt.o BsDicIdx.o BsDicIdxAb.o BsDicIwrds.o BsDicIdxIrtRaw.o BsDiIx.o BsDiIxTx.o BsDiIxT2.o BsDicLsa.o BsDiIxFind.o BsDicDescr.o BsDicDescrDsl.o BsDicObj.o BsDictSettings.o BsDicHist.o BsDict

BsDicWordDsl.o: BsDicWordDsl.c BsDicWordDsl.h BsDicWord.h
	$(CC) -I. -I../bslib -c BsDicWordDsl.c -o $@ $(CFLAGS)

BsDicFrmt.o: BsDicFrmt.c BsDicFrmt.h
	$(CC) -I. -I../bslib -c BsDicFrmt.c -o $@ $(CFLAGS)

BsDicIdx.o: BsDicIdx.c BsDicIdx.h
	$(CC) -I. -I../bslib -c BsDicIdx.c -o $@ $(CFLAGS)

BsDicIdxAb.o: BsDicIdxAb.c BsDicIdxAb.h BsDicIdx.h BsDicIdx.o
	$(CC) -I. -I../bslib -c BsDicIdxAb.c -o $@ $(CFLAGS)

BsDicIwrds.o: BsDicIwrds.c BsDicIwrds.h BsDicIdx.h BsDicIdx.o
	$(CC) -I. -I../bslib -c BsDicIwrds.c -o $@ $(CFLAGS)

BsDicIdxIrtRaw.o: BsDicIdxIrtRaw.c BsDicIdxIrtRaw.h
	$(CC) -I. -I../bslib -c BsDicIdxIrtRaw.c -o $@ $(CFLAGS)

BsDiIx.o: BsDiIx.c BsDiIx.h
	$(CC) -I. -I../bslib -c BsDiIx.c -o $@ $(CFLAGS)

BsDiIxTx.o: BsDiIxTx.c BsDiIxTx.h BsDicIdxIrtRaw.o
	$(CC) -I. -I../bslib -c BsDiIxTx.c -o $@ $(CFLAGS)

BsDiIxT2.o: BsDiIxT2.c BsDiIxT2.h
	$(CC) -I. -I../bslib -c BsDiIxT2.c -o $@ $(CFLAGS)

BsDicLsa.o: BsDicLsa.c BsDicLsa.h
	$(CC) -I. -I../bslib -c BsDicLsa.c -o $@ $(CFLAGS)

BsDiIxFind.o: BsDiIxFind.c BsDiIxFind.h BsDiIxTx.o
	$(CC) -I. -I../bslib -c BsDiIxFind.c -o $@ $(CFLAGS)

BsDicDescr.o: BsDicDescr.c BsDicDescr.h BsDiIxTx.o
	$(CC) -I. -I../bslib -c BsDicDescr.c -o $@ $(CFLAGS)

BsDicDescrDsl.o: BsDicDescrDsl.c BsDicDescrDsl.h BsDicDescr.o
	$(CC) -I. -I../bslib -c BsDicDescrDsl.c -o $@ $(CFLAGS)

BsDicObj.o: BsDicObj.c BsDicObj.h BsDicDescrDsl.o
	$(CC) -I. -I../bslib -c BsDicObj.c -o $@ $(CFLAGS)

BsDictSettings.o: BsDictSettings.c BsDictSettings.h BsDicObj.o
	$(CC) -I. -I../bslib -c BsDictSettings.c -o $@ $(CFLAGS) `pkg-config gtk+-2.0 --cflags`

BsDicHist.o: BsDicHist.c BsDicHist.h
	$(CC) -I. -I../bslib -c BsDicHist.c -o $@ $(CFLAGS) `pkg-config gtk+-2.0 --cflags`

BsDict: BsDict.c BsDictSettings.o BsDicHist.o
	$(CC) -I. -I../bslib -c $@.c -o $@.o $(CFLAGS) `pkg-config gtk+-2.0 --cflags`
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsI18N.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../bslib/BsFioWrap.o ../bslib/BsDataSet.o ../bslib/BsStrings.o ../bslib/BsIntSet.o BsDicFrmt.o BsDicIdx.o BsDicIdxAb.o BsDicWordDsl.o BsDicIwrds.o BsDicIdxIrtRaw.o BsDiIxTx.o BsDiIx.o BsDiIxT2.o BsDicLsa.o BsDiIxFind.o BsDicDescr.o BsDicDescrDsl.o BsDicObj.o BsDicHist.o BsDictSettings.o -o $@ $(LDFLAGS) -logg -lvorbis -lvorbisfile -lvorbisenc -pthread `pkg-config gtk+-2.0 --libs`

clean:
	rm -f *.o BsDict
