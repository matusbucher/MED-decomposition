[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)


# MED-decomposition tester

This system provides test tool for MED-decomposition of cubic (3-regular) graphs. The system has a command-line interface with an option to read from or write to file and also different options for output format.


## Building

So far, building was tested only for Linux. Soon build scripts for other OS will be added as well.

### Linux

The basic requirements are **Make** and **g++** compiler. You can install them easily by running the command `sudo apt-get install build-essential` for Make and the command `sudo apt-get install g++` for g++ compiler.

You can (but don't need to) install **cryptominsat** library from [here](https://github.com/msoos/cryptominisat) for significantly better performance and it is strongly recommended. Preferred are realeses 5.11.21+, but some older realeses should also work (follow build and install instructions provided at their github repository).

Then to build the project with installed cryptominisat library, simply issue:

```
make
```

or to build without cryptominisat library issue:

```
make NOSAT=1
```

The binary will be located in `bin` folder.


## Usage

> ***NOTE:*** *All of the following instructions can be shown by running* `./med help`.

Command syntax:

```
./med [--input | -i <path>] [--output | -o <path>] [--mode | -m <mode_name>] [--time | -t]
```

Options description:

- `--input`, `-i`\
Specifies input file from which the program should read graphs. If omitted, standard input is used.

- `--output`, `-o`\
Specifies output file to which the program should write results. If omitted, standard output is used.

- `--mode`, `-m`\
Specifies formating of output. Use with one of the following mode names (default is 'only result' mode):
    - `onlyresult` / `or` / `r`\
    Prints only the answer 'true' or 'false' ('true' means graph is MED-decomposable).
    - `notdecomposable` / `nd` / `d`\
    Prints only numbers of those graphs, that are not MED-decomposable.
    - `notdecomposablebridgeless` / `ndb` / `b`\
    Prints only numbers of those graphs, that are not MED-decomposable and bridgeless at the same time.
    - `coloring` / `c`\
    Prints one of possible MED decomposition or 'false' if there is no such decomposition. Output format is similar to the input format, except 'colors' are added to each edge. 'm' is matching edge, 'c' is cycle edge, 's' is double-star center edge and 'h' is double-star point edge.

- `--time`, `-t`\
Adds execution time at the end of the output.


## Input format

The input should consist of only integers separated by any whitespace. The first integer is a total number of tested graphs. Then follows that number of graph descriptions. A graph description starts with the number of graph (can be actually any number), followed by the number of vertices. Vertices are by default numbered from 0 to n-1, where n is the number of vertices. Next n "lines" represents the adjency list - each "line" consists of three integers. If k-th line has integers a, b and c, then vertex k is adjacent to vertices a, b and c (it does not matter in which order are integers a, b, c). Similarly, the a-th "line" should contain number k, otherwise reading the graph from input causes an error. Here is an example:

```
2       // number of graphs
1       // graph n. 1
4       // number of vertices in graph n. 1
1 2 3   // vertices adjacent to vertex 0
0 2 3   // vertices adjacent to vertex 1
0 1 3   // vertices adjacent to vertex 2
0 1 2   // vertices adjacent to vertex 3
2       // graph n. 2
10      // ...
1 2 3
0 4 5
0 6 7
0 8 9
1 7 8
1 6 9
2 5 8
2 4 9
3 4 6
3 5 7
```
> ***NOTE:*** *Don't write comments in the actual input file.*