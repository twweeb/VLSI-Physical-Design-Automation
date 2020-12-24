#!/usr/bin/perl

# Written by Gi-Joon Nam (gnam@us.ibm.com) on Dec 2005
# usage: check_density_target.pl <input.nodes> <solution.pl> <input.scl> <targetdensity>

use POSIX qw(floor ceil);

sub my_exit {
  print "Abnormal condition happened. Exit\n";
  exit(1);
}

$DEFAULT_ROW_HEIGHT = 504;  # DO NOT CHANGE. All circuit row height is 12
$DENSITY_TARGET = 1.0;     # default density target
$BIN_ROW_FACTOR = 10;      # default bin size = 10 circuit row height x 10 circuit row height

%ObjectDB;
@RowDB;        # stores row_y_low value for each row.
@RowDBstartX;  # starting X coordinate of each row
@RowDBendX;    # ending X coordinate of each row. RowDBs share the same index

$node_file = $ARGV[0];
$sol_file  = $ARGV[1];
$scl_file  = $ARGV[2];
$commandline_td = $ARGV[3];
$phase = 0;

if (!defined($node_file) || !defined($sol_file) || !defined($scl_file)) {
    #print "Usage: check_density_target.pl <input.nodes> <Solution PL file> <SCL file> <density_target>\n";
    print "Usage: check_density_target.pl <input.nodes> <Solution PL file> <SCL file>\n";
    exit(0);
}

open (NODEFILE, $node_file) || die "I can't open node file $node_file\n";
open (SOLFILE, $sol_file)  || die "I can't open sol  file $sol_file\n";
open (SCLFILE, $scl_file)  || die "I can't open scl  file $scl_file\n";

if (defined($commandline_td) && ($commandline_td > 0) && ($commandline_td <= 1.0)) {
    $target_density = $commandline_td;
}
else {
    $target_density = $DENSITY_TARGET;
}

###########################
# scl file processing
###########################
$WINDOW_LX = 999999;
$WINDOW_LY = 999999;
$WINDOW_HX = -99999;
$WINDOW_HY = -99999;

