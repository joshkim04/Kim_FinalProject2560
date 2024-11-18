#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

/*
Index values for "Row"
Index 0: Tournament name 
Index 1: Stage 
Index 2: Match type 
Index 3: Player name
Index 4: Team
Index 5: Agents
Index 6: Rounds played
Index 7: Rating
Index 8: ACS 
Index 9: Kills:Deaths
Index 10: Kill, Assist, Trade, Survive %
Index 11: Average Damage Per Round
Index 12: Kills Per Round
Index 13: Assists Per Round
Index 14: First Kills Per Round
Index 15: First Deaths Per Round
Index 16: Headshot %
Index 17: Clutch Success %
Index 18: Clutches (won/played)
Index 19: Maximum Kills in a Single Map
Index 20: Kills
Index 21: Deaths
Index 22: Assists
Index 23: First Kills
Index 24: First Deaths

structure type
Top level: Player name, tournaments played, list of teams played for
Second Level: tournament/match types played (find all match type names and find the statline with the most rounds played )
Third level: stats for each match type 

Make another level of data intake for each map?

Player name, tournaments played, teams played for 
Select a tournament you'd like to look at 
Select a match to look at 

All Match Types = "Overall stats" or "Tournament stats"

top level
tournament [lcq, reg season]
second level
lcq = [lower final, upper final]

ex:
vector<master_stat> player;
To find tournament: player.tournaments (vector of [tournament name, vector<stages>]) 


*/

//Structure to store player stats, structure was used instead of vector to make indivisual stats earsier to find
struct stats {
    vector<string> agents;
    int rounds;
    float rating;
    float acs;
    float kd;
    float kats;
    int adr;
    float kpr;
    float apr;
    float fkpr;
    float fdpr;
    float hs;
    float cs;
    float clutches; //maybe change this to string?
    int maxk;
    int kills;
    int deaths;
    int assists;
    int fk;
    int fd;
};

struct mtypes {
    //Name of the match
    string mname;
    stats stats;
};

struct stages {
    //Name of the stage
    string sname;
    vector<mtypes> mtypes;
};

struct tournaments {
    //Name of the tournament 
    string tname;
    vector<stages> stages;
};

struct master_stat {
    //Name of the player 
    string name;
    //Vector of the tournament elements 
    vector<tournaments> tournaments; // Top level of tournaments names, second level of stages within the tournament, third level of match types
    //Vector of teams this player has played for
    vector<string> teams;
};

// Function for file intake, takes data and stores them in a vector "data" where each element is another vector whose elements are each column of the data
int file_intake(vector<vector<string> >& data) {
    ifstream file("players_stats.csv");

    // Check if the file is open
    if (!file.is_open()) {
        cerr << "Could not open the file!" << endl;
        return 1;
    }

    string line;
    // Read each line in the file
    while (getline(file, line)) {
        vector<string> row;
        stringstream lineStream(line);
        string cell;

        // Split the line by commas
        while (getline(lineStream, cell, ',')) {
            row.push_back(cell);
        }
        
        int x = 0;
        while (int(row.size()) > (6 + x) && !isdigit(row[6 + x][0]))
        {
            row[5] += "," + (row[6 + x]);
            x += 1;
        }
        row.erase(row.begin() + 5, row.begin() + 5 + x);         
        // Store the row in the data vector
        data.push_back(row);
    }
    // Close the file
    file.close();
    data.erase(data.begin());
    return 0;
}

bool name_comp(const vector<string>& n1, const vector<string>& n2) {
    string s1 = n1[3];
    string s2 = n2[3];
    for(int x = 0; x <= int (s1.size()); x++)
    {
        s1[x] = toupper(s1[x]);
    }
    for(int x = 0; x <= int (s2.size()); x++)
    {
        s2[x] = toupper(s2[x]);
    }
    return s1 < s2;
}


stats assign_stats(vector<vector<string> >& data, int pos) {
    //Variable for player stats
    stats pstats;
    pstats.agents.push_back(data[pos][5]);
    pstats.agents.push_back(data[pos][5]);
    pstats.rounds = stoi(data[pos][6]);
    pstats.rating = stof(data[pos][7]);
    pstats.acs = stof(data[pos][8]);
    pstats.kd = stof(data[pos][9]);
    pstats.kats = stof(data[pos][10]);
    pstats.adr = stoi(data[pos][11]);
    pstats.kpr = stof(data[pos][12]);
    pstats.apr = stof(data[pos][13]);
    pstats.fkpr = stof(data[pos][14]);
    pstats.fdpr = stof(data[pos][15]);
    pstats.hs = stof(data[pos][16]);
    pstats.cs = stof(data[pos][17]);
    pstats.clutches = stof(data[pos][18]);
    pstats.maxk = stoi(data[pos][19]);
    pstats.kills = stoi(data[pos][20]);
    pstats.deaths = stoi(data[pos][21]);
    pstats.assists = stoi(data[pos][22]);    
    pstats.fk = stoi(data[pos][23]);
    pstats.fd = stoi(data[pos][24]);
    return pstats;

}


mtypes assign_match(vector<vector<string> >& data, int pos){
    mtypes match;
    match.mname = data[pos][2];
    match.stats = assign_stats(data, pos);
    return match;
}

stages assign_stage(vector<vector<string> >& data, int pos){
    stages stage;
    stage.sname = data[pos][1];
    stage.mtypes.push_back(assign_match(data,pos));
    return stage;
}

