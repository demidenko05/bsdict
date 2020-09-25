include ../Make.Rules

all: tst_BsDicWordDsl tst_BsDicFrmt tst_BsDicIdxAb tst_BsDicIdxAbMatch tst_BsDicWordDslBigest tst_BsDicIwrds tst_BsDicIdxIrtRaw tst_BsDicIdxIrtRawBig tst_BsDicIwrdsBig tst_BsDiIxTx tst_BsDiIxFind tst_BsDiIxFindBig tst_BsDiIxFindBigFile tst_BsDicDescrDsl tst_BsDicLsa

tst_BsDicWordDsl: tst_BsDicWordDsl.c
	$(CC) -I../dict -I../bslib -c tst_BsDicWordDsl.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../dict/BsDicWordDsl.o -o $@ $(LDFLAGS)

tst_BsDicFrmt: tst_BsDicFrmt.c
	$(CC) -I../dict -I../bslib -c tst_BsDicFrmt.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../dict/BsDicFrmt.o -o $@ $(LDFLAGS)

tst_BsDicIdxAb: tst_BsDicIdxAb.c
	$(CC) -I../dict -I../bslib -c tst_BsDicIdxAb.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o ../dict/BsDicWordDsl.o -o $@ $(LDFLAGS)

tst_BsDicIdxAbMatch: tst_BsDicIdxAbMatch.c
	$(CC) -I../dict -I../bslib -c tst_BsDicIdxAbMatch.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o ../dict/BsDicWordDsl.o -o $@ $(LDFLAGS)

tst_BsDicWordDslBigest: tst_BsDicWordDslBigest.c
	$(CC) -I../dict -I../bslib -c tst_BsDicWordDslBigest.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../dict/BsDicWordDsl.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o -o $@ $(LDFLAGS)

tst_BsDicIwrds: tst_BsDicIwrds.c
	$(CC) -I../dict -I../bslib -c tst_BsDicIwrds.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../bslib/BsDataSet.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o ../dict/BsDicWordDsl.o ../dict/BsDicIwrds.o -o $@ $(LDFLAGS)

tst_BsDicIwrdsBig: tst_BsDicIwrdsBig.c
	$(CC) -I../dict -I../bslib -c tst_BsDicIwrdsBig.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o ../dict/BsDicWordDsl.o ../dict/BsDicIwrds.o ../bslib/BsDataSet.o -o $@ $(LDFLAGS)

tst_BsDicIdxIrtRaw: tst_BsDicIdxIrtRaw.c
	$(CC) -I../dict -I../bslib -c tst_BsDicIdxIrtRaw.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o ../dict/BsDicWordDsl.o ../dict/BsDicIwrds.o ../dict/BsDicIdxIrtRaw.o ../bslib/BsDataSet.o -o $@ $(LDFLAGS)

tst_BsDicIdxIrtRawBig: tst_BsDicIdxIrtRawBig.c
	$(CC) -I../dict -I../bslib -c tst_BsDicIdxIrtRawBig.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o ../dict/BsDicWordDsl.o ../dict/BsDicIwrds.o ../dict/BsDicIdxIrtRaw.o ../bslib/BsDataSet.o -o $@ $(LDFLAGS)

tst_BsDiIxTx: tst_BsDiIxTx.c
	$(CC) -I../dict -I../bslib -c tst_BsDiIxTx.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../dict/BsDicFrmt.o ../bslib/BsStrings.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o ../dict/BsDicWordDsl.o ../dict/BsDicIwrds.o ../dict/BsDicIdxIrtRaw.o ../dict/BsDiIx.o ../dict/BsDiIxTx.o ../bslib/BsDataSet.o ../bslib/BsFioWrap.o -o $@ $(LDFLAGS)

tst_BsDicLsa: tst_BsDicLsa.c
	$(CC) -I../dict -I../bslib -c tst_BsDicLsa.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../bslib/BsDataSet.o ../bslib/BsStrings.o ../bslib/BsIntSet.o ../bslib/BsFioWrap.o ../dict/BsDicFrmt.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o ../dict/BsDicWordDsl.o ../dict/BsDicIwrds.o ../dict/BsDicIdxIrtRaw.o ../dict/BsDiIxTx.o ../dict/BsDiIx.o ../dict/BsDiIxT2.o ../dict/BsDicDescr.o ../dict/BsDicLsa.o -o $@ -logg -lvorbis -lvorbisfile -lvorbisenc $(LDFLAGS)

tst_BsDiIxFind: tst_BsDiIxFind.c
	$(CC) -I../dict -I../bslib -c tst_BsDiIxFind.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../bslib/BsFioWrap.o ../bslib/BsDataSet.o ../bslib/BsStrings.o ../dict/BsDicFrmt.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o ../dict/BsDicWordDsl.o ../dict/BsDicIwrds.o ../dict/BsDicIdxIrtRaw.o ../dict/BsDiIx.o ../dict/BsDiIxTx.o ../dict/BsDiIxFind.o -o $@ $(LDFLAGS)

