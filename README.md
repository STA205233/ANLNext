ANL Next
========================================

- Version: 1.7.0
- Author: Hirokazu Odaka


Introduction
----------------------------------------

ANL Next is a framework to generate software which performs event-by-event
analysis. The event-by-event analysis is defined as data analysis which treats
a list of event entries as input and applies some processes to each event
entry. Since algorithm for each entry must be identical for all events, the
entire program consists of a loop of a procedure that realizes the algorithm
for all the entires. We usually need such a data processing for experimental
data obtained with radiation detectors including X-ray and gamma-ray telescopes
for high-energy astrophysics.

ANL Next provides us with easy ways to develop and run a software application
of event-by-event analysis. In this framework, an analysis loop is treated as
an "analysis chain", which is a list of "ANL modules". Each ANL module is
responsible for one single task, or relatively simple process. In a single
loop, the manager process sequentially calls ANL modules in the analysis chain.
In this scheme, Users can concentrate on the simple task applied to one event
entry. Once modules are prepared, the users can run applications with various
configurations of the analysis chain, without additional development.

While ANL Next adopts the conventional ANL concept about the event-by-event
loop, we have written the code completely from scratch. This framework is
written in C++, and provides Ruby binding (extention library) for dynamical and
flexible use of analysis applications. To operate an analysis applications,
what you need to do is only writing a simple Ruby script, in which you define
an analysis chain and set parameters to the ANL modules. The application build
by ANL Next are as fast as C++ code, and as flexible as Ruby code.

This framework is originated from the ANL++ framework (by Shin Watanabe), which
is C++ implementation of ANL. The original ANL was a data analysis framework
developed by the PEP-4 TPC collaboration at the Lawrence Berkeley Laboratory
in 1980s. It has been improved on data sharing methods between ANL modules in
1990s by T. Takahashi, H. Kubo, Y. Ishisaki, et al., and maintenaced for data
processing of X-ray detectors onboard the ASCA and Suzaku X-ray observatories.

### Contact

- Hirokazu Odaka 
- ISAS/JAXA
- odaka(AT)astro.isas.jaxa.jp

### GitHub

https://github.com/odakahirokazu/ANLNext/

### Platforms

- Mac (the author's main enviroment for development and testing)
- Linux

#### Test environmemt (Mac)

- MacBook Pro (Late 2013)
- OS X 10.10.5
- Apple LLVM version 6.1.0 (clang-602.0.53) (based on LLVM 3.6.0svn)
- cmake version 3.3.1
- ruby 2.2.2p95 (2015-04-13 revision 50295) [x86\_64-darwin14]
- SWIG Version 3.0.7
- Boost 1.58.0

#### Test environmemt (Linux)

- a virtual machine (Parallels Desktop) on Mac
- ubuntu 14.04 LST
- gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1 14.04)
- cmake version 2.8.12.2
- ruby 2.2.3p173 (2015-08-18 revision 51636) [x86\_64-linux]
- SWIG Version 3.0.7
- Boost 1.58.0

### API Reference

http://www.astro.isas.jaxa.jp/~odaka/anlnext/doxygen/

### Applications using ANL Next

- ComptonSoft
- MONACO
- HXISGDDataReceiver

### Licence

ANL Next is distributed under the GNU General Public License version 3.


ANL model
----------------------------------------

(now writing...)
![ANL model](./documentation/anlmodel.png)


Installation
----------------------------------------

### Required Software

#### (1) C++ compliler

#### (2) [Boost C++ library](http://www.boost.org/)
*version 1.56.0 or later*

#### (3) [CMake](http://www.cmake.org/)
*version 3.0 or later*

This package uses CMake (Cross platform make) for easy installation.

#### (4) [Ruby](http://www.ruby-lang.org/en/)
*version 2.0 or later*

ANL Next provides a Ruby extended library, which is generated by SWIG, as
interface to operate an analysis software generated by this framework. Ruby is
a scriptable, dynamic, object-orriented programming language, and offers
powerful and flexible interface to control the user program. Ruby is widely
used in the world, and is easy to learn like Python. This is optional but very
useful for productive data analysis.

#### (5) [SWIG](http://www.swig.org/)
*version 3.0 or later*

As SWIG (Simplified Wrapper and Interface Generator) provides an easy way to
generate extended libraries of various scripting languages, ANL Next uses SWIG
for making Ruby binding of itself.


### Installation Guide

#### (1) Obtain ANL Next

    $ git clone https://github.com/odakahirokazu/ANLNext.git

