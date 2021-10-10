#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Rift

struct PLAYER_NAME : public Player {
  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
  const vector<Dir> dirs = {Up,Down,Left,Right};

  //Will come in handy for the BFS procedure
  struct queueCell {
    Pos ps;
    int dist;
    Dir first_step;
  };

  //For each round, true until proven otherwise
  //If in the current round, a warrior concludes there's no weapon of X type, he will comunicate it to the other warrior through this booleans
  bool guns_available = true;
  bool bazookas_available = true;

  //Number of maxed barricades (maxed barricade = barricade with 320 resistance points)
  int maxed_barricades;

  //Returns true if and only if id corresponds with a citizen from your team
  bool is_ally(const int& id) {
    if (id == -1) {
      return false;
    } 
    else {
      vector<int> w = warriors(me());
      vector<int> b = builders(me());

      int size = w.size();
      //Iterate through all ally warriors
      for(int i = 0; i < size; ++i) {
        if (w[i] == id) return true;
      } 
      
      size = b.size();
      //Iterate through all ally builders
      for(int i = 0; i < size; ++i) {
        if (b[i] == id) return true;
      } 
      return false;
    }
  }

//MOVEMENT BY OBJECTIVES FUNCTION (Money, Food, Weapons...)---------------------------------------------------------------------------

  //Returns distance from origin to the closest cell that accomplishes the aim stated. Returns -1 if there's no way of accomplishing that aim
  //Parameter aim: 1 -> get kills with Bazooka                    (search for cell with weaker enemy)
  //               2 -> get kills with Gun                        (cell with weaker enemy)
  //               3 -> get kills with Hammer                     (cell with weaker enemy)
  //               4 -> find Food                                 (cell with food bonus)
  //               5 -> find Money                                (cell with money bonus)
  //               6 -> find an empty ally Barricade to hide in   (cell with an unoccupied barricade)
  //               7 -> find Bazooka                              (cell with bazooka)
  //               8 -> find Gun                                  (cell with bazooka)
  int bfs_next_step(const Pos& orig, Dir& next_step, int aim) {
    vector< vector<bool> > visited(board_rows(), vector<bool>(board_cols(), false));
    
    //Origin is considered visited
    visited[orig.i][orig.j] = true;

    queue<queueCell> Q;

    //Origin
    queueCell o = {orig, Up};
    Q.push(o);

    int first_moves = 0;

    while (not Q.empty()) {
      queueCell curr = Q.front();
      Pos ps = curr.ps;
      Q.pop();

      if (pos_ok(ps)) {
        switch(aim) {
        case 1: //BAZOOKA KILLS
        {
          int id1 = cell(ps).id;
          if (id1 != -1 and not is_ally(id1) and citizen(id1).weapon != Bazooka) {
            next_step = curr.first_step;
            return curr.dist;
          }
          for (int k = 0; k < 4; ++k) {
            Pos adj = ps + dirs[k];

            if (first_moves < 4) {  //for storing the direction of the initial step
              curr.first_step = dirs[k];
              ++first_moves;
            }
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).resistance < 320 or cell(adj).b_owner == me() or cell(adj).b_owner == -1) and not is_ally(cell(adj).id) and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
        case 2: //GUN KILLS
        {
          int id2 = cell(ps).id;
          if (id2 != -1 and not is_ally(id2) and citizen(id2).weapon != Bazooka and citizen(id2).weapon != Gun) {
            next_step = curr.first_step;
            return curr.dist;
          }
          for (int k = 0; k < 4; ++k) {
            Pos adj = ps + dirs[k];

            if (first_moves < 4) {  //for storing the direction of the initial step
              curr.first_step = dirs[k];
              ++first_moves;
            }
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).b_owner == me() or cell(adj).b_owner == -1) and not is_ally(cell(adj).id) and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
        case 3: //HAMMER KILLS
        {
          int id3 = cell(ps).id;
          if (id3 != -1 and not is_ally(id3) and citizen(id3).weapon == NoWeapon) {
            next_step = curr.first_step;
            return curr.dist;
          }
          for (int k = 0; k < 4; ++k) {
            Pos adj = ps + dirs[k];

            if (first_moves < 4) {  //for storing the direction of the initial step
              curr.first_step = dirs[k];
              ++first_moves;
            }
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).b_owner == me() or cell(adj).b_owner == -1) and not is_ally(cell(adj).id) and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
        case 4: //FIND FOOD
        {
          BonusType bonus4 = cell(ps).bonus;
          if (bonus4 == Food) {
            next_step = curr.first_step;
            return curr.dist;
          }
          for (int k = 0; k < 4; ++k) {
            Pos adj = ps + dirs[k];

            if (first_moves < 4) {  //for storing the direction of the initial step
              curr.first_step = dirs[k];
              ++first_moves;
            }
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).b_owner == me() or cell(adj).b_owner == -1) and cell(adj).id == -1 and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
        case 5: //FIND MONEY
        {
          BonusType bonus5 = cell(ps).bonus;
          if (bonus5 == Money) {
            next_step = curr.first_step;
            return curr.dist;
          }
          for (int k = 0; k < 4; ++k) {
            Pos adj = ps + dirs[k];

            if (first_moves < 4) {  //for storing the direction of the initial step
              curr.first_step = dirs[k];
              ++first_moves;
            }
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).b_owner == me() or cell(adj).b_owner == -1) and cell(adj).id == -1 and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
        case 6: //FIND BARRICADE
        {
          int owner6 = cell(ps).b_owner;
          if (owner6 == me() and not is_ally(cell(ps).id)) {
            next_step = curr.first_step;
            return curr.dist;
          }
          for (int k = 0; k < 4; ++k) {
            Pos adj = ps + dirs[k];

            if (first_moves < 4) {  //for storing the direction of the initial step
              curr.first_step = dirs[k];
              ++first_moves;
            }
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).b_owner == me() or cell(adj).b_owner == -1) and cell(adj).id == -1 and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
        case 7: //FIND BAZOOKA
          {
          WeaponType weapon7 = cell(ps).weapon;
          if (weapon7 == Bazooka) {
            next_step = curr.first_step;
            return curr.dist;
          }
          for (int k = 0; k < 4; ++k) {
            Pos adj = ps + dirs[k];

            if (first_moves < 4) {  //for storing the direction of the initial step
              curr.first_step = dirs[k];
              ++first_moves;
            }
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).b_owner == me() or cell(adj).b_owner == -1) and not is_ally(cell(adj).id) and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
        case 8: //FIND GUN
        {
          WeaponType weapon8 = cell(ps).weapon;
          if (weapon8 == Gun) {
            next_step = curr.first_step;
            return curr.dist;
          }
          for (int k = 0; k < 4; ++k) {
            Pos adj = ps + dirs[k];

            if (first_moves < 4) {  //for storing the direction of the initial step
              curr.first_step = dirs[k];
              ++first_moves;
            }
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).b_owner == me() or cell(adj).b_owner == -1) and not is_ally(cell(adj).id) and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
        }
      }
    }
    return -1;
  }

//BUILDERS' FUNCTIONS---------------------------------------------------------------------------------------------------------

