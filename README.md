# quality-measures
It contains quality measures for evaluating the strengths of overlapping or disjoint communities in weighted networks
To use the code, first compile it on a linux/unix terminal as follows:

g++ -std=c++11 weighted_overlapping_modularity.cpp -o Qwo

Then an executable code Qwo would be produced. This can be used as follows:

./Qwo network-file community-file -w

Here, network-file is the file containing the network in edgelist format(in two/three columns), where the first two columns indicate the list of edges, and the third one indicates the corresponding weights.
And, community-file is the file containing the community structure of the network, with each line containg a community. 
Note that both the files must be in the same directory where the executable code Qwo lies.
The flag -w is needed only if the network is weighted, that is, if the network-file contains three columns.
If this code is used for any research purpose, the following research article may please be cited:

Kumar, P., & Dohare, R. (2019). A neighborhood proximity based algorithm for overlapping community structure detection in weighted networks. Frontiers of computer science, 13(6), 1353-1355.
