# cleanpath

[![CMake](https://github.com/jhunkeler/cleanpath/actions/workflows/cmake.yml/badge.svg)](https://github.com/jhunkeler/cleanpath/actions/workflows/cmake.yml)

`cleanpath` is a utility that filters unwanted elements from an environment variable.


# Installation

```shell
$ git clone https://github.com/jhunkeler/cleanpath
$ cd cleanpath
$ mkdir -p build
$ cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
$ make install
```

# Usage

```shell
usage: cleanpath [-hVelrsv] [pattern ...]
  --help       -h    Displays this help message
  --version    -V    Displays the program's version
  --exact      -e    Filter when pattern is an exact match (default)
  --loose      -l    Filter when any part of the pattern matches
  --regex      -r    Filter matches with (Extended) Regular Expressions
  --sep [str]  -s    Use custom path separator (default: ':')
  --env [str]  -E    Use custom environment variable (default: PATH)
```

# Example

A typical MacOS path with Macports installed:
```shell
/opt/local/bin:/opt/local/sbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/opt/X11/bin:/Library/Apple/usr/bin
```

## Remove MacPorts from the PATH

### Exact match (default)
```shell
$ cleanpath /opt/local/bin /opt/local/sbin
/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/opt/X11/bin:/Library/Apple/usr/bin
```

### Loose match
```shell
$ cleanpath -l /opt/local
/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/opt/X11/bin:/Library/Apple/usr/bin
```

### Regex match

```shell
$ cleanpath -r ^/opt/local/.*
/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/opt/X11/bin:/Library/Apple/usr/bin
```

## Modifying other environment variables

```shell
$ TESTVAR=a:b/c:d/e:f
$ cleanpath -E TESTVAR -s / -l c
a:b/e:f
```

## Using cleanpath in a script

```shell
#!/usr/bin/env bash
# Remove MacPorts and Fink
PATH=$(cleanpath -r '^/opt/local/.*' '^/opt/sw/.*')
export PATH
```
