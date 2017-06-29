#!/usr/bin/perl -w

use BOSS::Config;
use KBS2::ImportExport;
use KBS2::Util;
use PerlLib::SwissArmyKnife;
use UniLang::Util::TempAgent;

use Math::Round;

$specification = q(
	-a <agent>		Formalog Agent to contact
	-i <interval>		Interval between heartbeats
	-t			Test
);

my $config =
  BOSS::Config->new
  (Spec => $specification);
my $conf = $config->CLIConfig;
# $UNIVERSAL::systemdir = "/var/lib/myfrdcsa/codebases/minor/system";

my $interval = $conf->{'-i'} || 60;

my $tempagent = UniLang::Util::TempAgent->new;
my $importexport = KBS2::ImportExport->new();

sub Main {
  while (1) {
    ProcessHeartbeat(Input => 'mainLoopHeartbeat(Results).');
    sleep $interval;
  }
}

sub ProcessHeartbeat {
  my (%args) = @_;
  my $input = $args{Input};
  my $res1 = $importexport->Convert
    (
     Input => $input,
     InputType => 'Prolog',
     OutputType => 'Interlingua',
    );
  if ($res1->{Success}) {
    my $interlingua = $res1->{Output}[0];
    my $variables = ListVariablesInFormula(Formula => $interlingua);
    unshift @$variables, '_prolog_list';
    my $query = [['_prolog_list',$variables,$interlingua]];

    my $res2 = $tempagent->MyAgent->QueryAgent
      (
       Receiver => $conf->{'-a'} || "Agent1",
       Data => {
		_DoNotLog => 1,
		Eval => $query,
	       },
      );
    print Dumper({FormalogQueryResult => $res2});
    # print Dumper({Res2 => $res2});
    shift @$variables;
    my $assertions = $res2->{Data}{Result};
    foreach my $assertion (@$assertions) {
      # print Dumper({Assertion => $assertion});
      my $ref = ref($assertion);
      if ($ref eq 'ARRAY') {
	shift @$assertion;
	my $i = 0;
	while (defined $variables->[$i] and
	       defined $assertion->[$i]) {
	  my $res3 = $importexport->Convert
	    (
	     Input => [$assertion->[$i]],
	     InputType => 'Interlingua',
	     OutputType => 'Prolog',
	    );
	  my @res;
	  my $var = TermIsVariable($variables->[$i]);
	  if ($res3->{Success}) {
	    my $output = $res3->{Output};
	    chomp $output;
	    push @res, [$var, $output];
	  } else {
	    push @res, [$var, "---ERROR---"];
	  }
	  my $it = $res[0];
	  print $it->[0]." = ".$it->[1]."\n";
	  ++$i;
	}
      }
      print "\n";
    }
  }
}

Main();

