#!/usr/bin/env perl

use Getopt::Long;
use Data::Dumper;
use File::Find;
use Cwd;

my $vendor  = 0;
my $asan = 0;
GetOptions(
    "vendor"  => \$vendor,
    "asan" => \$asan,
) or die "Invalid options\n";

my $srcPath = "src";
my $includePath = "include";
my $vendorPath = "vendor";
my $buildPath = "build";

my @libs = ("GL","GLU","glfw","X11","ssl","crypto");
my $formatedLibs = "";
foreach $lib (@libs)
{
  $formatedLibs = $formatedLibs .  " -l" . $lib;
}

my @files;
find(
    sub {
        return unless /\.cpp$/;
        push @files, $File::Find::name;
    },
    $srcPath
);
find(
    sub {
        return unless /\.c$/;
        push @files, $File::Find::name;
    },
    $srcPath
);

if($vendor){
    find(
        sub {
            return unless /\.c$/;
            push @files, $File::Find::name;
        },
        $vendorPath
    );
    find(
        sub {
            return unless /\.cpp$/;
            push @files, $File::Find::name;
        },
        $vendorPath
    );
}

#Dump(\@files);

# my @files = (split("/\n/",$fileCPP),split("/\n/",$fileC));
# my $rn ="g++ -Wall -I" . $includePath . " -I" . $vendorPath . " -g -ggdb " . $formatedLibs . " ";

my $rn =" -std=c++17 -Wall -Wno-unused -I" . $includePath . " -I." . " -g -ggdb ";
if($asan)
{
 $rn =" -Wall -Wno-unused -fsanitize=address -I" . $includePath . " -I." . " -g -ggdb ";
}

foreach $file (@files){
    if( $file =~ /(\w*)\/(\w+)\.\w+/g ) {
        my $command = $rn . $formatedLibs . " "  . $file ." -c -o " . $buildPath . "/$1_$2" . ".o";
        print "\n";
        my $k = system "g++", (split ' ', $command) ;
        if($k != 0){
            print $command . "\n";
            print STDERR "Error1\n";
            exit 1;
        }
    }
    else{
        print STDERR "Error2\n";
        exit 1;
    }
}

my $fileO = ``;
find(
    sub {
        return unless /\.o$/;
        $fileO .= " " . $File::Find::name;
    },
    $buildPath
);

$fileO =~ s/\n/ /g;
my $caraia = $rn . $formatedLibs . " " . $fileO ." -o ".$buildPath ."/final";
my $pinto = system "g++", (split ' ', $caraia);

if($pinto != 0){
    print STDERR "Error3\n";
        exit 1;
}

exit 0;
