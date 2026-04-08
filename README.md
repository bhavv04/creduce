# creduce

A MapReduce engine built from scratch in C. No Hadoop, no JVM, no abstractions.

## What it does

Implements the full MapReduce programming model — map, shuffle, sort, reduce — with a parallel map phase via pthreads. Designed to process large datasets efficiently and benchmarked against real workloads.

## Architecture

- **Function pointer job API** — define any job by passing map and reduce functions, just like the original Google MapReduce
- **KVList** — dynamic key-value array with automatic reallocation
- **Map phase** — splits input across N worker threads via pthreads
- **Shuffle phase** — sorts all key-value pairs by key using qsort
- **Reduce phase** — groups by key and applies the reduce function
- **File I/O** — reads line-by-line input, writes tab-separated output

## Getting started

### Requirements
- GCC
- pthreads (included on Linux/macOS, use MinGW on Windows)
- make

### Build

```bash
make
```

### Dataset

Download Moby Dick from Project Gutenberg:

```bash
mkdir data
curl -o data/moby_dick.txt https://www.gutenberg.org/files/2701/2701-0.txt
```

On Windows (PowerShell):

```powershell
mkdir data
Invoke-WebRequest -Uri "https://www.gutenberg.org/files/2701/2701-0.txt" -OutFile "data/moby_dick.txt"
```

### Run

```bash
./wordcount
```
