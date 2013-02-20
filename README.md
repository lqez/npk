npk - neat package system [en:pack]
===================================
[![Build Status](https://travis-ci.org/lqez/npk.png)](https://travis-ci.org/lqez/npk)

- Copyright (c) 2009-2013 Park Hyun-woo(ez.amiryo@gmail.com)
- Distributed under MIT license. (see LICENSE for detail)


# Why files should be packed?

- Open multiple files vs Open only one file.
    - If you want to open multiple files in your application, kernel should traverse FAT(file allocation tables).
      It needs lots of time - sometimes even longer than load files into memory.
      npk has its own list of files and also supports hash table for large packages.
      Yes, npk is a small logical file system itself.

- Encryption and Compression
    - tar can archive files into a package. But it needs gzip or something to compress files.
      npk embeds xxtea encyption and zlib compression for convenience. You can just read / write files via npk API, no concern.


# What is npk?

'npk' is a file package system, called 'en-pack'.

- Support file compression(zlib) and encryption(xxtea).
- Support most modern operating systems.

npk consists of below items.

- libnpk : ANSI C API based on POSIX.
- npk : Command-line interface tool for npk-formatted files.
- binding
    - pynpk : Python extension
        - https://github.com/lqez/pynpk
    - objnpk : Objective-c extension
        - https://github.com/lqez/objnpk
    - jvnpk : JNI binding
        - (will be available soon)


# Installation

- See https://github.com/lqez/npk/wiki/HowToInstall

Or, if you're on OS X and using homebrew then,

    $ brew tap lqez/npk
    $ brew install npk


# Contact

- project homepage : https://github.com/lqez/npk
- issues : https://github.com/lqez/npk/issues
- developer : ez.amiryo+npk@gmail.com


# License

Distributed under MIT/X license. See LICENSE file.