$num_processed_rows = 0;
$row_height = $DEFAULT_ROW_HEIGHT; 
$total_row_area = 0;
while (defined($line = <SCLFILE>)) {
    $line =~ s/^\s+//; # removes front/end white spaces
    $line =~ s/\s+$//;
    @words = split(/\s+/, $line);

    if ($words[0] eq "#" || $words[0] eq "UCLA") {
	next;
    }
    if ($words[0] eq "NumRows" || $words[0] eq "Numrows") {
	$num_rows = $words[2];
	print "NumRows: $num_rows are defined\n";
	next;
    }
    if ($words[0] eq "CoreRow") {
	$line = <SCLFILE>;
	$line =~ s/^\s+//; # removes front/end white spaces
	$line =~ s/\s+$//;
	@words = split(/\s+/, $line);
	
	# Keyword: Coordinate 
	if ($words[0] eq "Coordinate") {
	    $row_y = $words[2];
	}
	else {
	    print       "ERROR: CoreRow Processing: Coordinate keyword not found\n";
	    my_exit();
	}
	push(@RowDB, $row_y);
	
	# Keyword: Height
	$line = <SCLFILE>;
	$line =~ s/^\s+//; # removes front/end white spaces
	$line =~ s/\s+$//;
	@words = split(/\s+/, $line);
	
	$prev_row_height=$row_height;
	if ($words[0] eq "Height") {
	    $row_height = $words[2];
	}
	else {
	    print       "ERROR: CoreRow Processing: Height keyword not found\n";
	    my_exit();
	}

	if ($prev_row_height != $row_height) {
	    print       "ERROR: Row Height mismatch: $prev_row_height vs $row_height\n";
	    my_exit();
	}
	
	# Keyword: Sitewidth
	$line = <SCLFILE>;
	$line =~ s/^\s+//; # removes front/end white spaces
	$line =~ s/\s+$//;
	@words = split(/\s+/, $line);
	
	if ($words[0] eq "Sitewidth") {
	    $site_width = $words[2];
	}
	else {
	    print       "ERROR: CoreRow Processing: Sitewidth keyword not found\n";
	    my_exit();
	}
	
	# Keyword: Sitespacing
	$line = <SCLFILE>;
	$line =~ s/^\s+//; # removes front/end white spaces
	$line =~ s/\s+$//;
	@words = split(/\s+/, $line);
	
	if ($words[0] eq "Sitespacing") {
	    $site_width = $words[2];
	}
	else {
	    print       "ERROR: CoreRow Processing: Sitespacing keyword not found\n";
	    my_exit();
	}
	
	# Keyword: Siteorient
	$line = <SCLFILE>;
	$line =~ s/^\s+//; # removes front/end white spaces
	$line =~ s/\s+$//;
	@words = split(/\s+/, $line);
	
	if ($words[0] eq "Siteorient") {
	    $site_orient = $words[2];
	}
	else {
	    print       "ERROR: CoreRow Processing: Siteorient keyword not found. $words[0] $words[1]\n";
	    my_exit();
	}
	
	# Keyword: Sitesymmetry
	$line = <SCLFILE>;
	$line =~ s/^\s+//; # removes front/end white spaces
	$line =~ s/\s+$//;
	@words = split(/\s+/, $line);
	
	if ($words[0] eq "Sitesymmetry") {
	    $site_symmetry = $words[2];
	}
	else {
	    print       "ERROR: CoreRow Processing: Sitesymmetry keyword not found. $words[0] $words[1]\n";
	    my_exit();
	}
	
	# Keyword: SubrowOrigin
	$line = <SCLFILE>;
	$line =~ s/^\s+//; # removes front/end white spaces
	$line =~ s/\s+$//;
	@words = split(/\s+/, $line);
	
	if ($words[0] eq "SubrowOrigin") {
	    $row_x = $words[2];
	    $row_num_sites = $words[5];
	    push(@RowDBstartX, $row_x);
	    push(@RowDBendX, $row_x + $row_num_sites*$site_width);
	    $total_row_area += ($row_num_sites * $row_height*$site_width);
	}
	else {
	    print       "ERROR: CoreRow Processing: SubrowOrigin keyword not found\n";
	    my_exit();
	}
	
	# Keyword; End
	$line = <SCLFILE>;
	$line =~ s/^\s+//; # removes front/end white spaces
	$line =~ s/\s+$//;
	@words = split(/\s+/, $line);
	
	if ($words[0] ne "End") {
	    print       "ERROR: Keyword End is expected. $words[0] shows up\n";
	    my_exit();
	}
	
	if ($WINDOW_LX > $row_x) {
	    $WINDOW_LX = $row_x;
	}
	if ($WINDOW_HX < $row_x + $row_num_sites*$site_width) {
	    $WINDOW_HX = $row_x + $row_num_sites*$site_width;
	}
	if ($WINDOW_LY > $row_y) {
	    $WINDOW_LY = $row_y;
	}
	if ($WINDOW_HY < $row_y + $row_height) {
	    $WINDOW_HY = $row_y + $row_height;
	}
	
	$num_processed_rows++; 
    }
}
close(SCLFILE);
print "Phase $phase: Total $num_processed_rows rows are processed.\n";
print "         ImageWindow=($WINDOW_LX $WINDOW_LY $WINDOW_HX $WINDOW_HY) w/ row_height=$row_height\n";
print "         Total Row Area=$total_row_area\n"; 
$phase++;

###############################################
# MAP Initialization
###############################################
$WINDOW_WIDTH  = $WINDOW_HX - $WINDOW_LX;
$WINDOW_HEIGHT = $WINDOW_HY - $WINDOW_LY;

