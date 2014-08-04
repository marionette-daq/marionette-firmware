# Document Directory
---

This directory contains general documentation.

---
## Graphiz input files

### Prerequisites

* Install Graphviz (and dot)

### Generating Graph Diagrams

Files that end in **.dot** are inputs to the graph drawing program [graphviz](http://www.graphviz.org "Graphviz")

The Makefile will build all images using the dot program and output .png files. These output files are checked in to the repository if they are used in the wiki.

---
## Doxygen

There is a Doxyfile for doxygen document generation. Output will be placed in the directory **doxydocs**. This output directory is **not** committed to the repository.

### Prerequisites

* Install Doxygen
* Install Graphviz (and dot)

### Running Document Generator

To generate documents


```
cd .../marionette/Doc
doxygen
```

To view output open this file html browser: **.../doxydocs/html/index.html**

Example:


```
chromium doxydocs/html/index.html
```
---





