#!/usr/bin/env perl


use Getopt::Long;

my $vendor  = 0;

GetOptions(
    "vendor"  => \$vendor,
) or die "Invalid options\n";

my $srcPath = "src";
my $includePath = "include";
my $vendorPath = "vendor";
my $buildPath = "build";

my @libs = ("GL","GLU","glfw");
my $formatedLibs = "";
foreach $lib (@libs)
{
  $formatedLibs = $formatedLibs .  " -l" . $lib;
}
my $k = " ";
if($vendor){
    $k = $vendorPath;
}

my $fileCPP = `find $srcPath $k -name '*.cpp' -print`;
my $fileC = `find $srcPath $k -name '*.c' -print`;
my @files = (split (/\n/,$fileCPP), split( /\n/,$fileC));
# my @files = (split("/\n/",$fileCPP),split("/\n/",$fileC));
# my $rn ="g++ -Wall -I" . $includePath . " -I" . $vendorPath . " -g -ggdb " . $formatedLibs . " ";
my $rn ="g++ -Wall -I" . $includePath . " -I" . $vendorPath . " -g -ggdb "  . $formatedLibs ." ";
foreach $file (@files){
    if($file =~ /\w*\/(\w+)\.\w+/g){
        print "\n";
        my $putamerdainfernocaralho = $rn  .  $file." -c -o "  .$buildPath ."/". $1. ".o";
        my $k = system($putamerdainfernocaralho);
        if($k != 0){
            print STDERR "Error1\n";
            exit 1;
        }
    }
    else{
        print STDERR "Error2\n";
        exit 1;
    }
}

my $fileO = `find $buildPath -name '*.o' -print`;
$fileO =~ s/\n/ /g;
my $pinto = system($rn . $fileO ."-o ".$buildPath ."/final");
if($pinto != 0){
    print STDERR "Error3\n";
        exit 1;
}

exit 0;