$XUNIT = $DEFAULT_ROW_HEIGHT * $BIN_ROW_FACTOR; # 10 circuit row height
$YUNIT = $DEFAULT_ROW_HEIGHT * $BIN_ROW_FACTOR;
$XDIM  = ceil($WINDOW_WIDTH  / $XUNIT); 
$YDIM  = ceil($WINDOW_HEIGHT / $YUNIT);

$CMAP = {
    XDIM  => $XDIM,
    YDIM  => $YDIM,
    XUNIT => $XUNIT,
    YUNIT => $YUNIT,
    BINS  => [],
};

$debug_summed_bin_cap = 0;
$starty = $WINDOW_LY;
for ($j = 0; $j < $CMAP->{YDIM}; $j++) {
    $startx = $WINDOW_LX;
    for ($i = 0; $i < $CMAP->{XDIM}; $i++) {
	$index = $j*$CMAP->{XDIM} + $i;

	$bin_lx = $startx + $i * $XUNIT;
	$bin_ly = $starty;
	$bin_hx = $bin_lx + $XUNIT;
	$bin_hy = $bin_ly + $YUNIT;
	
	if (($i == $XDIM - 1) && ($bin_hx > $WINDOW_HX)) {
	    # last column bins
	    $bin_hx = $WINDOW_HX;
	}
	if (($j == $YDIM - 1) && ($bin_hy > $WINDOW_HY)) {
	    # last row bins
	    $bin_hy = $WINDOW_HY;
	}

        #calculate bin cap based on row definition
	$bin_cap = 0;
	for ($row_index = 0; $row_index < $num_rows; $row_index++) {
	    $row_lx = $RowDBstartX[$row_index];
	    $row_hx = $RowDBendX[$row_index];
	    $row_ly = $RowDB[$row_index];
	    $row_hy = $row_ly + $row_height;

	    if ($row_ly > $bin_hy) {
		last;
	    }
	    if ($row_hy < $bin_ly || $row_ly > $bin_hy || $row_hx < $bin_lx || $row_lx > $bin_hx) {
		# no overlapping area between this row and bin under consideration
		next;
	    }

	    # get intersection between row & bin
	    if ($row_lx >= $bin_lx) {
		$common_lx = $row_lx;
	    }
	    else {
		$common_lx = $bin_lx;
	    }
	    if ($row_hx >= $bin_hx) {
		$common_hx = $bin_hx;
	    }
	    else {
		$common_hx = $row_hx;
	    }
	    if ($row_ly >= $bin_ly) {
		$common_ly = $row_ly;
	    }
	    else {
		$common_ly = $bin_ly;
	    }
	    if ($row_hy >= $bin_hy) {
		$common_hy = $bin_hy;
	    }
	    else {
		$common_hy = $row_hy;
	    }
	    $common_area = ($common_hx - $common_lx)*($common_hy - $common_ly);
	    $bin_cap += $common_area;
	}
	$cover_area = ($bin_hx - $bin_lx) * ($bin_hy - $bin_ly);

	$bin = {
	    LX => $bin_lx,
	    HX => $bin_hx,
	    LY => $bin_ly,
	    HY => $bin_hy,
	    CAP => $bin_cap,
	    MUSAGE => 0,
	    FUSAGE => 0,
	};
	@{$CMAP->{BINS}}[$index] = $bin;
	$debug_summed_bin_cap += $bin->{CAP};
    }
    $starty += $CMAP->{YUNIT};
}

# Debugging purpose: Just make sure sigma(bin area) == placement area
$debug_place_area = $WINDOW_WIDTH * $WINDOW_HEIGHT;
if ($debug_summed_bin_cap != $total_row_area) {
    # Checking
    print "ERROR: Summed_bin_cap_: $summed_bin_cap Total Row_Area: $total_row_area\n";
    my_exit(); 
}

