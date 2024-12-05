#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>

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
    float clutches; 
    int maxk;
    int kills;
    int deaths;
    int assists;
    int fk;
    int fd;
    float jrate;
};

//Structure to store data about a specific match
struct mtypes {
    //Name of the match
    string mname;
    stats stats;
};

//Structure to store data about a specific tournament satge
struct stages {
    //Name of the stage
    string sname;
    vector<mtypes> mtypes;
};

//Definition in advance
struct master_stat;

//Structure to store data about a specific tournament
struct tournaments {
    //Name of the tournament 
    string tname;
    vector<stages> stages;
    //Vector to list top players from the tournamnet
    vector<master_stat> top_stats;

    //Operator overloading for this structure
    bool operator==(tournaments const& x1)
    {
        return tname == x1.tname;
    }
};

//Top level structure to store player data
struct master_stat {
    //Name of the player 
    string name;
    //Vector of tournaments the player has played in  
    vector<tournaments> tournaments; 
    //Vector of teams this player has played for
    vector<string> teams;
    stats stats; //Used to store stats for a specific tournament
};

// Function for file intake, takes data and stores them in a vector "data" where each element is another vector whose elements are each column of the data
int file_intake(vector<vector<string> >& data) {
    //Opens file
    ifstream file("players_stats.csv");

    //Check if the file was succesfully opened
    if (!file.is_open()) {
        cerr << "Could not open the file!" << endl;
        return 1;
    }

    string line;
    //Read each line in the file 
    while (getline(file, line)) {
        vector<string> row;
        stringstream lineStream(line);
        string cell;

        //Split the line by commas and move each element into the vector
        while (getline(lineStream, cell, ',')) {
            row.push_back(cell);
        }


        int x = 0;
        //Move agent names back into a single slot
        while (int(row.size()) > (6 + x) && !isdigit(row[6 + x][0]))
        {
            row[5] = row[5] + row[6 + x];
            x += 1;
        }
        row.erase(row.begin() + 6, row.begin() + 6 + x);
        row[5].erase(row[5].begin());
        if (!row[5].empty())
            row[5].pop_back();

        data.push_back(row);
    }
    // Close the file
    file.close();
    //Erase the headings
    data.erase(data.begin());
    cout << "File intake complete" << endl;
    return 0;
}

//Boolean functions for alphabetically organizing names
bool name_comp(const vector<string>& n1, const vector<string>& n2) {
    string s1 = n1[3];
    string s2 = n2[3];
    //Uppercase all letters to avoid case errors
    transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

    return s1 < s2;
}



//Assigns stats from raw data into appropriate struct
stats assign_stats(vector<vector<string> >& data, int pos) {
    //Variable for player stats
    stats pstats;
    string temp;
    //Add commas back into agent slot for readability
    for (char x : data[pos][5])
    {
        if (x == ' ')
        {
            pstats.agents.push_back(temp);
            temp = "";
        }
        else {
            temp += x; 
        }
    }
    pstats.agents.push_back(temp);
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
    
    //Custom rating system made for this project
    pstats.jrate = (pstats.kats / 100) * ((pstats.kpr + pstats.apr) / 2) * ((pstats.acs / 100) + (pstats.adr / 100) + pstats.fkpr - pstats.fdpr);
    //Conversions done in order to round the calculated value down to 2 decimal places
    temp = to_string(pstats.jrate).substr(0,4);
    int t = round(stof(temp) * 100);
    pstats.jrate = t;
    pstats.jrate = pstats.jrate / 100;
    return pstats;

}

//Assign match name and stats
mtypes assign_match(vector<vector<string> >& data, int pos) {
    mtypes match;
    match.mname = data[pos][2];
    match.stats = assign_stats(data, pos);
    return match;
}

//Assign stage names and matches
stages assign_stage(vector<vector<string> >& data, int pos) {
    stages stage;
    stage.sname = data[pos][1];
    stage.mtypes.push_back(assign_match(data, pos));
    return stage;
}

