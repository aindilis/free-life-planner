#!/usr/bin/perl -w

use PerlLib::SwissArmyKnife;

foreach my $file (split /\n/, `cat files.txt`) {
  print "<$file>\n";
  my $dir = dirname($file);
  my $qd = shell_quote(ConcatDir("/var/lib/myfrdcsa/codebases/minor/free-life-planner/release/input",$dir));
  system 'mkdir -p '.$qd;
  system 'cp '.shell_quote($file).' '.$qd;
}
