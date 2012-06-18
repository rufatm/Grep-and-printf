To compile the program:
*run make 
*make will copmile two program: main.c for the first part(finds) and
 my_printf.c for the second part. Two files will be produced: finds and 
 my_printf. Program finds takes several command-line arguments as follows: 
 finds -s pathname [-f c|h|S] [-l] -s s 
 Program my_printf does not take any arguments.I have a main function in
 there with one test case. More test cases can be added 

* Parts completed: Both parts are completed fully. I did not do the bonus
  part (adding brackets to the regular expressions). 
  However my engine takes more than one occurence of each control character,
  i.e it is ok to have something like He*ll*o or He?ll?o

* Note: for symbolic link I did not want to have a list of all inodes I've
  ever visited so I implemented way to store less inode numbers at a time.
  Instead Im storing the entry points meaning that if a particular inode is
  stored I have not only visited it already but I visited all of its
  children in the tree
  




 

