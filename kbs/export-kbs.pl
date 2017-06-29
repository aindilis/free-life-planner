#!/usr/bin/perl -w

use PerlLib::SwissArmyKnife;

my $datetime = DateTimeStamp();
my $rootdir = '/var/lib/myfrdcsa/codebases/minor/free-life-planner/kbs/backup';
my $exportdir = ConcatDir($rootdir,$datetime);
MkDirIfNotExists(Directory => $exportdir);
foreach my $context ('Org::Cyc::BaseKB','Org::Cyc::WSMContext::real','Org::Cyc::WSMContext::fictional') {
  my $contextfile = ConcatDir($exportdir,$context.'.kbs');
  my $command = "kbs2 -c $context show > $contextfile";
  print $command."\n";
}
