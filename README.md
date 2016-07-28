
# zstdmt

zstdmt depends currently on posix threads for multi-threading and is
only tested on linux

I will add support for windows and CreateThread() together with
WaitForMultipleObjects() / WaitForSingleObject() later...

## Building for Linux with gcc

 - Run `make`.

## Building for Linux with Clang

 - Run `make CC=clang`

## Overview

 - three tools:
   - `./zstd-st` - single threaded multi stram mode
   - `./zstd-mt` - multi threaded via phread_create / pthread_join
   - `./zstd-mt2` - multi threaded via X workers that read/compress/write
     (decompress has to be changed a bit)

## See also

 - [zstd Extremely Fast Compression algorithm](https://github.com/Cyan4973/zstd)
 - [7-Zip with zstd support](https://github.com/mcmilk/7-Zip-Zstd)

## Tables with some first testing...

 - System: Arch Linux, Kernel 4.3.6-ck
 - Intel(R) Core(TM) i7-3632QM, limited to 1,2 .. 1,8 GHz
 - data.tar contains the tarred silesia directory

Level|Threads|InSize|OutSize|Frames|Real|User|Sys|MaxMem
-----|-------|------|-------|------|----|----|---|-------
ZSTD-L1|1|211957760|73656326|1618|2,147|2,105|0,37|2648
ST-L1|1|211957760|73662797|1618|1,991|1,955|0,33|2564
MT-L1|1|211957760|73662797|1618|2,82|1,392|0,2|2616
MT2-L1|1|211957760|73662815|1618|1,973|1,928|0,40|2444
ST-L1|2|211957760|74127588|1618|2,2|1,964|0,35|3848
MT-L1|2|211957760|74127588|1618|1,167|1,435|0,4|3588
MT2-L1|2|211957760|74103077|1618|1,664|3,272|0,47|4084
ST-L1|4|211957760|74472999|1618|2,4|1,973|0,27|5932
MT-L1|4|211957760|74472999|1618|0,982|1,498|0,6|6072
MT2-L1|4|211957760|74451798|1618|0,563|2,201|0,38|6996
ST-L1|6|211957760|74582082|1618|2,96|2,53|0,39|8244
MT-L1|6|211957760|74582082|1618|0,831|1,877|0,14|8496
MT2-L1|6|211957760|74587273|1618|0,483|2,829|0,45|11396
ST-L1|8|211957760|74689942|1618|2,135|2,87|0,44|11324
MT-L1|8|211957760|74689942|1618|0,741|2,246|0,20|10732
MT2-L1|8|211957760|74660843|1618|0,444|3,417|0,60|12144
ST-L1|12|211957760|74767085|1618|2,578|2,523|0,48|14940
MT-L1|12|211957760|74767085|1618|0,653|2,422|0,22|15196
MT2-L1|12|211957760|74639131|1614|0,455|3,495|0,73|20348

Level|Threads|InSize|OutSize|Frames|Real|User|Sys|MaxMem
-----|-------|------|-------|------|----|----|---|-------
ZSTD-L5|1|211957760|65003964|1618|5,441|5,381|0,47|4356
ST-L5|1|211957760|65010435|1618|5,122|5,69|0,43|3992
MT-L5|1|211957760|65010435|1618|5,195|4,545|0,4|4208
MT2-L5|1|211957760|65010463|1618|5,42|4,993|0,38|4108
ST-L5|2|211957760|65648111|1618|6,130|6,77|0,37|7024
MT-L5|2|211957760|65648111|1618|2,945|4,911|0,3|7120
MT2-L5|2|211957760|65650264|1618|2,565|5,85|0,33|7396
ST-L5|4|211957760|66124195|1618|6,333|6,261|0,57|12560
MT-L5|4|211957760|66124195|1618|2,548|6,382|0,9|12688
MT2-L5|4|211957760|66099370|1618|1,810|7,157|0,44|13548
ST-L5|6|211957760|66415364|1618|6,990|6,923|0,50|18060
MT-L5|6|211957760|66415364|1618|2,148|8,296|0,9|18340
MT2-L5|6|211957760|66448047|1618|1,587|9,387|0,58|21160
ST-L5|8|211957760|66589422|1618|6,365|6,292|0,57|23660
MT-L5|8|211957760|66589422|1618|1,913|10,8|0,18|23988
MT2-L5|8|211957760|66620937|1618|1,530|11,933|0,62|27232
ST-L5|12|211957760|66870307|1618|6,780|6,696|0,68|34748
MT-L5|12|211957760|66870307|1618|1,841|10,768|0,34|35148
MT2-L5|12|211957760|66792124|1614|1,590|12,423|0,59|41060

Level|Threads|InSize|OutSize|Frames|Real|User|Sys|MaxMem
-----|-------|------|-------|------|----|----|---|-------
ZSTD-L10|1|211957760|60176247|1618|12,965|12,883|0,51|14592
ST-L10|1|211957760|60182718|1618|13,627|13,543|0,51|14344
MT-L10|1|211957760|60182718|1618|13,669|13,155|0,7|14564
MT2-L10|1|211957760|60182727|1618|14,106|14,25|0,45|14276
ST-L10|2|211957760|61052905|1618|15,175|15,102|0,38|27328
MT-L10|2|211957760|61052905|1618|7,808|14,401|0,6|27192
MT2-L10|2|211957760|61099151|1618|7,822|15,550|0,46|27728
ST-L10|4|211957760|61822684|1618|15,695|15,600|0,55|53160
MT-L10|4|211957760|61822684|1618|5,828|18,90|0,8|53184
MT2-L10|4|211957760|61820465|1618|4,754|18,830|0,78|58004
ST-L10|6|211957760|62322565|1618|15,960|15,861|0,56|79260
MT-L10|6|211957760|62322565|1618|4,670|22,58|0,24|79116
MT2-L10|6|211957760|62340755|1618|3,965|23,542|0,68|84132
ST-L10|8|211957760|62672815|1618|15,368|15,263|0,65|104864
MT-L10|8|211957760|62672815|1618|4,22|25,542|0,24|104876
MT2-L10|8|211957760|62695757|1618|3,493|27,318|0,84|110056
ST-L10|12|211957760|63167326|1618|15,374|15,256|0,78|156512
MT-L10|12|211957760|63167326|1618|3,975|25,875|0,32|156372
MT2-L10|12|211957760|63116812|1615|3,533|26,845|0,80|159760

Level|Threads|InSize|OutSize|Frames|Real|User|Sys|MaxMem
-----|-------|------|-------|------|----|----|---|-------
ZSTD-L15|1|211957760|58013718|1618|58,253|58,18|0,65|22648
ST-L15|1|211957760|58020189|1618|57,548|57,344|0,62|22772
MT-L15|1|211957760|58020189|1618|57,551|57,217|0,9|22564
MT2-L15|1|211957760|58020195|1618|57,555|57,335|0,56|22604
ST-L15|2|211957760|59057589|1618|57,406|57,200|0,57|43704
MT-L15|2|211957760|59057589|1618|28,659|55,233|0,9|43808
MT2-L15|2|211957760|59063384|1618|28,828|57,456|0,48|44096
ST-L15|4|211957760|60087162|1618|55,540|55,329|0,71|85816
MT-L15|4|211957760|60087162|1618|18,302|63,285|0,21|86996
MT2-L15|4|211957760|60094094|1618|15,824|62,951|0,71|86780
ST-L15|6|211957760|60748124|1618|53,394|53,194|0,68|128772
MT-L15|6|211957760|60748124|1618|13,254|69,115|0,24|128132
MT2-L15|6|211957760|60750146|1618|11,916|70,951|0,82|129128
ST-L15|8|211957760|61218685|1618|52,236|52,22|0,85|170856
MT-L15|8|211957760|61218685|1618|10,734|75,669|0,32|170208
MT2-L15|8|211957760|61240493|1618|9,876|77,588|0,107|173732
ST-L15|12|211957760|61911651|1618|49,643|49,423|0,102|255612
MT-L15|12|211957760|61911651|1618|10,209|74,498|0,43|254728
MT2-L15|12|211957760|61911572|1616|9,690|76,109|0,95|257144

Level|Threads|InSize|OutSize|Frames|Real|User|Sys|MaxMem
-----|-------|------|-------|------|----|----|---|-------
ZSTD-L20|1|211957760|53010733|1618|147,732|147,269|0,81|330548
ST-L20|1|211957760|53017204|1618|147,808|147,335|0,93|330336
MT-L20|1|211957760|53017204|1618|144,750|144,220|0,36|330560
MT2-L20|1|211957760|53017210|1618|144,966|144,510|0,105|330688
ST-L20|2|211957760|54572594|1618|137,548|137,59|0,136|659404
MT-L20|2|211957760|54572594|1618|68,135|130,987|0,61|659552
MT2-L20|2|211957760|54593414|1618|66,789|133,38|0,110|660032
ST-L20|4|211957760|56001910|1618|126,222|125,706|0,186|1317540
MT-L20|4|211957760|56001910|1618|39,912|137,859|0,115|1317676
MT2-L20|4|211957760|56012864|1618|34,482|136,588|0,188|1320072
ST-L20|6|211957760|56766084|1618|116,52|115,538|0,246|1975792
MT-L20|6|211957760|56766084|1618|29,425|151,530|0,188|1977964
MT2-L20|6|211957760|56722303|1618|26,166|154,415|0,268|1976948
ST-L20|8|211957760|57295693|1618|115,503|114,866|0,337|2588116
MT-L20|8|211957760|57295693|1618|23,734|166,717|0,249|2586996
MT2-L20|8|211957760|57306440|1618|22,0|170,493|0,345|2590624
ST-L20|12|211957760|58020436|1618|108,427|107,702|0,455|3776192
MT-L20|12|211957760|58020436|1618|22,569|163,133|0,395|3775632
MT2-L20|12|211957760|58059839|1618|21,778|166,102|0,538|3781700