  //Avoid stronger enemies
  void avoid_stronger_enemies(const int& id) {
    //"Double for" vision
  }

  //Find a cell cointaining money and go step by step to get that money
  void search_money(const int& id) {
    Pos p = citizen(id).pos;
    Dir money_step;

    if (bfs_next_step(p, money_step, 5) != -1) {  //If money available on the map, go there

      if (pos_ok(p+money_step)) {
        move(id, money_step);
      }
    } else {
      Dir random_dir = dirs[random(0,3)];
	    if (pos_ok(p+random_dir)) {
	      move(id,random_dir);
      }
    }
  }

  //During day, builders must build up to 3 barricades in total, upgrade them to the max and then, collect money
  void build_and_collect(const int& id) {
    Pos p = citizen(id).pos;
    bool build_done = false;
    Pos np = p;
    int built_barricades = barricades(me()).size();
    if (cell(p).b_owner == -1 and (built_barricades < max_num_barricades() or maxed_barricades < built_barricades)) {
      for (uint i = 0; not build_done and i < dirs.size(); ++i){
        np = p + dirs[i];
        if (pos_ok(np) and cell(np).b_owner == me() and cell(np).resistance < barricade_max_resistance()) {
          build(id,dirs[i]);    //upgrade
          if (cell(np).resistance == barricade_max_resistance()) ++maxed_barricades;
          build_done = true;
        } else if (pos_ok(np) and cell(np).is_empty() and built_barricades < max_num_barricades()) {
          build(id,dirs[i]);    //construct
          build_done = true;
        }
      }
    }
    if (not build_done) {
      search_money(id);
    }
  }