//Assign tournament name and stages
tournaments assign_tournament(vector<vector<string> >& data, int pos) {
    tournaments tournament;
    tournament.tname = data[pos][0];
    tournament.stages.push_back(assign_stage(data, pos));
    return tournament;
}

//Boolean function to compare tournament stats
bool stat_comp(master_stat x1, master_stat x2)
{
    return x1.stats.jrate > x2.stats.jrate;
}


//Function to trim the data 
void data_trim(vector<vector<string> >& data, vector<master_stat>& players, vector<tournaments>& tournies) {
    //Sort dataset alphabetically via player names 
    sort(data.begin(), data.end(), name_comp);

    //Start processing the data for each player
    string name = data[0][3];  //Get the name of the first player
    int pcount = 0; //Start index for players vector

    master_stat temp;
    temp.name = name;
    //Add the first player to the vector
    players.push_back(temp);


    //Iterate through all lines of data
    for (int x = 0; x < int(data.size()); x++) {
        //If this is a new player, add them to the players vector
        if (data[x][3] != name) {
            master_stat temp;
            temp.name = data[x][3];
            name = data[x][3];
            players.push_back(temp); //Add new player to players vector
            //Iterate through all tournaments a player has played
            for (tournaments& x : players[pcount].tournaments)
            {
                //Check location of the tournament within the tournament vector
                for (tournaments& y : tournies)
                {
                    if (x.tname == y.tname)
                    {
                        //Add players tournament stats to the list
                        master_stat p;
                        p.name = players[pcount].name;
                        p.stats = x.stages.back().mtypes.back().stats;
                        y.top_stats.push_back(p);
                    }
                }
            }
            //Increase player count
            pcount += 1;
        }
        //Access the current player's struct in the vector
        master_stat& player = players[pcount];
        //Section to assign tournament data to a player 
        //Iterate through all tournaments under this player 
        bool torn1 = false;
        for (int y = 0; y < int(player.tournaments.size()); y++) {
            //Check if the player has an entry for this tournament
            if (player.tournaments[y].tname == data[x][0]) {
                torn1 = true;
                bool stage1 = false;
                //Iterate through all known stages of the tournament 
                for (int z = 0; z < int(player.tournaments[y].stages.size()); z++) {
                    //Check if the stage is known for the tournament  
                    if (player.tournaments[y].stages[z].sname == data[x][1]) {
                        stage1 = true;
                        bool match1 = false;
                        //Iterate through all matches of this stage
                        for (int a = 0; a < int(player.tournaments[y].stages[z].mtypes.size()); a++)
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
                        //If the match does not exist for the stage add it 
                        if (!match1)
                        {
                            player.tournaments[y].stages[z].mtypes.push_back(assign_match(data, x));
                        }
                    }
                }
                //If the stage does not exist for the tournament add it 
                if (!stage1)
                {
                    player.tournaments[y].stages.push_back(assign_stage(data, x));
                }
            }
        }
        //If the tournament does not exist for the player create it 
        if (!torn1)
        {
            tournaments temp = assign_tournament(data, x);
            player.tournaments.push_back(temp);
            //Check if the tournmanet exists in the master tournament list and add it if it does not
            auto pos = (find(tournies.begin(), tournies.end(), temp));
            if (pos == tournies.end())
            {
                tournies.push_back(temp);
            }
        }
    }
    

     
}


//Function to print out every element of a list of strings with their respective positions. 
void print_things(vector<string> list)
{
    for (int a = 0; a < int(list.size()); a++)
    {
        cout << list[a] << "[" << a << "]\n";
    }
}

