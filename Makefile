include Make.Rules

all clean:
	$(MAKE) -C bslib $@
	$(MAKE) -C dict $@
	$(MAKE) -C test $@
	rm -f *.log *.pcm *.wav *.ogg

test: all
	$(MAKE) -C bslib $@
	$(MAKE) -C test $@

test_big_dsl: all
	$(MAKE) -C test $@

test_big_find: all
	$(MAKE) -C test $@

test_big_find_file: all
	$(MAKE) -C test $@

test_big_irt: all
	$(MAKE) -C test $@

test_big_iwords: all
	$(MAKE) -C test $@

test_descr_dsl: all
	$(MAKE) -C test $@

test_lsa: all
	$(MAKE) -C test $@

install: all
	install -d $(DESTDIR)$(PREFIX)/bin
	install -v dict/BsDict $(DESTDIR)$(PREFIX)/bin
	install -d $(DESTDIR)$(PREFIX)/share/applications
	install -vm644 BsDict.desktop $(DESTDIR)$(PREFIX)/share/applications
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/16x16/apps
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/24x24/apps
	install -vm644 icons/16x16/* $(DESTDIR)$(PREFIX)/share/icons/hicolor/16x16/apps
	install -vm644 icons/24x24/* $(DESTDIR)$(PREFIX)/share/icons/hicolor/24x24/apps

install-strip: all
	install -d $(DESTDIR)$(PREFIX)/bin
	install -vs dict/BsDict $(DESTDIR)$(PREFIX)/bin
	install -d $(DESTDIR)$(PREFIX)/share/applications
	install -vm644 BsDict.desktop $(DESTDIR)$(PREFIX)/share/applications
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/16x16/apps
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/24x24/apps
	install -vm644 icons/16x16/* $(DESTDIR)$(PREFIX)/share/icons/hicolor/16x16/apps
	install -vm644 icons/24x24/* $(DESTDIR)$(PREFIX)/share/icons/hicolor/24x24/apps
