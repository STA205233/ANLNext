ANL Next
================================================================

- Version: 1.2
- Author: Hirokazu Odaka


 1. Introduction
----------------------------------------------------------------

ANL Next is a framework to generate software which performs
event-by-event analysis. The event-by-event analysis is defined
as data analysis which treats a list of event entries as input
and applies some processes to each event entry. Since algorithm
for each entry must be identical for all events, the entire
program consists of a loop of a procedure that realizes the
algorithm for all the entires. We usually need such a data
processing for experimental data obtained with radiation
detectors including X-ray and gamma-ray telescopes for
high-energy astrophysics.

ANL Next provides us with easy ways to develop and run a
software application of event-by-event analysis. In this
framework, an analysis loop is treated as an "analysis chain",
which is a list of "ANL modules". Each ANL module is
responsible for one single task, or relatively simple process.
In a single loop, the manager process sequentially calls ANL
modules in the analysis chain. In this scheme, Users can
concentrate on the simple task applied to one event entry.
Once modules are prepared, the users can run applications with
various configurations of the analysis chain, without
additional development.

While ANL Next adopts the conventional ANL concept about the
event-by-event loop, we have written the code completely from
scratch. This framework is written in C++, and provides Ruby
binding (extention library) for dynamical and flexible use of
analysis applications. To operate an analysis applications,
what you need to do is only writing a simple Ruby script, in
which you define an analysis chain and set parameters to the
ANL modules. The application build by ANL Next are as fast as
C++ code, and as flexible as Ruby code.

This framework is originated from the ANL++ framework
(by Shin Watanabe), which is C++ implementation of ANL. ANL is
a fundamental framework of event-by-event analysis, which has
been used for data processing of X-ray detectors on board ASCA
and Suzaku X-ray observatories.


 2. Information
----------------------------------------------------------------

### (1) Contact
- Hirokazu Odaka 
- ISAS/JAXA
- odaka(AT)astro.isas.jaxa.jp

### (2) Documentation

http://www.astro.isas.jaxa.jp/~odaka/anlnext/

### (3) Git Hub

https://github.com/odakahirokazu/ANLNext/


 3. Supported System
----------------------------------------------------------------

- Mac OS X
- (Linux): possible to run on Linux


 4. Required software
----------------------------------------------------------------

### (1) C++ compliler

### (2) [CMake] (http://www.cmake.org/) (Cross platform make)
[version 2.8 or later]  
For easy installation, this package uses CMake to generate
building tools such as Makefile.

### (3) [Boost C++ library] (http://www.boost.org/)
[version 1.48.0 or later]  

### (4) [Ruby] (http://www.ruby-lang.org/en/)
[Ruby 2.0 is recommended.]  
ANL Next provides a Ruby extended library, which is generated
by SWIG, as interface to operate an analysis software
generated by this framework. Ruby is a scriptable, dynamic,
object-orriented programming language, and offers powerful and
flexible interface to control the user program. Ruby is widely
used in the world, and is easy to learn like Python. This is
optional but very useful for productive data analysis.

### (5) [SWIG] (http://www.swig.org/) (Simplified Wrapper and Interface Generator)
[version 2.0 or later]  
As SWIG provides an easy way to generate extended libraries
of various scripting languages, ANL Next uses SWIG for making
Ruby binding of itself.


 5. Installation
----------------------------------------------------------------

### (1) Obtain ANL Next.

    unix> git clone git://github.com/odakahirokazu/ANLNext.git

### (2) Perform CMake.

Make a directory for building the software, and then move to
the directory.

    unix> cd ANLNext
    unix> mkdir build
    unix> cd build

Perform cmake to generate Makefiles. Give the directory of
the source tree to "cmake" command.

    unix> cmake .. [options] -DCMAKE_INSTALL_PREFIX=/path/to/install

There are several options:

- ANL_USE_RUBY [Default=ON]: enable Ruby binding.
- ANL_USE_EXE [Default=OFF]: enable standalone executables.
- ANL_USE_TVECTOR [Default=OFF]: enable ROOT vector.
- ANL_USE_HEPVECTOR [Default=OFF]: enable CLHEP vector.
- ANL_USE_G4VECTOR [Default=OFF]: enable Geant4-CLHEP vector.
- ANL_INSTALL_HEADERS [Default=ON]: install all header files.
- ANL_INSTALL_CMAKE_FILES [Default=ON]: install all cmake files.

By default, the install destination is set to ${HOME}.
So "make install" will install binaries and libraries into
the user's home directory, such as ${HOME}/bin or ${HOME}/lib.
You can change it by setting CMAKE_INSTALL_PREFIX.

### (3) Make and install

    unix> make
    unix> make install
