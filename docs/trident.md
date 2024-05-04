% trpm(1) TRPM user's manual
% ShadowDevZ
% May 4, 2024

# NAME

trpm - trident package manager

# SYNOPSIS

trpm [*operation*] ...

# DESCRIPTION

Trident is a simple package manager utility available on Linux distributions
utilizing custom `tpx(5)` file format.



# OPTIONS

-i *[package]*

    Installs package and records it to *user* package set.
    To prevent it from being recorded pass the `--norec` option

-r *[package]*

    Removes the package from the system, leaves dependencies

-rd *[package]*

    Removes the specified package with all of it's dependencies
    unless needed by other system package

-iu *[package]*

    Installs and updates the specified package

-im *[path]*

    Installs the local package provided by path

-s *[mirror]*
    
    Syncs the specified mirror repository. If no option is provided
    all repositories are synced

-u *[package]*

    Updates the package from the mirror list. It is highly advised
    to used the -su option to also sync the repository to receive
    the newest packages

-Q *[package]*

    Queries the remote package list and retrieves all names matching
    the package name or description

-l
    
    Lists all installed local packages

-F *[package]*
    
    Queries the local package database and returns all
    matching occurences

-c *[package]*
    
    Reads the package changelog/developer notice for the
    installed version of the package.

-C 

    Shows all available changelogs which are available to be read

-Ar

    Autoremoves all unused dependencies (safe)

-Arr

    Forcibly and usafely removes all dependencies
    which are deemed as unused. This can cause errors and
    missing packages. This option should only be used
    when neccessary (unsafe)

--record *[package-set]*
    
    Records the package to the specified set. If this option is not
    used. TRPM automatically records all packages to the trpm-user
    package set

--unrecord *[package-set]*

    Unrecords the specified package from the specified package set

-ls

    Lists all system package sets

--machine-readable
    
    Formats the output to use the NULL terminated characters
    instead of newlines. Also uses special tags so the output
    can be parsed more easily by the software. If you are
    developing the TRPM frontend please consider reading `trpm(5)`
    which provides library with functions for direct package
    management and manipulation whilst providing verbose error
    messages that
    can be used when debugging.

-h 
    
    Displays the program help menu

-v

    Displays the current version of libtrpm and trpm  
    
--log *[file]*
    
    Logs all operations and error to the specified file.
    If no file is provided all output is redirected to the
    STDIN.

--verbose

    Prints more verbose messages about operation

# CONFIGURATION

Please refer to the `trpmconf.xml(5)`

# SEE ALSO
`trpmconf.xml(5)`, `libtrpm.so(5)`

# BUGS
If you find a bug in either the libtrpm.so or the trpm
please open an issue on the github page <https://github.com/ShadowDevZ/trident-pm>
