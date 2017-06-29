#!/usr/bin/perl -w

use BOSS::Config;
use PerlLib::SwissArmyKnife;

$specification = q(
	-t <text>		Text
	-f <filename>		Filename containing text 
);

my $dir = '/var/lib/myfrdcsa/codebases/minor/alexa-integration/data-git/push-notifications/cache/';

my $config =
  BOSS::Config->new
  (Spec => $specification);
my $conf = $config->CLIConfig;
# $UNIVERSAL::systemdir = "/var/lib/myfrdcsa/codebases/minor/system";

my $text;
if (exists $conf->{'-t'}) {
  $text = $conf->{'-t'};
} elsif (exists $conf->{'-f'}) {
  my $filename = $conf->{'-f'};
  if (-f $filename) {
     $text = read_file($filename);
   }
}

$text =~ s/[^a-zA-Z0-9-_]/_/sg;
print ConcatDir($dir,$text.'.mp3');
