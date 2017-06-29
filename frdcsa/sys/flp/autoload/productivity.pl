accomplished(andrewDougherty,'integrated CSO KB with FLP',[2016-04-29,06:15:12]).
accomplished(andrewDougherty,'accomplishment feature of FLP',[2016-04-29,06:16:43]).
accomplished(andrewDougherty,'added search to elog',[2016-04-29,06:46:39]).
accomplished(andrewDougherty,'got the basic KBFS2 thing work (in packager-agent)',[2016-05-11,14:00:56]).
accomplished(andrewDougherty,madeProgressOn(kbfs2,[
						   'got the swipl archive.pl working and processing archive files, which I extract to a temp dir and then index all their md5,sha1,crc32,basename and size into KBS2',
						   'when I complete the KBS2::Reasoner::Prolog, assertions will be several orders of magnitude faster (LP instead of FOLE)',
						   'the motivation for working on KBFS2 right now (besides all the others), is to iterate over archive files in the FRDCSA and extract email addresses from them, in order to relate code to email addresses, and then those systems that we indicate a preference to package, the agent will send an email out automatically to the people involved asking whether they are interested and can assist with the packaging process.',
						   'this would be trivial with Perl, but here it is all in Prolog'
						  ]),
	     [2016-05-11,18:54:09]).
