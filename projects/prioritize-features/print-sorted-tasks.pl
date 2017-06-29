#!/usr/bin/perl -w

use BOSS::Config;
use Do::ListProcessor2;
use KBS2::ImportExport;
use Manager::Dialog qw(Choose);
use PerlLib::SwissArmyKnife;

my $c = read_file('results.pl');
my $data = DeDumper($c);

my $config =
  BOSS::Config->new
  (Spec => $specification);
my $conf = $config->CLIConfig;
# $UNIVERSAL::systemdir = "/var/lib/myfrdcsa/codebases/minor/system";

my $listprocessor = Do::ListProcessor2->new();
my $res1 = $listprocessor->MyLight->PrettyGenerate
  (
   Structure => [reverse @$data],
   Indent  => 0,
  );

print $res1."\n";
