#!/usr/bin/perl -w

# file:        mdoc.pl
# author:      Andrea Vedaldi
# description: Extract MATLAB M-Files documentation

# AUTORIGHTS
# Copyright (C) 2006 Andrea Vedaldi
#       
# This file is part of MDOC.
# 
# MDOC is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

# --------------------------------------------------------------------
#                                                        Parse options
# --------------------------------------------------------------------

use Pod::Usage ;
use Getopt::Long ;

# Debugging level
$verb = 0 ;

# PDF document location
$pdfdoc = '' ;

# Options
$exclude_test = 0 ;

# Starting directory
@start  = ('.') ;

if( @ARGV > 0 ) {
  Getopt::Long::Configure('bundling') ;
  $succ = GetOptions('verbose|v'    => \$verb,
                     'help|?'       => \$help,
                     'pdfdoc=s'     => \$pdfdoc,
                     'exclude|x=s'  => \$exclude,
                     'exclude-test' => \$exclude_test,
                     'output|o=s'   => \$output) ;
  if( not $succ ) {
    pod2usage("Invalid options.") ;
  }
}

if( $help ) {
  pod2usage({ -exitval => 0 }) ;
}

if( @ARGV > 0 ) {
  @start = @ARGV ;
}

# --------------------------------------------------------------------
#                                                           Do the job
# --------------------------------------------------------------------

# This is the queue of directories to process.
# It gets filled recursively.
@dir_fifo = @start ;
print STDERR "Scanning @dir_fifo.\n" if $verb ;

# This will hold an entry for each m-file found
%mfiles = () ;

# This will hold an entry for each module found 
%modules = () ;

# #ARGV is the index of the last element, which is 0
if ( $output ) {
  open(FOUT,">$output") ;
  print STDERR "Writing to file '$output'.\n" if $verb ;
} else {
  *FOUT= *STDOUT; 
  print STDERR "Writing to standard output.\n" if $verb ; 
}

# Each module is a subdirectory. The subdirectory path is used
# as module ID.

MODULE:
while ($module_path = shift(@dir_fifo)) {
  print STDERR "=> '$module_path'\n" if $verb ;
  
  if( $exclude ) {
    next MODULE if $module_path =~ /^$exclude$/ ;
  }

  # get a first version of the module name
  $module_path =~ m/.*\/([^\\]+)$/ ;
  $module_name = $1 ;
  $module_name = 'root' if not $module_name ;
  
  # start a new module    
  $module = {
    'path'        => $module_path,
    'id'          => $module_name,
    'name'        => $module_name,
    'mfiles'      => [],
    'description' => ""
  } ;
  
  # .................................................................
  opendir(DIRHANDLE, $module->{'path'}) ;
 FILE: foreach (sort readdir(DIRHANDLE)) {
   $name = $_ ;
   $path = $module->{'path'} . "/" . $_ ;
   
   if( $exclude ) {
     next FILE if $name =~ /^$exclude$/ ;
   }
   
   # push if a directory
   $_ = $path ;
   if (-d) {
     next FILE if /(\.$|\.\.$)/ ;
     push( @dir_fifo, "$_" ) ;
     next FILE ;
   }
   
   # parse if .m and not test_
   next FILE unless (/.+\.m$/) ;
   next FILE if ($exclude_test and /test_.*/) ;
   $name =~ m/(.*).m/ ;
   $name = $1 ;
   print STDERR "   + m-file '$name'\n" if $verb ;
   my ($brief,$description) = get_comment($path) ;
   
   # if the m-file is a module description then treat it
   # according
   if (/overview/) {
     print STDERR "     (module description from '$name')\n" if $verb ;
     $module->{'id'}          = $name ;
     $module->{'name'}        = $brief ;
     $module->{'description'} = $description ;
     next FILE ;
   }
     
   # use names as IDs
   $id = $name ;
   $id =~ tr/A-Z/a-z/ ;
   
   # create a new mfile object
   $mfile = {
     'id'          => $id,
     'path'        => $path,
     'name'        => $name,
     'brief'       => $brief,
     'module'      => $module,
     'description' => $description
   } ;
   
   # add a reference to this mfile
   # object to the global mfile list
   $mfiles{$id} = $mfile ;
   
   # add a reference to this mfile
   # object to the current module
   push( @{$module->{'mfiles'}}, $mfile) ;
   
   
   }
   closedir(DIRHANDLE) ;
  # ................................................................
  
  # add a reference to the current module to the global
  # module list
  $modules{$module->{'id'}} = $module ;

  if( $verb ) {
    print STDERR "   # module done (name: '" . $module->{'name'} .
        "' id: '". $module->{'id'} . "')\n" ;
  }
}
  
# ....................................................................
#                                                  write documentation
# ....................................................................
  
print FOUT <<EOF;
<html>
  <head>
    <!-- <link href="default.css" rel="stylesheet" type="text/css"/> -->
    <style type="text/css">
