#!/usr/bin/perl -w

use PerlLib::SwissArmyKnife;

# completed('\'')

# also fix:

# '\').

# '' r').

my $c1 = read_file('files-flp.txt');
foreach my $file (split /\n/, $c1) {
  if ($file !~ /\.txt\.nlu.pl$/) {
    die "What: $file\n";
  }
  print "<$file>\n";
  my $c2 = read_file($file);
  if ($c2 =~ /'\\''/s) {
    print "Match!\n";
  }
  my $c3 = $c2;
  $c2 =~ s/'\\''/''/sg;
  if ($c2 ne $c3) {
    my $command = 'cp '.shell_quote($file).' '.shell_quote($file.'.bac');
    print $command."\n";
    my $res1 = `$command`;
    print $res1."\n";
    if (1) {
      my $fh = IO::File->new();
      if ($fh->open('>'.$file)) {
	print $fh $c2;
	$fh->close();
      } else {
	die "What2: $file\n";
      }
    }
  } else {
    print "Duh!\n";
  }
}
