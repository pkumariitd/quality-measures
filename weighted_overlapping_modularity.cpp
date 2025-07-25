#include <iostream>
#include <algorithm>
#include <fstream>
#include <set>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <string>
#include <iterator>
#include <stdio.h>
#include <list>
using namespace std;

class Graph
    {
    public:
        set<int> V; //only vertices with positive degree are stored
        map<int, float> strength;
        map<int, map<int, float> > weight;
        void read_edgelist(string&, bool);
        float get_weight(int, int);
        friend float overlapping_weighted_modularity(Graph&, map<int, set<int> >&, int, map<int, set<int> >& , int &);
    };

void Graph::read_edgelist(string& edgefile, bool weighted)
{
    ifstream fin;
    fin.open(edgefile);
    if(!fin.is_open())
    {
        cout<<"The file containing the edgelist could not be opened."<<endl;
        exit(1);
    }
    string line;
    while ( getline(fin, line ) )
    {
        istringstream is( line );
        int u, v;
        is>>u;
        is>>v;
        if(u == v)
            continue;
        float w;
        if(weighted == true)
        {
            if(is.str() == " " || is.eof())
            {   cout<<endl<<"The edge list has missing weights."<<endl;
                exit(1);
            }
            is>>w;
        }
        else
            w = 1;
        if(weight[u].find(v) != weight[u].end())
        {
            weight[u][v] = weight[u][v]+w;
            weight[v][u] = weight[v][u]+w;
        }
        else
        {
            weight[u][v] = w;
            weight[v][u] = w;
        }
        if(strength.find(u) == strength.end())
            strength[u] = w;
        else
            strength[u] = strength[u]+w;
        if(strength.find(v) == strength.end())
           strength[v] = w;
        else
           strength[v] = strength[v]+w;
        V.insert(u);
        V.insert(v);
    }
}
float Graph::get_weight(int u, int v)
{
    if(weight[u].find(v) != weight[u].end())
        return weight[u][v];
    else
        return 0;
}

void read_communities(string& comfile, map<int, set<int> >&, map<int, set<int> >&);
float overlapping_weighted_modularity(Graph&, map<int, set<int> >&, map<int, set<int> >&, int & );
void usage();

int main(int argc, char* argv[])
{
    string network_file, community_file;
    bool weighted = false;
    if(argc < 3 || argc > 4)
        usage();
    else
    {
        network_file = string(argv[1]);
        community_file = string(argv[2]);
    }
    if(argc == 4)
    {
        string arg = string(argv[3]);
        if(arg == "-w")
            weighted = true;
        else
            usage();
    }
    Graph g;
    map<int, set<int> > coms;
    g.read_edgelist(network_file, weighted);
    map<int, set<int> > memberships;
    read_communities(community_file, coms, memberships);
    int ghost_communities = 0;
    float Q_wo = overlapping_weighted_modularity(g, coms, memberships, ghost_communities);

    cout<<"-------------------------------------------------------------"<<endl;
    cout.setf(ios::left, ios::adjustfield);
    cout<<setw(25)<<"actual communities"<<"= "<<setw(20)<<coms.size()<<endl;
    cout<<setw(25)<<"ghost communities"<<"= "<<setw(20)<<ghost_communities<<endl;
    cout<<setw(25)<<"Qwo"<<"= "<<setw(20)<<Q_wo<<endl;
    cout<<"--------------------------------------------------------------"<<endl;
    return 0;
}

void read_communities(string& comfile, map<int, set<int> >& coms, map<int, set<int> >& memberships)
{
    std::ifstream fin(comfile);
    string line;
    if(!fin.is_open())
    {   cout<<"Community file could not be opened."<<endl;
        exit(1);
    }
    int i = 1;
    while ( std::getline(fin, line ) )
    {
        istringstream is( line );
        int v;
        while(is>>v)
        {
            coms[i].insert(v);
            memberships[v].insert(i);
        }
        i++;
   }
}


float overlapping_weighted_modularity(Graph& g, map<int, set<int> >& coms, map<int, set<int> >& memberships, int &ghost_communities)
{
    map<int, set<int> >::iterator ci;
    set<int>::iterator si;
    float indeg, deg, ind_frac;
    float Q_wo = 0;

    // checking ghost communities, if present.
    for(ci = coms.begin(); ci != coms.end(); )
    {
        bool flag = false;
        for(auto p = ci->second.begin(); p != ci->second.end(); ++p)
            if(g.V.find(*p) == g.V.end())
            {
                ci = coms.erase(ci);
                flag = true;
                ghost_communities++;
                break;
            }
        if(flag == false)
            ++ci;
    }

    for(ci = coms.begin(); ci != coms.end(); ++ci)
    {
        int total_memberships = 0;
        indeg = 0;
        deg = 0;
        for(si = ci->second.begin(); si != ci->second.end(); ++si)
        {
            total_memberships += memberships[*si].size();
            map<int, float>::iterator mi;
            for(mi = g.weight[*si].begin(); mi != g.weight[*si].end(); ++mi)
            {
                deg += mi->second;
                if(ci->second.find(mi->first) != ci->second.end())
                    indeg += mi->second;
            }
        }
        Q_wo += (ci->second.size()*coms.size() - total_memberships)*indeg/(ci->second.size()*deg*coms.size());
    }
    return Q_wo/coms.size();
}
void usage()
{
    cout<<"Please, follow the syntax as given below:"<<endl;
    cout<<"-----------------------------------------------------------"<<endl;
    cout<<"./Qwo network_file community_file -w -si"<<endl;
    cout<<"-----------------------------------------------------------"<<endl;
    cout<<"Here, network_file is the file holding the network in edgelist form."<<endl;
    cout<<"and community_file is the file holding the communities, each written on a separate line."<<endl;
    cout<<"Use flag w if your network is weighted."<<endl;
    cout<<"Use flag si if you do not want other than required information."<<endl;
    exit(1);
}