#### (2) Perform CMake

Make a directory for building the software, and then move to the directory.

    $ cd ANLNext
    $ mkdir build
    $ cd build

Perform cmake to generate Makefiles. Give the directory of the source tree to
`cmake` command.

    $ cmake .. [options] -DCMAKE_INSTALL_PREFIX=/path/to/install

There are several options:

- `ANL_USE_RUBY` (Default=ON): enable Ruby binding.
- `ANL_USE_EXE` (Default=OFF): enable standalone executables.
- `ANL_USE_TVECTOR` (Default=OFF): enable ROOT vector.
- `ANL_USE_HEPVECTOR` (Default=OFF): enable CLHEP vector.
- `ANL_USE_G4VECTOR` (Default=OFF): enable Geant4-CLHEP vector.
- `ANL_INSTALL_HEADERS` (Default=ON): install all header files.
- `ANL_INSTALL_CMAKE_FILES` (Default=ON): install all cmake files.

By default, the install destination is set to `${HOME}`. So `make install` will
install headers and libraries into the user's home directory, such as
`${HOME}/include` or `${HOME}/lib`. You can change it by setting
`CMAKE_INSTALL_PREFIX`.

#### (3) Make and install

    $ make -jN (N: number of parallel complilation processes)
    $ make install

#### (4) Set environment variables

If you installed ANL Next into the $HOME directory (default destination), you
need to set the following environment variables:  
(example for bash/zsh)

    export DYLD_LIBRARY_PATH=${HOME}/lib:${DYLD_LIBRARY_PATH}
    export RUBYLIB=${HOME}/lib/ruby:${RUBYLIB}

Or, you may need to set the following envrionment variables:

    export ANLNEXT_INSTALL=/path/to/install
    export DYLD_LIBRARY_PATH=${ANLNEXT_INSTALL}/lib:${DYLD_LIBRARY_PATH}
    export RUBYLIB=${ANLNEXT_INSTALL}/lib/ruby:${RUBYLIB}


Getting Started
----------------------------------------

By using this framework, you can easily construct your own applications. If
you want a new application containing only existing modules, you can quickly
start with writing a Ruby script that defines an **analysis chain** descibing
an order of the ANL modules and sets parameters passed to the modules. This
enviroment provides you with powerful and flexible scripting framework.

### How to write a Ruby script operating an ANL application

You can see the simplest example of ANL application in
[examples/simple_loop](./examples/simple_loop). This application performs loop
in which nothing happens. To build it, you can use cmake:

    $ cd examples/simple_loop
    $ mkdir build
    $ cd build
    $ cmake ../source -DCMAKE_INSTALL_PREFIX=/path/to/install
    $ make
    $ make install

To run this application, see [run/run_anl_example.rb](./examples/simple_loop/run/run_anl_example.rb).
To use the extension libarary, it is necessary to *require* ANL libraries.

```ruby
require 'ANLLib' # ANL Next library
require 'myPackage' # Ruby extension library using ANL Next
```

At the beginning, you need to define your own application class which should
be derived from *ANL::ANLApp* class. Then, you can define the analysis chain
in *setup()* method. In this method, you specify the order of ANL modules in
the analysis chain, and set parameters of the modules. To push a module to
the analysis chain, you can use *chain()* method. Just after *chain()*, you can
set parameters by using *with_parameters()*.

```ruby
# Define your own application class derived from ANL::ANLApp.
class MyApp < ANL::ANLApp
  # Define an analysis chain in setup() method.
  def setup()
    chain MyPackage::MyModule
    with_parameters(MyParameter1: 10,
                    MyParameter2: 20,
                    MyParameter3: 30)

    # If you need to add the same type of module, you should set another name
    # via the second argument.
    chain MyPackage::MyModule, :MyModule2
    with_parameters(MyParameter2: 21)
  end
end
```

*chain()* method takes two parameters: module class and module ID. If module ID
is not specified, the class name is used for the module ID. The module ID should
be unique in a single analysis chain. If you need to add the same type of
module, it is necessary to set another module ID via the second argument.

After setting the parameter, just instantiate the application and run. The
number of the analysis loop can be passed through the first argument. The
second argument is optional for setting the display frequency.

```ruby
app = MyApp.new
app.run(100000)
```

If you need to set infinite loop, set -1 as the loop number. The second
argument is the display frequency (optional).

```ruby
app.run(-1, 100000)
```