EOF

print_css_stylesheet() ;
  
print FOUT<<EOF;
    </style>
  </head>
  <body>
EOF

# sort modules by path
sub criterion { $modules{$a}->{'path'} cmp $modules{$b}->{'path'} ; }

MODULE:
foreach $id ( sort criterion keys %modules ) {
    my $module = $modules{$id} ;   
    my $rich_description = make_rich($module->{'description'}) ;

    # skip this module if there are no m-files and no description
    next MODULE if 
        $#{$module->{'mfiles'}} < 0 and length($rich_description) == 0;

    # Print a <div> which contains the index of the module
    print FOUT <<EOF;
    <div class='module' id='$module->{"id"}'>
      <h1>$module->{'name'}</h1>
      <div class='index'>
        <h1>Module contents</h1>
        <ul>
EOF
    foreach( @{$module->{'mfiles'}} ) {
        print FOUT "        <li><a href='#$_->{'id'}'>" 
                 . "$_->{'name'}</a></li>\n" ;
    }
    print FOUT <<EOF;
        </ul>
      </div>
      <pre>
$rich_description
      </pre>
      <div class="footer">
      </div>
    </div>
EOF
}

foreach $id (sort keys %mfiles) {
    my $mfile = $mfiles{$id} ;
    my $rich_description = make_rich($mfile->{'description'}) ;

    print FOUT <<EOF;
    <div class="mfile" id='$mfile->{"id"}'>
      <h1>
       <span class="name">$mfile->{"name"}</span>
       <span class="brief">$mfile->{"brief"}</span>
      </h1>
      <pre>
$rich_description
      </pre>
      <div class="footer">
        <a href="#$mfile->{'module'}->{'id'}">
        $mfile->{'module'}->{'name'}
        </a>
      </div>
    </div>
EOF
}

print FOUT "</body></html>" ;

# Close file
close FOUT ;

# -------------------------------------------------------------------------
sub get_comment {
# -------------------------------------------------------------------------
    local $_ ;
    my $full_name = $_[0] ;
    my $brief     = "" ;
    my @comment   = () ;
    
    my $succ = open IN,$full_name ;
    if( not $succ ) {
      print STDERR "Warning: cannot open '$full_name'\n" if $verb ;
      return ("","") ;
    }
  SCAN: 
    while( <IN> ) {
        next if /^function/ ;
        last SCAN unless ( /^%/ ) ;
        push(@comment, substr("$_",1)) ;
    }
    close IN ;

    if( $#comment >= 0 && $comment[0] =~ m/^\s*\w+\s+(.*)$/ ) {
        $brief = $1 ;
        splice (@comment, 0, 1) ;
    }

    # from the first line
    return ($brief, join("",@comment)) ;
}

# -------------------------------------------------------------------------
sub make_rich {
# -------------------------------------------------------------------------
    local $_ = $_[0] ;
    s/([A-Z]+[A-Z0-9_]*)\(([^\)]*)\)/${\make_link($1,$2)}/g ;
    s/PDF:([A-Z0-9]([_\-:.A-Z0-9]*[A-Z0-9])?)/${\make_pdf_link($1)}/g ;
    s/PDF:(\s)/${\make_pdf_link('')}$1/g ;
    return $_ ;
}

# -------------------------------------------------------------------------
sub make_link {
# -------------------------------------------------------------------------
    local $_ ;
    my $name = $_[0] ;
    my $arg  = $_[1] ;
    my $id   = $name ;

    # convert name to lower case and put into $_
    $id =~ tr/A-Z/a-z/ ;
    
    # get mfile
    my $mfile = $mfiles{$id} ;
    my $module = $modules{$id} ;
    
    # return as appropriate    
    if($mfile) {
        return "<a href='#$id'>" . $name . "</a>" . "(" . $arg . ")" ;
    } elsif($module) {
      return "<a class='module' href='#$id'>" . $name . 
          "</a>" . "(" . $arg . ")" ;
    } else {
        return $name . "(" . $arg .")" ;
    }
}

# -------------------------------------------------------------------------
sub make_pdf_link {
# -------------------------------------------------------------------------
    local $_ ;
    my $name = $_[0] ;
    my $id   = $name ;

    # convert name to lower case 
    $id =~ tr/A-Z/a-z/ ;

    # if non void prepend '#'
    $id = '#' . $id if $id ;
    
    return "<a href='${pdfdoc}$id'>PDF:$name</a>" ;
}

