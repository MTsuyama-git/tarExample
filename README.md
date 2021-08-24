# tarExample
## tar create example(for hobby use)
### build
```
$ make
```

### examples
Example input files are created automatically by bin/genExample. Here is a list of them.
- sizes: A tree that has 3 kinds of files.
- exceptional: A tree that has a long named node.
They can be generated with this rule:
```
$ make example
```
### output from example
Output of bin/compress and tar cmd from the examples.
- dest/sizes.tarcmd.tar: An archive that has 3 kinds of files. (creted with tar command)
- dest/sizes.compress.tar: An archive that has 3 kinds of files. (creted with bin/compress)
- dest/exceptional.tarcmd.tar: An archive that has a long name node. (creted with tar command)
- dest/exceptional.compress.tar: An archive that has a long name node. (creted with bin/compress)
They can be generated with this rule:
```
$ make compress
```

### Run
Show Information
```
$ ./bin/readbytes input.tar
```
Compress Archive
```
$ ./bin/compress output.tar input
```
Expand Archive
The argument "dest" works like as tar -C /path/to/...
```
$ ./bin/expand input.tar (dest)
```