$num_entry = 0;
foreach $bin (@{$CMAP->{BINS}}) {
    $bin_lx = $bin->{LX};
    $bin_ly = $bin->{LY};
    $bin_hx = $bin->{HX};
    $bin_hy = $bin->{HY};
    $cap = $bin->{CAP};
    $musage = $bin->{MUSAGE};
    $fusage = $bin->{FUSAGE};

#    print "Bin ($bin_lx, $bin_ly)-($bin_hx, $bin_hy): cap $cap musage $musage fusage $fusage\n";
    $num_entry++;
}
print "Phase $phase: CMAP Dim: $XDIM x $YDIM BinSize: $XUNIT x $YUNIT Total $num_entry bins.\n";
$phase++;

###################################
# Node file processing
##################################
$num_obj = 0;
$num_terminal = 0;
$total_movable_area = 0;
while (defined($line = <NODEFILE>)) {
    $line =~ s/^\s+//; # removes front/end white spaces
    $line =~ s/\s+$//;
    @words = split(/\s+/, $line);
    @words + 0;
    $num_words = scalar(@words);
    if ($words[0] eq "#" || $words[0] eq "UCLA" || $num_words < 1) {
	# skip comment or UCLA line or empty line
	next;
    }
    if ($words[0] eq "NumNodes") {
	$num_obj_from_file = $words[2];
	$line = <NODEFILE>;
	$line =~ s/^\s+//; # removes front/end white spaces
	$line =~ s/\s+$//;
	@words = split(/\s+/, $line);
	$num_term_from_file = $words[2];
	print "         NumNodes: $num_obj_from_file NumTerminals: $num_term_from_file\n";
	next;
    }
    
    $name = $words[0];
    $dx   = $words[1];
    $dy   = $words[2];
    if ($words[3] eq "terminal") {
	$move_type = "terminal";
	$num_terminal++;
    }
    else {
	$move_type = "movable";
	$total_movable_area += ($dx * $dy);
    }
    $lx = 0;    
    $ly = 0;
    $record = {
	NAME => $name,
	DX   => $dx,
	DY   => $dy,
	LX   => $lx,
	LY   => $ly,
	TYPE => $move_type,
    };
    $ObjectDB{ $record->{NAME} } = $record;
#    printf "%s dx=%d dy=%d (%d, %d) type=%s\n", 
#    $record->{NAME}, $record->{DX}, $record->{DY}, $record->{LX}, $record->{LY}, $record->{TYPE};
    $num_obj++;
}

$num_entry=0;
while (($name, $record) = each %ObjectDB) {
#    printf "ODB %s dx=%d dy=%d (%d, %d) type=%s\n", 
#    $record->{NAME}, $record->{DX}, $record->{DY}, $record->{LX}, $record->{LY}, $record->{TYPE};
    $num_entry++;
}
close(NODEFILE);
print "Phase $phase: Node file processing is done. Total $num_obj objects (terminal $num_terminal)\n";
print "         Total $num_entry entries in ObjectDB\n";
print "         Total movable area: $total_movable_area\n";
$phase++;

###################################
# Solution PL file processing
# Also checks ErrorType0: whether a terminal object moved or not
###################################
$num_obj = 0;
$num_terminal = 0;
while (defined($line = <SOLFILE>)) {
    my @tmpRecord;
    
    $line =~ s/^\s+//; # removes front/end white spaces
    $line =~ s/\s+$//;
    @words = split(/\s+/, $line);
    @words + 0;
    $num_words = scalar(@words);
    if ($words[0] eq "UCLA" || $words[0] eq "#" || $num_words < 1) {
	# skip comment line or UCLA line or empty line
	next;
    }
    
    $name = $words[0];
    $locx = $words[1];
    $locy = $words[2];

#kaie
    $direction = $words[4];
#@kaie

    $record = $ObjectDB{$name};
    if (!defined($record)) {
	print       "ERROR: Undefined object $name appear in Solution PL file.\n";
	my_exit();
    }

    $move_type = $record->{TYPE};
    if ($move_type eq "terminal") {
	$num_terminal++;
    }
    
#kaie
    if($direction eq "W" || $direction eq "E" || $direction eq "FW" || $direction  eq "FE")
    {
	$obj_dx = $record->{DX};
	$obj_dy = $record->{DY};
	$record->{DX} = $obj_dy;
	$record->{DY} = $obj_dx;
    }
#@kaie

    $record->{LX} = $locx;
    $record->{LY} = $locy;
    $OjbectDB{ $record->{NAME} } = $record;
    $num_obj++;
}
close(SOLFILE);
print "Phase $phase: Solution PL file processing is done.\n";
print "         Total $num_obj objects (terminal $num_terminal)\n";
$phase++;

