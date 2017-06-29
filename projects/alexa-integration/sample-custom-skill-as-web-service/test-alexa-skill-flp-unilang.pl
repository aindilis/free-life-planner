#!/usr/bin/perl -w

use KBS2::Util;
use PerlLib::SwissArmyKnife;
use UniLang::Util::TempAgent;

my $tempagent = UniLang::Util::TempAgent->new(Name => 'Alexa-Skill-FLP');

my $res1 = $tempagent->MyAgent->QueryAgent
  (
   Receiver => "Agent1",
   Data => {
	    _DoNotLog => 1,
	    Eval => [['_prolog_list',\*{'::?Response'},['alexaSkillFLPQuery','When is my next appointment with <HEALTHWORKER>?',\*{'::?Response'}]]],
	   },
  );

print Dumper($res1);
