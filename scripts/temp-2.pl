#!/usr/bin/perl -w

use KBS2::ImportExport;

use Data::Dumper;

my $ie = KBS2::ImportExport->new();

my @testinputs = (
		  "completed(task).",
		  "completed('this is a test').",
		  # "'this is a test'.",
		 );
foreach my $testinput (@testinputs) {
  my $res0 = $ie->Convert
    (
     Input => $testinput,
     InputType => 'Prolog',
     OutputType => 'Interlingua',
    );
  if ($res0->{Success}) {
    print Dumper($res0->{Output}[0]);
  }
}
