#!/usr/bin/perl -w

use PerlLib::SwissArmyKnife;

my $sites =<<OUT;
http://dev.freelifeplanner.org/frdcsa/codebases/minor/interactor
http://dev.freelifeplanner.org/frdcsa/codebases/minor/dialog_interface
http://dev.freelifeplanner.org/frdcsa/planning_systems/notes
http://dev.freelifeplanner.org/frdcsa/codebases/minor/free_life_planner
http://dev.freelifeplanner.org/frdcsa/planning_systems/dates
http://dev.freelifeplanner.org/ontology/andrewDougherty
OUT

foreach my $url (split /\n/, $sites) {
  print "<$url>\n";
  my $c = "firefox -new-tab -url ".shell_quote($url);
  print "$c\n";
  system $c;
}
