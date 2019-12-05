Robertson, Neil, Daniel Sanders, Paul Seymour, and Robin Thomas. 1996. “A New Proof of the Four-Colour Theorem.” Electronic Research Announcements of the American Mathematical Society 2 (1): 17–25. https://doi.org/10.1090/S1079-6762-96-00003-0.

Robertson, Neil, Daniel Sanders, Paul Seymour, and Robin Thomas. 1997. “The Four-Colour Theorem.” Journal of Combinatorial Theory, Series B 70 (1): 2–44. https://doi.org/10.1006/jctb.1997.1750.

Robertson, Neil, Daniel P. Sanders, Paul Seymour, and Robin Thomas. 1997. “Discharging Cartwheels.” http://arxiv.org/abs/1401.6485.

    There are 9 ancillary files associated with this article. 

    discharge.c (54.3kB)
    discharge.pas (45.5kB)
    present10 (34.3kB)
    present11 (10.9kB)
    present7 (195.1kB)
    present8 (172.5kB)
    present9 (115.7kB)
    rules (2.6kB)
    unavoidable.conf (398.2kB)

Robertson, Neil, Daniel P. Sanders, Paul Seymour, and Robin Thomas. 1997. “Reducibility in the Four-Color Theorem.” http://arxiv.org/abs/1401.6481.

    There are 2 ancillary files associated with this article. 

    reduce.c (29.5kB)
    unavoidable.conf (398.2kB)

================================================================================

Steinberger, John. 2010. “An Unavoidable Set of 𝐷-Reducible Configurations.” Transactions of the American Mathematical Society 362 (12): 6633–61. https://doi.org/10.1090/S0002-9947-2010-05092-5.

The following files accompany the paper "An unavoidable set of D-recudible configurations"
by John Steinberger:

U_2822.conf (list of configurations, computer readable)
U_2822.pdf (list of configurations, graphical)
L_42 (discharging rules)
reduce.c (reducibility program by Robertson, Sanders, Seymour and Thomas)
discharge.c (proof checking program by Robertson et al.)
p5_2822 (proof script degree 5)
p6_2822 (proof script degree 6)
p7_2822 (proof script degree 7)
p8_2822 (proof script degree 8)
p9_2822 (proof script degree 9)
p10_2822 (proof script degree 10)
p11_2822 (proof script degree 11)
reduce.pdf (documentation for reduce.c, written by Robertson et al.)
discharge.pdf (documentation for discharge.c, written by Robertson et al.)

The programs reduce.c and discharge.c contain slight modifications from their original
versions. The changes are documented within the code and marked by a 'jps' comment line. 
The original versions of these programs are hosted by Robin Thomas at www.math.gatech.edu/~thomas.

The files U_2822.conf and U_2822.pdf contain the unavoidable set of D-reducible 
configurations in computer-readable and human-readable form. The format of U_2822.conf 
is as follows: each configuration starts with an identifier (number). The next line lists 
the number of vertices in the free completion, the ring-size, the number of colorings 
which extend to the configuration, and the size of the largest consistent set in the 
complement of the set of colorings that extend to the configuration (in our case, 
always 0). The next line lists the contract (reducer), if any (in our case always empty). 
Then the vertex adjacency lists are given, starting with ring vertices. The remaining 
lines specify coordinates for the vertices, left zero in our file. The files U_2822.conf, 
U_2822.pdf list the configurations in the same order, lexicographically by degree sequence. 
The files p5_2822, ..., p11_2822 are the presentation files giving the proof-of-correctness 
of the discharging rules. The discharging rules themselves are given in L_42. Use

p7_2822 U_2822.conf L_42 0 1 

as typical arguments to discharge.c, and

U_2822.conf

as argument to reduce.c.

April 2009
