#!/usr/bin/perl -w

use BOSS::Config;
use KBS2::Util;
use PerlLib::SwissArmyKnife;
use UniLang::Util::TempAgent;

use Error qw(:try);

$specification = q(
	-c <command>		Run the query for command
);


my $config =
  BOSS::Config->new
  (Spec => $specification);
my $conf = $config->CLIConfig;
# $UNIVERSAL::systemdir = "/var/lib/myfrdcsa/codebases/minor/guardian";

my $notconnected = 0;
my $tempagent;

try {
  $tempagent = UniLang::Util::TempAgent->new
    (
     Name => 'FLP-Cron',
    );
}
  catch Error with {
    print "Could not connect to FLP through UniLang Agent\n";
    $notconnected = 1;
  };

# Send a message to the guard agent
unless ($notconnected) {
  my $res1 = $tempagent->MyAgent->QueryAgent
    (
     Receiver => 'Agent1',
     Data => {
	      Eval => [
		       ['_prolog_list',
			Var('?Message'),
			['grabUsersAttentionByLockingScreenWithMessage',Var('?Message')]
		       ]
		      ],
	     },
    );
  if (exists $res1->{Data}) {
    if (exists $res1->{Data}{Result}) {
      my @list = @{$res1->{Data}{Result}};
      shift @list;
      print "Message: $list[0]\n";
    } else {
      # throw Error();
    }
  } else {
    # throw Error();
  }
}
