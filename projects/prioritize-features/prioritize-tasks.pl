#!/usr/bin/perl -w

# derived from /var/lib/myfrdcsas/versions/myfrdcsa-1.0/codebases/releases/do-0.1/do-0.1/scripts/prioritize-actual-goals.pl

use BOSS::Config;
use Do::ListProcessor2;
use Manager::Dialog qw(Choose);
use PerlLib::SwissArmyKnife;

$specification = q(
	-c <contents>		Task Contents
	-f <file>		Task Contents File
);

my $config =
  BOSS::Config->new
  (Spec => $specification);
my $conf = $config->CLIConfig;
# $UNIVERSAL::systemdir = "/var/lib/myfrdcsa/codebases/minor/system";

if ($conf->{'-c'} and $conf->{'-f'}) {
  die "Please use only of -c or -f arguments\n";
}
if (! ($conf->{'-c'} or $conf->{'-f'})) {
  die "Need to specify tasks with either -c or -f arguments\n";
}

my $c;
if ($conf->{'-f'}) {
  my $file = $conf->{'-f'};
  if (! -f $file) {
    die "File does not exist: <$file>\n";
  } else {
    $c = read_file($file);
  }
}
if ($conf->{'-c'}) {
  my $contents = $conf->{'-c'};
  if (! length($contents)) {
    die "Must have non-zero length contents\n";
  } else {
    $c = $contents;
  }
}

my $listprocessor = Do::ListProcessor2->new();
my $domain = $listprocessor->MyLight->Parse
  (
   Contents => $c,
  );
my $res1 = $listprocessor->ProcessDomainNew
  (Domain => $domain);
# print Dumper($res1);
my $items = $res1->{ReturnDomain}[0];
shift @$items;
# print Dumper($items);

print Dumper([sort {Compare(ClearDumper($a),ClearDumper($b))} @$items]);

my $history = {};

sub Compare {
  my ($a,$b) = @_;
  my $dontcare = "Don't care";
  print "Which one is most important...\n";
  my $result = Choose($a,$b,$dontcare);
  if (! exists $history->{$a}->{$b}) {
    my $retval;
    if ($result eq $a) {
      $retval = 1;
    } elsif ($result eq $b) {
      $retval = -1;
    } elsif ($result eq $dontcare) {
      $retval = 0;
    }
    $history->{$a}->{$b} = $retval;
  }
  return $history->{$a}->{$b};
}
