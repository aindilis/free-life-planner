#!/usr/bin/perl -w

use BOSS::Config;
use PerlLib::SwissArmyKnife;

$specification = q(
	-f <files>...	Files to analyze
);

my $config =
  BOSS::Config->new
  (Spec => $specification);
my $conf = $config->CLIConfig;

my $predicates = {};
foreach my $file (@{$conf->{'-f'}}) {
  my $c = read_file($file);
  foreach my $line (split /\n/, $c) {
    if ($line =~ /^\s*([a-z][a-zA-Z0-9_]+)[^a-zA-Z0-9_]?.*?:-/) {
      $predicates->{$1}++;
    } elsif ($line =~ /^([a-z][a-zA-Z0-9_]+)\(.+?\)\.\s*$/) {
      $predicates->{$1}++;
    }
  }
}
print Dumper($predicates);
