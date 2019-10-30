# Thread or Trick

## Introduction

Tonight is Halloween, children are excited about tonight's party: customs, candies and, if there is no other choice, settle accounts with those wicked who don't have enough sweet supplies. And that is that year after year the thing has got worse: more little monsters dressed as children, less candies.

You've been choosen to try to prevent the dissaster and you've been hired to run a simulation of what can happen and be prepared.

## Problem

Develop an application that can simulate the party and obtain statistics about candy flow and trick count.

To do so, you must take into consideration the following parameters:

- There are ```N``` children in the neighborhood.
- There are ```M``` houses in the neighborhood.
- ```2M <= N```.
- No house will have more than ```C=10``` candies at any moment.
- Each house will start with maximum candy supplies.
- When a child knocks on a door it will ask for "Treat or trick". If there are candies at the house, will be treat (decreasing house's stack and increasing child's count). Otherwise trick.
- Trick duration is ```50ms```, while treat is ```10ms```. Moving from a house to the next one takes ```20ms```.
- Each child may start at any house of the neighborhood.
- There is a candy deliveryman that is constantly given a few candies (```D=10```) to each house if it is running below a certain threshold ```T=3```.
- Delivery is done in ```5ms``` and it takes ```15ms``` to reach the next house.
- The deliveryman always start at house 0.
- While the owner of a house is busy attending a child no other child or the deliveryman can access the candy storage. It includes the case of trick. The same happens while the deliveryman is giving candies: no child can access the house.
- Both children and deliveryman travel houses in the same incresing order. If last house is reached, next house is 0.
- Both children and deliveryman will be traveling houses over and over until either a child reaches 100 candies or 100 tricks done. Candies and tricks are checked before moving to the next house.
- At the end, overall statistics must be printed:
  - Total number of candies received.
  - Average number of candies received per child.
  - Total number of tricks done.
  - Average number of tricks done per child.
  - Total number of candies remaining at houses.

## Delivery and Grading

Solution proposed shall be developed using C++11 or higher, all into a single ```.cpp``` file (no compressed solutions, no executables, etc).

Source code must be sent to Professor's email before October 31st, 2019, 23:59:59. Only the last email within the valid period will be corrected, but all deliveries after deadline will just be ignored.

This practice aims for 5% of the course (maximum score is then 0.5).

Grading rules are as follows:
- No code sent: 0 points. Code sent after deadline will be ignored and doesn't count at all.
- Code doesn't compile: 0 points.
- No statistics: -0.1 points.
- Race conditions are not handled correctly: -0.25 points.
- No multi-threading at all: -0.45 points.
