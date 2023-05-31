# teriOS

teriOS is a prototype nanokernel aimed at exploring the potential of [CHERI](https://www.cl.cam.ac.uk/research/security/ctsrd/cheri/)-enabled [Arm Morello boards](https://www.cl.cam.ac.uk/research/security/ctsrd/cheri/cheri-morello.html) for implementing trusted execution environments.

The contents of this repository will be particularly valuable to developers interested in running bare-metal pure capability code across all exception levels on Arm Morello boards.

teriOS can schedule multiple tasks, using a timer interrupt to initiate a context switch. An example system call (SVC) is also implemented to print text to the UART.

## Compiling

The code, located in the `src` directory, has been tested on the Morello FVP, rather than the SoC. It has been tested and compiled using the Arm Development Studio. However, creating a `Makefile` should be straightforward. Once compiled, it can be loaded by constructing a BL33 payload. The documentation below explains this further.

## Documentation

A comprehensive project report accompanies this repository, providing in-depth documentation on the design and implementation of teriOS, along with relevant background information, related work, and future prospects.

The documentation below outlines the process for setting up and configuring the development environment.

1. Morello Fixed Virtual Platform (FVP)
    1. [Installing the FVP](https://developer.arm.com/downloads/-/arm-ecosystem-fvps)
    1. [Running the FVP](https://git.morello-project.org/morello/docs/-/blob/morello/mainline/common/standalone-baremetal-readme.rst#id6)
1. Compiling code
    1. [Obtaining the cross compilation toolchain](https://git.morello-project.org/morello/llvm-project-releases)
1. Arm Development Studio (Compilation and Debugging)
    1. [Installing the Arm Development Studio](https://developer.arm.com/downloads/-/morello-development-tools-downloads)
    1. [Configuring the Arm Development Studio](https://github.com/cap-tee/cheri-docs/blob/main/morello/BareMetalOnMorello/HelloWorldArmDevStudio/HelloWorldArmDevStudio.md)
    1. [Debugging at EL3](/docs/ads-el3-debug.md)
    1. [Debugging at EL2 (TF-A BL33 payload)](/docs/ads-el2-debug.md)
   
There is also some additional [third-party documentation](https://github.com/cap-tee/cheri-docs/blob/main/morello-getting-started.md). However, be aware that some parts are now outdated, and certain steps are no longer necessary.