  //It's nighttime and builders better hide in a barricade (while it is safe) or run through all the map searching money
  void run_and_hide(const int& id) {
    Pos p = citizen(id).pos;

    if (pos_ok(p) and cell(p).b_owner != me()) { //If not in a barricade
      Pos p = citizen(id).pos;
      Dir barr_step;

      if (bfs_next_step(p, barr_step, 6) != -1) {  //If barricade available on the map, go there
        if (pos_ok(p+barr_step)) {
          move(id, barr_step);
        }
      } else {
        search_money(id); //No hiding spot available, you could run for your life but it's better to gather some money
      }
    } else {
      if (pos_ok(p) and cell(p).b_owner == me() and cell(p).resistance < 80) {  //Someone's breaking your barricade, escape before it's too late
        search_money(id); //There's a guy chasing you, but you won't run for your life: you will run for money
      }
    }
  }

//WARRIORS' FUNCTIONS-------------------------------------------------------------------------------------------------------------

  //The main warrior goal is to find a bazooka but if on the path of looking for it
  //you encounter an enemy with a lower weapon, this function will handle the kill
  void no_bazooka_hunt(const int& id) {
    Pos p = citizen(id).pos;
    Dir kill_step;

    if (citizen(id).weapon == Gun) {
      if (bfs_next_step(p, kill_step, 2) != -1) {  //If weaker enemy on the map, go kill him
        if (pos_ok(p+kill_step)) {
          move(id, kill_step);
        }
      } else {
        search_money(id);
      }
    } else if (citizen(id).weapon == Hammer) {
        if (bfs_next_step(p, kill_step, 3) != -1) {  //If weaker enemy on the map, go kill him
        if (pos_ok(p+kill_step)) {
          move(id, kill_step);
        }
      } else {
        search_money(id);
      }
    }
  }

  //To survive through the night, a gun will increase your chances
  void find_gun(const int& id) {
    Pos p = citizen(id).pos;
    Dir gun_step;
    if (bfs_next_step(p, gun_step, 8) != -1) {  //If gun available on the map, go there
      if (pos_ok(p+gun_step)) {
        move(id, gun_step);
      }
    } else {
      guns_available = false;
      if (is_day()) search_money(id);
      else no_bazooka_hunt(id);
    }
  }

  //To survive through the night, a bazooka will surely increase your chances and a gun could be helpful
  void find_bazooka(const int& id) {
    Pos p = citizen(id).pos;
    Dir baz_step;
    if (bazookas_available and bfs_next_step(p, baz_step, 7) != -1) {  //If bazooka available on the map, go there
      if (pos_ok(p+baz_step)) {
        move(id, baz_step);
      }
    } else {
      bazookas_available = false;
      find_gun(id); //If no bazookas available, try searching for a gun
    }
  }

  //Warrior is low on health (will die of 2 or less enemy hits), better find some food
  void search_food(const int& id) {
    Pos p = citizen(id).pos;
    Dir food_step;
    if (bfs_next_step(p, food_step, 4) != -1) {  //If food available on the map, go there
      if (pos_ok(p+food_step)) {
        move(id, food_step);
      }
    } else {
      search_money(id); //future updates: here will go avoid_all_enemies
    }
  }

  //Follow enemies and kill them (this is one of the functions that will give the most points at the end of the night)
  //Pre: you have a Bazooka 
  void enemy_hunt(const int& id) {
    Pos p = citizen(id).pos;
    Dir kill_step;
    if (bfs_next_step(p, kill_step, 1) != -1) {  //If weaker enemy on the map, go kill him
      if (pos_ok(p+kill_step)) {
        move(id, kill_step);
      }
    } else {
      search_money(id); //If no weaklings to kill (highly improbable situation), just chill by gathering some money
    }
  }

  //Now that you've got a bazooka in your hands, time to go on a killing spree and heal during day if needed
  void kill_and_heal(const int& id) {
    if (citizen(id).life <= 40 and is_day()) { //Low on health, better search for food
      search_food(id);
    } else {
      enemy_hunt(id);
    }
  }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    //DAYTIME: warriors look for better weapons until they have bazooka, then they gather money and food
    //         builders build barricades to the max and then gather money and food

    //NIGHTTIME: warriors aim for weaker enemy citizens
    //           builders hide in barricades and run and gather food and money if no more barricades left

    guns_available = true;
    bazookas_available = true;
    vector<int> w = warriors(me());
    for(int id : w) {
      if (citizen(id).weapon != Bazooka) {  //Main goal: find a bazooka (but kill the weaklings on your way)
        find_bazooka(id);
      } else {
        kill_and_heal(id);
      }
    }
    
    maxed_barricades = 0;
    vector<int> b = builders(me());
    if (is_day()) {
      for(int id : b) {
        build_and_collect(id);
      }
    } else {
      for(int id : b) {
        run_and_hide(id);
      }
    }
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