#####################################################
# Congestion Map Generation
#####################################################
$num_entry = 0;
$num_terminal = 0;
while (($name, $record) = each %ObjectDB) {
    $name = $record->{NAME};
    $obj_lx = $record->{LX};
    $obj_ly = $record->{LY};
    $obj_dx = $record->{DX};
    $obj_dy = $record->{DY};
    $type = $record->{TYPE};

    $obj_hx = $obj_lx + $obj_dx;
    $obj_hy = $obj_ly + $obj_dy;

    if ($type eq "terminal" && 
	(($obj_hx < $WINDOW_LX) || ($obj_lx > $WINDOW_HX) || ($obj_hy < $WINDOW_LY) || ($obj_ly > $WINDOW_HY))) {
	$num_terminal++;
	$num_entry++;
	next;
    }

    $bin_lx_index = floor(($obj_lx - $WINDOW_LX)/$XUNIT);
    $bin_ly_index = floor(($obj_ly - $WINDOW_LY)/$YUNIT);
    $bin_hx_index = floor(($obj_hx - $WINDOW_LX)/$XUNIT);
    $bin_hy_index = floor(($obj_hy - $WINDOW_LY)/$YUNIT);
    
    if ($bin_lx_index < 0 || $bin_ly_index < 0) {
	#print "ERROR: Bin low index error during object $name processing: lx_index: $bin_lx_index ly_index: $bin_ly_index\n";
	print "ERROR: object $name is placed out of the placement region\n";
	my_exit();
    }
    if ($bin_hx_index > $XDIM || $bin_hy_index > $YDIM) {
	print "Bin high index error during object $name processing: hx_index: $bin_hx_index hy_index: $bin_hy_index\n";
	print "ERROR: object $name is placed out of the placement region\n";
	my_exit();
    }
    if ($bin_hx_index == $XDIM) {
	# case where object right edge aligns with chip boundary...
	$bin_hx_index--;
    }
    if ($bin_hy_index == $YDIM) {
	# case where object top edge aligns with chip boundary...
	$bin_hy_index--;
    }

    $debug_obj_area = $obj_dx * $obj_dy;
    $debug_summed_common_area = 0;
    for ($j = $bin_ly_index; $j <= $bin_hy_index; $j++) {
	for ($i = $bin_lx_index; $i <= $bin_hx_index; $i++) {
	    $index = $i + $j * $XDIM;

	    $bin = @{$CMAP->{BINS}}[$index];
	    $bin_lx = $bin->{LX};
	    $bin_ly = $bin->{LY};
	    $bin_hx = $bin->{HX};
	    $bin_hy = $bin->{HY};

	    # get intersection between object & bin
	    if ($obj_lx >= $bin_lx) {
		$lx = $obj_lx;
	    }
	    else {
		$lx = $bin_lx;
	    }
	    if ($obj_hx >= $bin_hx) {
		$hx = $bin_hx;
	    }
	    else {
		$hx = $obj_hx;
	    }
	    if ($obj_ly >= $bin_ly) {
		$ly = $obj_ly;
	    }
	    else {
		$ly = $bin_ly;
	    }
	    if ($obj_hy >= $bin_hy) {
		$hy = $bin_hy;
	    }
	    else {
		$hy = $obj_hy;
	    }
	    $common_area = ($hx - $lx)*($hy - $ly);
	    $debug_summed_common_area += $common_area;
	    #if ($common_area > $debug_obj_area) {
		#print "ERROR: Area calculation failure during object $name processing: Common_Area $common_area Org_Area: $debug_obj_area\n";
		#my_exit();
	    #}
	    if ($type eq "movable") {
		$bin->{MUSAGE} += $common_area;
	    }
	    else {
		$bin->{FUSAGE} += $common_area;
	    }
	}
    }
    # debugging purpose: sigma(common area) == original object area
    #if ($debug_summed_common_area != $debug_obj_area) {
	# Checking
	#print "ERROR: Area calculation failure during object $name processing: Sum_Area: $debug_summed_common_area Org_Area: $debug_obj_area\n";
	#my_exit(); 
    #}
    if ($type eq "terminal") {
	$num_terminal++;
    }
    $num_entry++;
}
print "Phase $phase: Congestion map construction is done.\n";
print "         Total $num_entry objects (terminal $num_terminal)\n";
$phase++;