//Prints stats
void print_stats(stats stat)
{
    cout << "Their agents played are: ";
    for (int a = 0; a < int(stat.agents.size()); a++)
    {
        cout << stat.agents[a] ;
        if (a != stat.agents.size() - 1)
        {
            cout << ", ";
        }
    }
    cout << "\nThe number of rounds played is " << stat.rounds;
    cout << "\nTheir vlr rating is " << stat.rating;
    cout << "\nTheir acs is " << stat.acs;
    cout << "\nTheir kd is " << stat.kd;
    cout << "\nTheir kats is " << stat.kats;
    cout << "\nTheir adr is " << stat.adr;
    cout << "\nTheir kpr is " << stat.kpr;
    cout << "\nTheir apr is " << stat.apr;
    cout << "\nTheir fkpr is " << stat.fkpr;
    cout << "\nTheir fdpr is " << stat.fdpr;
    cout << "\nTheir hs% is " << stat.hs;
    cout << "\nTheir combat score is " << stat.cs;
    cout << "\nTheir clutch% is " << stat.clutches;
    cout << "\nTheir max kills in a game is " << stat.maxk;
    cout << "\nTheir total kills are " << stat.kills;
    cout << "\nTheir total deaths are " << stat.deaths;
    cout << "\nTheir total assists is " << stat.assists;
    cout << "\nTheir total first kills is " << stat.fk;
    cout << "\nTheir total first deaths is " << stat.fd << endl;
    cout << "\nTheir rating 2.0 is " << stat.jrate << endl;
}



//Ask the user for a number and check that the number is valid and within the given range
//A function because it makes checking a valid input easier via recursion
int num_check(int range)
{
    cout << "Please select the number you would like to search: ";
    int pos;
    cin >> pos;
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(256, '\n');
        cout << "That number is not an option please try agian" << endl;
        //Recursivly run until a valid number is inputted by the user
        return num_check(range);
    }
    else if (pos > 0 && pos <= range) {
        return pos - 1;
    }
    else {
        cout << "That number is not an option please try agian" << endl;
        return num_check(range);
    }
}

//Find the tournament the user wants to view. 
tournaments find_tournament(master_stat player)
{
    //Check if the player has only played in one tournament
    if (player.tournaments.size() == 1)
    {
        //Automatically select the option if the player has only played one tournament
        cout << "This player has only played in: " << player.tournaments[0].tname << endl;
        return player.tournaments[0];
    }
    //List the avaliable tournaments to view stats for
    cout << "The tournaments this player has played: \n";
    for (int a = 0; a < int(player.tournaments.size()); a++)
    {
        cout << player.tournaments[a].tname << " [" << a + 1 << "]\n";
    }
    cout << endl;
    //Ask for user selection
    int pos = num_check(player.tournaments.size());
    return player.tournaments[pos];
}

//Find the stage the user wants to view 
stages find_stage(tournaments tourn)
{
    //Check if the player has only played in one stage
    if (tourn.stages.size() == 1)
    {
        //Automatically select the option if the player has only played one stage
        cout << "The only stage of this tournament is: " << tourn.stages[0].sname << endl;
        return tourn.stages[0];
    }
    //List the avaliable stages 
    cout << "The stages of this tournament are: \n";
    for (int a = 0; a < int((tourn.stages.size())); a++)
    {
        cout << tourn.stages[a].sname << " [" << a + 1 << "]\n";
    }
    cout << endl;
    //Ask for which number the user selected
    int pos = num_check(tourn.stages.size());
    return tourn.stages[pos];
}

//Find the match the user wants to view 
mtypes find_match(stages stage)
{
    //Check if the player has only played in one match
    if (stage.mtypes.size() == 1)
    {
        //Automatically select the option if the player has only played one match
        cout << "The only match of this stage is: " << stage.mtypes[0].mname << endl;
        return stage.mtypes[0];
    }
    //List the avaliable matches 
    cout << "The matches of this tournament are: \n";
    for (int a = 0; a < int((stage.mtypes.size())); a++)
    {
        cout << stage.mtypes[a].mname << " [" << a + 1 << "]\n";
    }
    cout << endl;
    //Ask for which number the user selected
    int pos = num_check(stage.mtypes.size());
    return stage.mtypes[pos];
}


