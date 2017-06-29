#!/usr/bin/perl -w

use KBS2::ImportExport;
use PerlLib::SwissArmyKnife;

use Language::Prolog::Yaswi;
use Language::Prolog::Types::Internal;

# my $swipl = bless( [ 'whatIf', bless( [ 'not', bless( [ 'receive', '<PERSON>', '<FORMOFASSISTANCE>' ], 'Language::Prolog::Types::Internal::functor' ) ], 'Language::Prolog::Types::Internal::functor' ) ], 'Language::Prolog::Types::Internal::functor' );

my $swipli = [ 'whatIf', [ 'not', [ 'receive', '<PERSON>', '<FORMOFASSISTANCE>' ]]];

print Dumper($swipl);

my $ie = KBS2::ImportExport->new();
my $res = $ie->Convert
  (
   Input => [$swipli],
   InputType => 'Interlingua',
   OutputType => 'Prolog',
  );

print Dumper($res);
