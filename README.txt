Beigesoft™ BsDict is GTK2 based offline dictionary.

DEBUG LEVEL from 20000 ... 49999

To build it:
0) of course, you should have installed GTK2, make, gcc, ld, pkg-config...:
	apt-get install libvorbis-dev
  or install libogg and libvorbis from source:
  https://xiph.org/downloads/ libogg-1.3.4.tar.xz then libvorbis-1.3.7.tar.gz
1) unpack BsDict and make:
 make
 #as root:
 make install-strip
2) open BsDict, add distionaries DSL files, use "dictzip -d *.dsl.dz" for unzipping DSL file.

to install dictzip:
apt-get/dnf install dictzip

3) You have to convert UTF-16 into UTF-8, it will also reduce file size almost to twice:
iconv -f UTF-16 -t UTF-8 < inpututf16.dsl > outpututf8.dsl

4) To play LSA sound "ffplay (FFMPEG)" must be installed
------------------------------------------
features:
* support DSL, DSA dictionaries
* history doesn't allow duplicates
* it always saves history on exit
* export/replace/add history
* delete/move history's items
* toggle history sorting: historically + manual/alphabetical
* searching word by double-click


* index file is not cached by default (LSA IDX in RAM is not yet implemented, and so does interface to switch using index in file or in memory)

rationale of having a dictionary non-zipped and index file non-cached:
  nowadays a lot of notebooks, tablets and smartphones have non-HDD storage,
  i.e. files are placed in RAM (random access memory).
  So, unzipping and caching never help in this way.
  Computers with HDD already cache files.

! if a ZIPPED dictionary allows to seek a record (word's content) without unzipping, then it's more optimal approach.
  This is usually done by zipping only word's definition (content), i.e. head-words (keys) and file structure stay unzipped.
  For example PDF has same approach - only sub-contents (e.g. page's text) zipped, not headers (their keys/description).
  But dictzip uses actually GZIP archiving of whole file, so you have additional memory consumption on zipped dictionaries.

It has no "internal" sound-player yet, i.e. it invokes external player to play sound file placed into TMP.
But, you should never get sad, because this approach seems to be good (minimal system overhead)".

Partially implemented "history contains of data pointers" (i.e. offsets and other data to find a content without the IDX file).

------------------------------------------
licenses:
BSD 2-Clause License
https://sites.google.com/site/beigesoftware/bsd2csl

3-d party licenses for files BsDicLsa.c:

THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007
by the Xiph.Org Foundation https://xiph.org/
BSD 3-Clause License
see COPYING.vorbis file

2008-2012 Konstantin Isakov <ikm@goldendict.org>
Licensed under GPLv3 or later
see LICENSE.goldendict file
------------------------------------------

sponsored by Beigesoft™ Enterprise Information System:
https://www.facebook.com/Beigesoft-Accounting-All-In-One-40115090067724
 or
https://sites.google.com/site/beigesoftware

Многие предприниматели записывают в блокнот или электронную таблицу информацию рода:
* кто и сколько мне должен
* кому и сколько я должен
* за сколько я могу продать оборудование прямо сейчас
...
Читайте как наиболее эффективно вести коммерческий бухгалтерский учет:
https://sites.google.com/site/beigesoftware/eis-iv

Yesterday you sold merchandise in foreign currency, and exchange rate was 1.5.
Today you received payment in foreign currency, and exchange rate is 1.2.
How to properly record this?
Read more about exchange gain or loss:
https://sites.google.com/site/beigesoftware/eis-bobs-pizza/eis-multicurrency