//Function to ask the user for the first letter of a player name and do the necessary edge case checks
string ask_name()
{
    string name;
    //Ask the user for the first character of the players name
    cout << "What character does the players name start with? \n";
    cin >> name;
    //Make sure only one character is inputted
    if (name.length() != 1)
    {
        cout << "Please enter a single letter or number\n";
        return ask_name();
    }
    else {
        return name;
    }
}



//Find a specific player based on user input 
//Input: Vector of all player data
//Output: struct "Master_stat" for a specific player
master_stat find_player(vector<master_stat> players)
{
    //Ask for the first letter of the players name
    string name = ask_name();
    vector<string> names;
    vector<int> nums;
    //Check for all player names that start with that character
    for (int x = 0; x < players.size(); x++)
    {
        if (toupper((players[x].name)[0]) == toupper(name[0]))
        {
            names.push_back(players[x].name);
            nums.push_back(x);
        }
    }

    //Checks that there are players whose name starts with this character
    if (names.size() == 0)
    {
        cout << "There are no players whose name starts with the character, please try again" << endl;
        return find_player(players);
    }
    
    //Prints the names of the players that are searchable
    if (names.size() == 0)
    {
        cout << "There are no players whose name starts with that chracter, please try again" << endl;
        return find_player(players);
    }
    cout << "Searchable players are: \n";
    for (int x = 0; x < names.size(); x++)
    {
        cout << names[x] << " [" << x + 1 << "]\n";
    }
    //Calls a function to ask the user which number player they want to select
    int pos = num_check(names.size());
    //Returns the master_stat struct with all the players info 
    return players[nums[pos]];
}



//Function to search for player stats
void search_player(vector<master_stat> players)
{
    //Ask user which player they'd like to look for
    master_stat player = find_player(players);
    //Ask what tournoment they want to look at
    tournaments tourn =  find_tournament(player);
    //Ask which stage to look at
    stages stage = find_stage(tourn);
    //Ask match name or if they want to see tournoment stats 
    mtypes match = find_match(stage);
    //Display stats for match or display tournoment stats 
    print_stats(match.stats);
    
};

//Function to search for top players of a specific tournament
void search_tournoment(vector<tournaments> tournies)
{
    //Print all serachable tournaments
    cout << "Searchable tournoments are: \n";
    for (int x = 0; x < tournies.size(); x++)
    {
        cout << tournies[x].tname << " [" << x + 1 << "]\n";
    }
    //Calls a function to ask the user which tournament player they want to select
    int pos = num_check(tournies.size());
    tournaments selection = tournies[pos];
    //Sort tournament from highest to lowest rating
    sort(selection.top_stats.begin(), selection.top_stats.end(), stat_comp);
    cout << "\nThe best players via rating 2.0 are: \n";
    //Prints ratings from highest to lowest
    for (auto x : selection.top_stats)
    {
        string t = to_string(x.stats.jrate);
        //Removes trailing zeros from the string
        t.erase(4);
        cout << x.name + ": " + t << "\n";
    }
    cout << endl;

};

int main() {
    //Raw file intake data
    vector<vector<string> > data;

    //Vector for player data
    vector<master_stat> players;
    //Vector for tournament data
    vector<tournaments> tournies; 

    //File intake
    file_intake(data);
    //Format data 
    data_trim(data, players, tournies);
    players.erase(players.begin());
    tournies.erase(tournies.begin());
    //Ask if the user would like to search players or tournaments
    cout << "Would you like to look at player stats, [1], or tournament stats [2]? \n" << endl;
    int choice = num_check(2);
    switch (choice)
    {
    case 0:
        search_player(players);
        break;
    case 1:
        search_tournoment(tournies);

    }

    return 0;
}
