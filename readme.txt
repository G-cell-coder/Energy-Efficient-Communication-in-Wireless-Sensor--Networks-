1) Open the file
2) execute the file using gcc spanning_tree7.c -lm -o span
3) open the child nodes and other neighbour nodes.
4) run the child nodes using ./span B or ./span C respectively for all the child nodes.
5)finally, run the sink node using ./span A
6)request for temp from sink node

Notes:
For above program to get executed and tested - Try running with binary libraries associated with the implementation with (-lm option).
For the record: -l means binary libraries to have in mind when linking. 
Whatever goes after -l is by default interpreted as part of a filename starting by lib and ending in .a. Hence -lm means link libm.a