#####################################################
# congestion map analysis
#####################################################
$nviolation = 0;
$nblockedbin = 0;
$summed_overflow = 0;
$avg_overflow = 0;
$max_overflow = 0;
$total_free_space_amount = 0;
$total_overflow_amount = 0;
for ($j = 0; $j < $YDIM; $j++) {
    for ($i = 0; $i < $XDIM; $i++) {
	$index = $i + $j * $XDIM;
	$total_bin_count++;
	
	$bin = @{$CMAP->{BINS}}[$index];
	$bin_lx = $bin->{LX};
	$bin_ly = $bin->{LY};
	$bin_hx = $bin->{HX};
	$bin_hy = $bin->{HY};
	$bin_cap = $bin->{CAP};
	$bin_musage = $bin->{MUSAGE};
	$bin_fusage = $bin->{FUSAGE};

	#if ($bin_cap < $bin_musage + $bin_fusage) {
	    #print "ERROR: bin overflow cap: $bin_cap musage: $bin_musage fusage: $bin_fusage\n";
	    #my_exit();
	#}

	$total_capacity += $bin_cap;
	$total_musage   += $bin_musage;
	$total_fusage   += $bin_fusage;

	$bin_free_space = $bin_cap - $bin_fusage;
	if ($bin_free_space != 0) {
	    $bin_utilization = $bin_musage / $bin_free_space;
	    if ($bin_utilization > $target_density) {
		$overflow = ($bin_utilization - $target_density);
		$nviolation++;
		$summed_overflow += $overflow;
		if ($overflow > $max_overflow) {
		    $max_overflow = $overflow;
		}
		$total_free_space_amount += $bin_free_space;
		$total_overflow_amount += ($bin_musage - $target_density * $bin_free_space);
	    }
	}
	else {
	    if ($bin_free_space < 0) {
		print "ERROR: bin_free_space is negative ($bin_free_space)\n";
		my_exit();
	    }
	    $nblockedbin++;
	}
    } # for ($i)
} # for ($j)

$avg_overflow = $summed_overflow == 0 ? 0 : $summed_overflow / $nviolation;
printf "Phase $phase: Congestion map analysis is done.\n";
printf "\tTotal %d (%d x %d) bins. Target density: %f\n", $total_bin_count, $XDIM, $YDIM, $target_density;
printf "\tViolation num: %d (%f)\tAvg overflow: %f\tMax overflow: %f\n", $nviolation, $nviolation/$total_bin_count, $avg_overflow, $max_overflow;
printf "\tOverflow per bin: %f\tTotal overflow amount: %f\n", $total_overflow_amount/$total_bin_count, $total_overflow_amount;
#$scaled_overflow_per_bin = ($total_overflow_amount*$XUNIT*$YUNIT*$target_density)/($total_movable_area * 400);
$scaled_overflow_per_bin = ($total_overflow_amount*36)/($total_movable_area);
printf "\tScaled Overflow per bin: %f\n", $scaled_overflow_per_bin * $scaled_overflow_per_bin;
