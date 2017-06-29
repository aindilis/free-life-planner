#!/usr/bin/perl -w

# derived from /var/lib/myfrdcsas/versions/myfrdcsa-1.0/codebases/releases/do-0.1/do-0.1/scripts/prioritize-actual-goals.pl

use Do::ListProcessor2;
use Manager::Dialog qw(Choose);
use PerlLib::SwissArmyKnife;

my $listprocessor = Do::ListProcessor2->new();
my $c = read_file('/var/lib/myfrdcsa/codebases/minor/free-life-planner/prioritize-features/do.notes');
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