After the run, you will get the below message. From this message, you find the
definition of the analysis chain and module parameters, which are shown before
the analysis phase. After the analysis phase finishes, you get information on
the status of the analysis chain.

```
$ ./run_anl_example.rb 

      ***********************************
      ****          ANL Next         ****
      ***********************************

ANLManager: startup routine started.
ANLManager: prepare routine started.

      ***********************************
      ****      Analysis chain      *****
      ***********************************

 #                    Module ID                         Version    ON/OFF 
----------------------------------------------------------------------------
   1    MyModule                                          1.0        ON      
   2    MyModule2 (MyModule)                              1.0        ON      


      ***********************************
      ****     Module parameters     ****
      ***********************************

--- MyModule ---
MyParameter1: 10
MyParameter2: 20
MyParameter3: 30

--- MyModule2 ---
MyParameter1: 1
MyParameter2: 21
MyParameter3: 3

ANLManager: init routine started.
ANLManager: his routine started.
ANLManager: initialization done.


Analysis Begin  | Time: 2014-06-19 16:02:51 +0100
ANLManager: start bgnrun routine.
ANLManager: bgnrun routine started.
ANLManager: start analysis (with thread mode on).
You can quit the analysis routine by input 'q'.
Event :          0
Event :     100000
Event :     200000
Event :     300000
Event :     400000
Event :     500000
Event :     600000
Event :     700000
Event :     800000
Event :     900000
ANLManager: start endrun routine.
ANLManager: endrun routine started.

      ***********************************
      ****      Analysis chain      *****
      ***********************************
               PUT: 1000000
                |
     [  1]  MyModule  version  1.0                  
       1000000  |  OK: 1000000  SKIP: 0  ERR: 0
     [  2]  MyModule/MyModule2  version  1.0        
       1000000  |  OK: 1000000  SKIP: 0  ERR: 0
               GET: 1000000


Analysis End    | Time: 2014-06-19 16:02:52 +0100
ANLManager: exit routine started.

***** Results of Event Selection *****
    Number of EVS :        0

ANLManager: exiting...
```

### A more practical example

You can find a more practical example in
[examples/simple_loop](./examples/simple_loop). Below is a Ruby script to run
this application. It has three ANL modules: *SaveData*, *GenerateEvents*, and
*FillHistogram*. *SaveData* is a utility module to save analysis products to a
ROOT file. It does nothing in the analysis phase. *GenerateEvents* generates
an event data that have a property of energy by a some simulation based on a
random number. *FillHistogram* gets generated event information and then makes
histograms of the energy of the events. This example is a minimal application
that has a practical function, telling us how an ANL application works.

```ruby
require 'ANLLib'
require 'testHistogram'

class MyApp < ANL::ANLApp
  def setup()
    chain TestHistogram::SaveData
    with_parameters(filename: "output.root")

    chain TestHistogram::GenerateEvents
    with_parameters(energy: 120.0,
                    detector1_sigma: 1.0,
                    detector2_sigma: 5.0)

    chain TestHistogram::FillHistogram
    with_parameters(nbin: 128,
                    energy_min: 80.0,
                    energy_max: 150.0)
  end
end

a = MyApp.new
a.run(1000000, 100000)
```



Development of Your Applications
----------------------------------------

If you want to develop analysis software which applies your original algorithm,
you need to develop a new ANL module. You can find a simple example of your own
ANL application in *examples* directory.

### Design

### ANL Module

#### Structure of ANL module

#### Methods defined in an ANL module

#### Registering parameters

### Data Sharing between ANL Modules
    
#### Event selection flag (EVS)

#### Accessing information in other modules

You can access other modules via GetANLModule() method.

```c++
// Declaration of a const pointer to a module you want to access.
const YourModule* module;

// Get a pointer to the module.
GetANLModule("YourModule", module);

// Get some parameter via a member method of the module.
// The member method must be declared as const.</span>
int yourParam = module->getYourParam();
```

#### Action to other modules without "const"

GetANLModuleNC() gives non-const pointer of a module. NC stands for non-const.

```c++
// Declaration of a pointer to a module you want to access.
YourModule* ym;

// Get a pointer to the module.
GetANLModuleNC("YourModule", ym);

// Call methods of the module.
ym->setMode(mode);
ym->doSomething();
```
    
#### Ruby binding

#### Build settting

#### Run


Frequently Asked Questions
----------------------------------------

### Differences from ANL/ANL++

#### How can I use BNK?



****************************************
