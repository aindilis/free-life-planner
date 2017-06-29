#!/usr/bin/perl -w

use BOSS::Config;
use PerlLib::SwissArmyKnife;

$specification = q(
	-u <UserName>		UserName
	-a <IP>			IP Address
	-f [<Files>...]		Command to run
	-o <Output>		Sound output to use (e.g. bluetooth or "computer speakers")
);

my $config =
  BOSS::Config->new
  (Spec => $specification);
my $conf = $config->CLIConfig;
# $UNIVERSAL::systemdir = "/var/lib/myfrdcsa/codebases/minor/system";

my $username = $conf->{'-u'};
my $ip = $conf->{'-a'};
my $command = $conf->{'-c'};

my $pass;
if ($username eq '<USERNAME1>' and $ip eq '<IP1>') {
  $pass = '<PASSWORD1>';
} elsif ($username eq '<USERNAME2>' and $ip eq '<IP2>') {
  $pass = '<PASSWORD2>';
}

my $qusername = shell_quote($username);
my $qip = shell_quote($ip);
my $qcommand = shell_quote($command);
my $qpass = shell_quote($pass);

# transfer the file over.  probably best to put it in a location on
# game for now, since that's sort of a shared location.  Could be a
# security risk.

my $qfilenames = join(' ',map {shell_quote($_)} @{$conf->{'-f'}});

my $scpcommand = 'sshpass -p '.$qpass.' scp -r '.$qfilenames.' '.$username.'@'.$ip.':cache/';
print "<$scpcommand>\n";
system $scpcommand;

my @items = map {ConcatDir('/home/<USERNAME>/cache/'.basename($_))} @{$conf->{'-f'}};

my $qupdatedfilenames = join(' ',map {shell_quote($_)} @items);
print Dumper
  ({
    Items => \@items,
    QUpdatedFileNames => $qupdatedfilenames,
   });

my $output = $conf->{'-o'};
my $playcommand = "/var/lib/myfrdcsa/codebases/minor/prolog-agent/scripts/remote-execution.pl -s $qip -u $qusername -c \"/home/aindilis/bin/play-sound-files.sh $output ".$qupdatedfilenames."\"";
print "<$playcommand>\n";
system $playcommand;