tst_BsDiIxFindBig: tst_BsDiIxFindBig.c
	$(CC) -I../dict -I../bslib -c tst_BsDiIxFindBig.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../bslib/BsFioWrap.o ../bslib/BsDataSet.o ../bslib/BsStrings.o ../dict/BsDicFrmt.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o ../dict/BsDicWordDsl.o ../dict/BsDicIwrds.o ../dict/BsDicIdxIrtRaw.o ../dict/BsDiIx.o ../dict/BsDiIxTx.o ../dict/BsDiIxFind.o -o $@ $(LDFLAGS)

tst_BsDiIxFindBigFile: tst_BsDiIxFindBigFile.c
	$(CC) -I../dict -I../bslib -c tst_BsDiIxFindBigFile.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../bslib/BsFioWrap.o ../bslib/BsDataSet.o ../bslib/BsStrings.o ../dict/BsDicFrmt.o ../dict/BsDicIdx.o ../dict/BsDicIdxAb.o ../dict/BsDicWordDsl.o ../dict/BsDicIwrds.o ../dict/BsDicIdxIrtRaw.o ../dict/BsDiIx.o ../dict/BsDiIxTx.o ../dict/BsDiIxFind.o -o $@ $(LDFLAGS)

tst_BsDicDescrDsl: tst_BsDicDescrDsl.c
	$(CC) -I../dict -I../bslib -c tst_BsDicDescrDsl.c -o $@.o $(CFLAGS)
	$(LD) $@.o ../bslib/BsError.o ../bslib/BsLog.o ../bslib/BsFatalLog.o ../bslib/BsDataSet.o ../bslib/BsStrings.o ../bslib/BsIntSet.o ../bslib/BsFioWrap.o ../dict/BsDicDescr.o ../dict/BsDicDescrDsl.o -o $@ $(LDFLAGS)

test: tst_BsDicWordDsl tst_BsDicFrmt tst_BsDicIdxAb tst_BsDicIdxAbMatch tst_BsDicWordDslBigest tst_BsDicIwrds tst_BsDicIdxIrtRaw tst_BsDiIxTx tst_BsDiIxFind tst_BsDiIxFindBig
	./tst_BsDicWordDsl
	./tst_BsDicFrmt
	./tst_BsDicIdxAb
	./tst_BsDicIdxAbMatch
	./tst_BsDicIwrds
	./tst_BsDicIwrdsBig
	./tst_BsDicIdxIrtRaw
	./tst_BsDiIxTx
	./tst_BsDiIxFind
	./tst_BsDicDescrDsl

test_descr_dsl: tst_BsDicDescrDsl 
	./tst_BsDicDescrDsl "$(BIGDICPTH)" $(OFST)

test_big_dsl: tst_BsDicWordDslBigest
	rm -f *.diwo *.diwno *.irt
	./tst_BsDicWordDslBigest "$(BIGDICPTH)" $(BIGDICMAX) $(BIGDICBUFSIZE)

test_big_find: tst_BsDiIxFindBig 
	rm -f *.diwo *.diwno *.irt
	./tst_BsDiIxFindBig "$(BIGDICPTH)" "$(BIGDICSUBWRD)" $(IRT_IDX_START_R) $(IRT_IDX_END_R)

test_big_find_file: tst_BsDiIxFindBigFile 
	rm -f *.diwo *.diwno *.irt
	./tst_BsDiIxFindBigFile "$(BIGDICPTH)" "$(BIGDICSUBWRD)" $(IRT_IDX_START_R) $(IRT_IDX_END_R)

test_big_irt: tst_BsDicIdxIrtRawBig
	rm -f *.diwo *.diwno *.irt
	./tst_BsDicIdxIrtRawBig "$(BIGDICPTH)" $(BIGDICABSZ)

test_big_iwords: tst_BsDicIwrdsBig
	rm -f *.diwo *.diwno *.irt
	./tst_BsDicIwrdsBig "$(BIGDICPTH)" $(BIGDICABSZ) $(BIGWRDSPRN)

test_lsa: tst_BsDicLsa
	./tst_BsDicLsa "$(BIGDICPTH)" $(RECOFST) $(RECLEN) $(WORD)

clean:
	rm -f *.log *.pcm *wav *.ogg *.lg2 *.o *.diwo *.diwno *.irt *.idx tst_BsDicWordDsl tst_BsDicWordDslBigest tst_BsDicFrmt tst_BsDicIdxAb tst_BsDicIdxAbMatch tst_BsDicIwrds tst_BsDicIdxIrtRaw tst_BsDicIdxIrtRawBig tst_BsDicIwrdsBig tst_BsDiIxTx tst_BsDiIxFind tst_BsDiIxFindBig tst_BsDiIxFindBigFile tst_BsDicDescrDsl tst_BsDicLsa