# -------------------------------------------------------------------------
sub print_css_stylesheet {
# -------------------------------------------------------------------------
  print FOUT <<EOF;

/* ------------------------------------------------------------------
 *                                           Documentation stylesheet
 * --------------------------------------------------------------- */

h1
{
  color: #3a3a3a ;
}

pre
{
  font-family: monaco, courier, monospace ;
  font-size: 14px ;
  color: #3a3a3a ;
}

body
{
  margin: 10px ;
  padding: 1em ;
  right: 0px ;
  font-family: arial, sans-serif ;

  color: black ;
  background-color: #fffffa ; 
}

/* ------------------------------------------------------------------
 *                                                             Module
 * --------------------------------------------------------------- */

div.module
{
  margin-bottom: 1em ;
  //background-color: #ffffff ;
}

div.module h1
{
  margin: 0px ;
  margin-bottom: 0.5em ;
  padding: 0px ;
  font-size: 1.5em ;
  border-bottom: 2px #3a3a3a solid ;
}

div.module pre
{
  padding-left: 1em ;
}

div.module div.footer
{
  clear: both ;
}

div.module div.footer :link
{
  color: white ;
  text-decoration: none ;
}

/* ------------------------------------------------------------------
 *                                                       Module index
 * --------------------------------------------------------------- */

div.module div.index
{
  font-family: sans-serif ;
  font-size: 0.8em ;

  width: 15em ;
  float: right ;
  border: 1px #ccc0c0 solid ;
  background-color: #fcf0f0 ;
}

div.module div.index h1
{
  font-size: 1em ;  
  font-weight: bold ;
  text-align: center ;
  border: none ;
  padding: 0 ;
  margin: 0 ;
}

div.module div.index ul
{
  list-style-type: square ;
  list-style-position: inside ;
  color: #3a3a3a ;
  padding: 0 ;
  margin: 0 ;
  padding: 0.3em ;
}

div.module div.index ul li
{
  margin-bottom: 0.1em ;
}


/* ------------------------------------------------------------------
 *                                                              Mfile
 * --------------------------------------------------------------- */

div.mfile
{
  background-color: white ;
  margin-bottom: 1em ;
  border: 1px #aaaaaa solid ;
}

div.mfile h1
{
  margin: 0 ;
  padding: 0 ;
  padding-left: 10px ;
  font-size: 1.3em ;  
  background-color: #f0f0ff ;
}

div.mfile h1 span.name
{
  font-family: monaco, courier, fixed ;
}

div.mfile h1 span.brief
{
  padding-left: 10px ; 
  font-size: 0.9em ;
  font-weight: normal ;  
}

div.mfile pre
{
  padding-left: 1em ;
}

div.mfile div.footer
{
  font-size: 0.8em ;
  padding: 0.3em ;
}

div.mfile div.footer a
{
  text-decoration: none ;
  color: #777777 ;
}

div.mfile div.footer a:hover
{
  text-decoration: underline ;
}

EOF
}

# -------------------------------------------------------------------------
#                                                             Documentation
# -------------------------------------------------------------------------

=head1 NAME

mdoc.pl - Summarizes MATLAB M-Files documentation

=head1 SYNOPSIS

perl mdoc.pl [B<-o> I<outfile>] [B<--pdfdoc> I<pdffile>] [OPTIONS]
[I<indir1> I<indir2> ...]

=head1 DESCRIPTION

This program scans one or multiple directoroies containing MATLAB
M-Files and extracts the embedded documentation to a single HTML file.

The program supports a few conventions that can be used to get a more
structured documentation. These are described next.

=head2 Modules

Each (sub)directory in the hierarchy is considered to be a
module. Each module can be explicitly described by a M-File called
C<overview_MODULE>. C<MODULE> is the actual name of the module. The
documentation in that file is then extracted and used in a block that
describes the module.

=head2 Links to M-Files

Whenever the documentation refers to a visible M-File, an appropriate
link is automatically generated.  For this to work, the documentation
must refer to M-Files by their name in capital letters, followed by an
open and closed parentheses. For example, C<MDOC()> would work, but
not C<mdoc()>, or C<MODC>.

=head2 Links to PDF documentation

It is possible to add links to a PDF file, which may be useful to
refer to more detailed documentation. A link has the form
C<PDF:LINKNAME> where C<LINKNAME> is the name of the link in the PDF
document. The actual PDF document can be specified by the C<-pdfdoc>
option.

=head1 OPTIONS

=over 3

=item B<-h|--help> 

Print this help message.

=item B<-o|--output> I<filename>

Specifies an output file. If no output file is specified, standard
output is used.

=item B<-pdfdoc> I<filename>

Specifies the PDF file URL to link to when C<PDF:LINKNAME> tags are
used.

=item B<-x|--exclude> I<regexp>

Skip files and directories matching the specified (Perl) regexp.

=item B<--exclude-test>

Skip files beginning with the prefix 'test_'.

=back

=head1 SEE ALSO

perl(1)

=head1 AUTHOR

Andrea Vedaldi - L<http://cs.ucla.edu/~vedaldi>

=cut

# Emacs:
# Local Variables:
# mode: Perl
# perl-indent-level: 2
# end:

