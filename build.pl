#!/usr/bin/env perl

use Getopt::Long;

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
my $dPath = "build";

my @libs = ("GL","GLU","glfw","X11");
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
my @include_files = `find $includePath $k -name '*.hpp' -print`;
my @d_files = `find $dPath $k -name '*.d' -print`;
# my @files = (split("/\n/",$fileCPP),split("/\n/",$fileC));
# my $rn ="g++ -Wall -I" . $includePath . " -I" . $vendorPath . " -g -ggdb " . $formatedLibs . " ";

my $rn ="g++ -MMD -std=c++17 -Wall -I" . $includePath . " -I." . " -g -ggdb "  . $formatedLibs ." ";
if( $asan ) {
    $rn ="g++ -Wall -fsanitize=address -I" . $includePath . " -I." . " -g -ggdb "  . $formatedLibs ." ";
}


sub is_cached {
    my $file = shift @_;
    my $last_change = `stat -c %Y $file`;
    my $cached = 0;
    if(-e '.cached_comp') {
        open(my $cache_fh, '<', '.cached_comp') or die "Could not open file '.cached_comp' $!";
        while(my $line = <$cache_fh>) {
            if ($line eq "$file:$last_change\n") {
                $cached = 1;
                last;
            }
        }
        close($cache_fh);
    }
    return $cached;
}

sub cache {
    my @files = shift @_;
    open(my $cache_fh, '>', '.cached_comp') or die "Could not open file '.cached_comp' $!";
    foreach my $file (@files) {
        my $last_change = system("stat -c %Y $file");
        my $to_cache = "$file:$last_change\n";
        print $cache_fh $to_cache;
    }
    close($cache_fh);
}

foreach my $dep_file (@d_files) {
    (my $source_file = $dep_file) =~ s/\.d$//;

    $dep_file = substr $dep_file, 0, -1;
    open my $fh, '<:encoding(UTF-8)', $dep_file
        or die "Não abriu $dep_file: $!";

    my $content = do { local $/; <$fh> };
    close $fh;

    my @headers;
    while ($content =~ /([^\s\\:]+?\.hpp)/g) {
        push @headers, $1;
    }

    foreach my $header (@headers) {
        next if is_cached($header);

        my $i = 0;
        foreach my $file (@files) {
            next if $file ne $source_file;
            next if is_cached($file);
            $i++;
        }
        if($i == @files) {
            push @files, $file;
        }

        print "Header alterado ou novo: $header\n";
        print "   → deve recompilar: $source_file\n";
    }
}

foreach my $file (@files) {
    if($file =~ /(\w*)\/(\w+)\.\w+/g and (! is_cached $file)) {

        my $command = $rn . $file ." -c -o " . $buildPath . "/$1_$2" . ".o";
        print $command;
        print "\n";

        if( system($command) != 0 ){
            print STDERR "Error1\n";
            exit 1;
        }

        cache $file;
    }
    else {
        print STDERR "Error2\n";
        exit 1;
    }

    my $check_success = `find $buildPath -name '*.o' -print`;
    $check_success =~ s/\n/ /g;
    if( system($rn . $fileO ."-o ".$buildPath ."/final") != 0 ){
        print STDERR "Error3\n";
        exit 1;
    }
}

exit 0;