tournaments assign_tournament(vector<vector<string> >& data, int pos){
    tournaments tournament;
    tournament.tname = data[pos][0];
    tournament.stages.push_back(assign_stage(data, pos));
    return tournament;
}



void data_trim(vector<vector<string> >& data, vector<master_stat>& players) {
    //Sort dataset via player names (already done in the function)
    sort(data.begin(), data.end(), name_comp);

    //Start processing the data for each player
    string name = data[0][3];  //Get the name of the first player
    int pcount = 0; //Start index for players vector

    master_stat temp;
    temp.name = name;
    players.push_back(temp); 


    for (int x = 0; x < int (data.size()); x++) {
        //If this is a new player, add them to the players vector
        if (data[x][3] != name) {
            pcount += 1;
            name = data[x][3];
            master_stat temp;
            temp.name = name;
            players.push_back(temp); //Add new player to players vector
        }


        //Access the current player's struct in the vector
        master_stat& player = players[pcount];
        //Section to assign tournament data to a player 
        //Iterate through all tournaments under this player 

        bool torn1 = false; 
        for (int y = 0; y < int (player.tournaments.size()); y++) {
            //Check if the player has an entry for this tournament
            if (player.tournaments[y].tname == data[x][0]) {
                torn1 = true;
                bool stage1 = false;
                //Iterate through all known stages of the tournament 
                for (int z = 0; z < int (player.tournaments[y].stages.size()); z++) {
                    //Check if the stage is known for the tournament  
                    if (player.tournaments[y].stages[z].sname == data[x][1]) {
                        stage1 = true;
                        bool match1 = false;
                        //Iterate through all matches of this stage
                        for (int a = 0; a <= int (player.tournaments[y].stages[z].mtypes.size()); a++)
                        {
                            //Check if match is known for this stage
                            if (player.tournaments[y].stages[z].mtypes[a].mname == data[x][2])
                            {
                                match1 = true;

                                //Check if the new match type has less rounds played(aka means that it only covers one map)
                                if (player.tournaments[y].stages[z].mtypes[a].stats.rounds < stoi(data[x][6]))
                                {
                                    player.tournaments[y].stages[z].mtypes[a].stats = assign_stats(data, x);
                                }
                            } 
                        }
                        if(!match1)
                        {
                            player.tournaments[y].stages[z].mtypes.push_back(assign_match(data, x));
                        }
                    }
                }
                if(!stage1)
                {
                    player.tournaments[y].stages.push_back(assign_stage(data,x));
                }
            }
        }
        if(!torn1)
        {
            player.tournaments.push_back(assign_tournament(data,x));
        }
    }
}

void list_torns(vector<master_stat> players)
{
    cout << "Tournaments played is: ";
    for(int a = 0; a < int (players[0].tournaments.size()); a++)
    {
        cout << players[0].tournaments[a].tname << "[" << a << "]" << ", ";
    }
    cout << endl;
}

string ask_name()
{
    string name;
    cout << "What letter does the players name start with? \n";
    cin >> name;
    if(name.length() != 1) 
    {
        cout << "Please enter a single letter or number\n";
        return ask_name();
    } else {
        return name;
    }
}


int num_check(int range)
{
    cout << "Please select the number you would like to search: " ;
    int pos;
    cin >> pos;
    if(cin.fail()) 
    {
        cin.clear();
        cin.ignore(256,'\n');
        cout << "That number is not an option please try agian" << endl;
        return num_check(range);
    } else if (pos > 0 && pos <= range) {
        return pos - 1;
    } else {
        cout << "That number is not an option please try agian" << endl;
        return num_check(range);
    }
}


string find_player(vector<master_stat> players)
{
    string name = ask_name();
    cout << name << endl;
    vector<string> names;
    for(int x = 0; x < players.size(); x++)
    {
        if(toupper((players[x].name)[0]) == toupper(name[0]))
        {
            names.push_back(players[x].name);            
        }
    }

    cout << "Searchable players are: \n";
    for(int x = 0; x < names.size(); x++)
    {
        cout << names[x] << " [" << x + 1 << "]\n";
    }
    //int pos = psearch(names);
    int pos = num_check(names.size());
    return names[pos];
}





//Complete function to serach for players
void search()
{

};

int main() {
    // Raw file intake data
    vector<vector<string> > data;

    // Vector with a data structure of organized player stats 
    vector<master_stat> players;

    // File intake
    file_intake(data);
    cout << "File intake complete" << endl;
    data_trim(data, players);
    players.erase(players.begin());


    string name = find_player(players);
    cout << "The player you picked is " << name << endl;
    
    













    /*
    for(int a = 0; a < int (players.size()); a++)
    {
        cout << "Player name is: " << players[a].name  << endl;
    }
    
    
    cout << players[0].name << endl;


    list_torns(players);
    for(int a = 0; a < int (players[0].tournaments[2].stages[1].mtypes.size()); a++)
    {
        cout << "Match name: " << players[0].tournaments[2].stages[1].mtypes[a].mname << endl;
    }
    cout << players[0].tournaments[2].stages[1].mtypes[0].mname << endl;
    cout << players[0].tournaments[2].stages[1].mtypes[0].stats.rounds << endl;

    // Test code to print every line of "Data"
    
    for (const auto& row : data) {
        for (const auto& cell : row) {
            cout << cell << " ";
        }
        cout << endl;
    }
    */
    

    // Print the data
    return 0;
}
