# krssgtask
#FIRST TASK
Implemented finite state machines for the lift encounter problem. Came around various papers for finite states in which they were using inheritance etc. I couldn't understand much of it to implement using inheritance so I tried writing it from scratch.
Tried breaking down into more states as asked by Snehal but with limited time and seg faults, I reverted back to previous version. I have optimised the solution by checking few conditions before starting the lift, which can be understood after having a look at the code.

#SECOND TASK
I researched through papers of rrt and how is rrt different from rrt*. Then I implemented rrt* using a code for rrt which I came across while searching. I made changes such as making the program based on number of iterations instead of finding the goal and    stopping thereby.
Then after going through ROS tutorials, I started with turtlesim. 
I used data file management and extracted few points from the rrt* program and then made the turtle move through those points.
Approach for movement of turtle: first make in aligned by rotating, then gave it velocity to reach the target. Used P of PID for giving appropriate velocity/angular velocity after calculating error in current state and desired state.

#THIRD TASK
Made a change in the strategy.cc file.  

