#!/usr/bin/perl -w

use BOSS::Config;
use PerlLib::SwissArmyKnife;
# use Rival::Lingua::EN::Sentence ::NoPunctuation;

$specification = q(
	-f <file>	File to process containing transcript

	-l		Fix lines
	-r		Remove times
	-j		Fill paragraphs
);

my $config =
  BOSS::Config->new
  (Spec => $specification);
my $conf = $config->CLIConfig;
# $UNIVERSAL::systemdir = "/var/lib/myfrdcsa/codebases/minor/system";

my $c = read_file($conf->{'-f'});

if ($conf->{'-l'}) {
  my @lines = $c =~ /(.*?)\b(\d+:\d\d)\s(.*?)/sg;
  # print Dumper(@lines);
  if ($lines[0] eq '') {
    shift @lines;
  }
  my @results;
  while (@lines) {
    my ($time,$tmp,$line) = (shift @lines, shift @lines, shift @lines);
    $line =~ s/[\n\r]/ /sg;
    $line =~ s/^\s*//sg;
    $line =~ s/\s*$//sg;
    my $entry = '';
    unless ($conf->{'-r'}) {
      $entry .= "$time\t";
    }
    $entry .= "$line";
    push @results, $entry;
  }
  my $output;
  if ($conf->{'-j'}) {
    # FIXME:
    $output = join(' ',@results);
  } else {
    $output = join("\n",@results);
  }
  print $output;
}
