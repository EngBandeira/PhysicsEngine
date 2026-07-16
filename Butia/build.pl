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
my $includePath = "src";
my $buildPath = "build";
my $vendorPath = "vendor";

my @libs = ("GL","GLU","glfw","X11","ssl","crypto");
my $formatedLibs = "";
foreach $lib (@libs)
{
  $formatedLibs = $formatedLibs .  " -l" . $lib;
}

system("rm -f -r build/src/*");
system("rm -f -r libbutia.a");

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
            return unless /\.cpp$/;
            push @files, $File::Find::name;
        },
        $vendorPath
    );
    find(
        sub {
            return unless /\.c$/;
            push @files, $File::Find::name;
        },
        $vendorPath
    );
}

my $rn =" -std=c++17 -Wall -Wno-unused -Ivendor -I" . $includePath . " -I." . " -g -ggdb " . $formatedLibs;


if($asan)
{
 $rn = "-fsanitize=address " . $rn
}

foreach $file (@files){
    if( $file =~ /(\S*)\/(\w+).\w+/g ) {
        system("mkdir -p build/$1");
        my $command = $rn . $formatedLibs . " "  . $file ." -c -o " . $buildPath . "/$1/$2" . ".o";
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





my $fileO = "";

find(
    sub {
        return unless /\.o$/;
        $fileO .= " " . $File::Find::name;
    },
    $vendorPath
);

find(
    sub {
        return unless /\.o$/;
        $fileO .= " " . $File::Find::name;
    },
    $buildPath
);

$fileO =~ s/\n/ /g;
my $caraia = "rcs libbutia.a $fileO";
my $pinto = system "ar", (split ' ', $caraia);

#print Dumper([split ' ', $caraia]);


if($pinto != 0){
    print STDERR "Error3\n";
        exit 1;
}

exit 0;
