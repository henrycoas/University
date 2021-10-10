#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME KingusMk3

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

  //This structure will come in handy for the BFS procedure
  struct queueCell {
    Pos ps;         //coordinates of the cell
    int dist;       //steps from the player to ps
    Dir first_step; //direction of the first step taken from origin to reach the position ps
  };

  //For each round, true until proven otherwise
  //If in the current round, a warrior concludes there's no weapon of X type, he will comunicate it to the other warrior through this booleans
  bool guns_available = true;
  bool bazookas_available = true;

  //Number of maxed barricades (DEFAULT SETTINGS: maxed barricade = barricade with 320 resistance points)
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

  //Returns whether p is an strategic cell position for barricades to be built in
  //"Strategic cell" defined as the one with just 2 out of 4 directions are possible
  bool pos_is_strategic(const Pos p) {
    if (((pos_ok(p+Up) and cell(p+Up).type == Building) or not pos_ok(p+Up)) 
      and ((pos_ok(p+Down) and cell(p+Down).type == Building) or not pos_ok(p+Down)) 
      and (pos_ok(p+Left) and cell(p+Left).type == Street) and (pos_ok(p+Right) and cell(p+Right).type == Street)) {
        return true;
    } else if (((pos_ok(p+Right) and cell(p+Right).type == Building) or not pos_ok(p+Right)) 
      and ((pos_ok(p+Left) and cell(p+Left).type == Building) or not pos_ok(p+Left)) 
      and (pos_ok(p+Up) and cell(p+Up).type == Street) and (pos_ok(p+Down) and cell(p+Down).type == Street)) {
        return true;
    } else {
      return false;
    }

  }

//MOVEMENT BY OBJECTIVES FUNCTION (Money, Food, Weapons...)---------------------------------------------------------------------------

  //Returns distance from origin to the closest cell that accomplishes the aim stated. Returns -1 if there's no way of accomplishing that aim
  //Parameter aim values: 1 -> get kills with Bazooka                    (search for cell with weaker enemy)
  //                      2 -> get kills with Gun                        (cell with weaker enemy)
  //                      3 -> get kills with Hammer                     (cell with weaker enemy)
  //                      4 -> find Food                                 (cell with food bonus)
  //                      5 -> find Money                                (cell with money bonus)
  //                      6 -> find an empty ally Barricade to hide in   (cell with an unoccupied barricade)
  //                      7 -> find Bazooka                              (cell with bazooka)
  //                      8 -> find Gun                                  (cell with gun)
  //                      9 -> escape from enemy Warriors                (cell with enemy warrior)
  //                     10 -> follow weaker enemy Warriors              (cell with enemy warrior that has a lower weapon than a bazooka)
  //                     11 -> find a good place to build a Barricade    (cell that accomplishes some strategic conditions -see "pos_is_strategic" function-)
  int bfs_next_step(const Pos& orig, Dir& next_step, int aim) {
    vector< vector<bool> > visited(board_rows(), vector<bool>(board_cols(), false));
    
    //Origin is considered visited
    visited[orig.i][orig.j] = true;

    queue<queueCell> Q;

    //Origin
    queueCell o = {orig, 0, Up};
    Q.push(o);

    int first_moves = 0;

    while (not Q.empty()) {
      queueCell curr = Q.front();
      Pos ps = curr.ps;
      Q.pop();

      if (pos_ok(ps)) {
        //Each type of BFS search will have its own cells to avoid and its own destination cell
        switch(aim) {
        case 1: //BAZOOKA KILLS--------------------------
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
        case 2: //GUN KILLS---------------------------
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
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).resistance <= 40 or cell(adj).b_owner == me() or cell(adj).b_owner == -1) and not is_ally(cell(adj).id) and (cell(adj).id == -1 or citizen(cell(adj).id).weapon != Bazooka) and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
        case 3: //HAMMER KILLS----------------------------
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
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).b_owner == me() or cell(adj).b_owner == -1) and not is_ally(cell(adj).id) and (cell(adj).id == -1 or citizen(cell(adj).id).weapon == NoWeapon) and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
        case 4: //FIND FOOD--------------------------------
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
        case 5: //FIND MONEY------------------------------------
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
        case 6: //FIND BARRICADE-------------------------------------
        {
          int owner6 = cell(ps).b_owner;
          if (owner6 == me() and not is_ally(cell(ps).id) and cell(ps).resistance >= 80) {
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
        case 7: //FIND BAZOOKA----------------------------------------
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
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).b_owner == me() or cell(adj).b_owner == -1) and cell(adj).id == -1 and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
        case 8: //FIND GUN-------------------------------------
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
            if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).b_owner == me() or cell(adj).b_owner == -1) and cell(adj).id == -1 and not visited[adj.i][adj.j]) {
              visited[adj.i][adj.j] = true;
              queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
              Q.push(Adjacell);
            }
          }
        }
          break;
          case 9: //LOCATE ENEMY WARRIORS TO ESCAPE FROM THEM-----------
        {
          int id9 = cell(ps).id;
          if (id9 != -1 and not is_ally(id9) and citizen(id9).weapon != NoWeapon) {
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
          case 10: //LOCATE ENEMY WARRIORS WITH A WEAKER WEAPON THAN BAZOOKA----
        {
          int id10 = cell(ps).id;
          if (id10 != -1 and not is_ally(id10) and citizen(id10).weapon != NoWeapon and citizen(id10).weapon != Bazooka) {
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
          case 11: //FIND A STRATEGIC PLACE TO BUILD A BARRICADE-------------
        {
          int id10 = cell(ps).id;
          if (pos_is_strategic(ps)) {
            next_step = curr.first_step;
            return curr.dist;
          }
          for (int k = 0; k < 4; ++k) {
            Pos adj = ps + dirs[k];

            if (first_moves < 4) {  //for storing the direction of the initial step
              curr.first_step = dirs[k];
              ++first_moves;
            }
            if (pos_ok(adj) and not visited[adj.i][adj.j] and cell(adj).b_owner == -1 and cell(adj).type == Street and cell(adj).id == -1) {
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
	    if (pos_ok(p+random_dir) and cell(p+random_dir).type == Street) {
	      move(id,random_dir);
      }
    }
  }

  //Returns the opposite direction to d
  Dir opposite_dir(const Dir d) {
    if (d == Up) return Down;
    else if (d == Down) return Up;
    else if (d == Right) return Left;
    else return Right;
  }

  //Returns the two perpendicular directions to d
  pair<Dir,Dir> perpendicular_dirs(const Dir d) {
    if (d == Up or d == Down) return make_pair(Right, Left);
    else return make_pair(Down, Up);
  }

  //Avoid warriors by not getting too close to them
  //(A good strat could also be to lure warriors to an ally bazooka warrior but for now it is not implemented)
  void avoid_warriors(const int& id) {
    Pos p = citizen(id).pos;
    Dir enemy_step;
    int enemy_dist = bfs_next_step(p, enemy_step, 9);
    if (enemy_dist != -1 and enemy_dist < 5) {  //If warriors active on the map, escape from them
      Dir escape_step = opposite_dir(enemy_step);
      if (pos_ok(p+escape_step) and cell(p+escape_step).type == Street and (cell(p+escape_step).b_owner == me() or cell(p+escape_step).b_owner == -1) and cell(p+escape_step).id == -1) {  //if builder can go the exact opposite direction, go for it
        move(id, escape_step);
      } else {        //If not, escape perpendicularly
        pair<Dir,Dir> esc_steps2 = perpendicular_dirs(escape_step);
        if (pos_ok(p+esc_steps2.first) and cell(p+esc_steps2.first).type == Street and (cell(p+esc_steps2.first).b_owner == me() or cell(p+esc_steps2.first).b_owner == -1) and cell(p+esc_steps2.first).id == -1) { 
          move(id, esc_steps2.first);
        } else if (pos_ok(p+esc_steps2.second) and cell(p+esc_steps2.second).type == Street and (cell(p+esc_steps2.second).b_owner == me() or cell(p+esc_steps2.second).b_owner == -1) and cell(p+esc_steps2.second).id == -1) { 
          move(id, esc_steps2.second);
        }
      } 
    } else {
      search_money(id);
    }
  }

  //During day, builders must build up to 3 barricades in total, upgrade them to the max and then, collect money
  void build_and_collect(const int& id) {
    Pos p = citizen(id).pos;
    bool build_done = false;
    Pos np = p;
    int built_barricades = barricades(me()).size();

    if (cell(p).b_owner == -1 and (built_barricades < max_num_barricades() or maxed_barricades < built_barricades)) {
      //Upgrade a barricade if possible and if close to one
      for (uint i = 0; not build_done and i < dirs.size(); ++i){
        np = p + dirs[i];
        if (pos_ok(np) and cell(np).b_owner == me() and cell(np).resistance < barricade_max_resistance()) {
          build(id,dirs[i]);    //upgrade
          if (cell(np).resistance == barricade_max_resistance()-barricade_resistance_step()) ++maxed_barricades;
          build_done = true;
        } 
      }
      //Go to strategic point and build
      if (not build_done and built_barricades < max_num_barricades()) {
        Dir build_step;
        int build_dist = bfs_next_step(p, build_step, 11);

        if (build_dist != -1) {  //If strategic points to build available on the map, go there to build
          if (build_dist == 1) {  //build
            if (pos_ok(p+build_step) and cell(p+build_step).is_empty()) {
              build(id, build_step);
              build_done = true;
            } else if (cell(p+build_step).b_owner == -1 and cell(p+build_step).type == Street and cell(p+build_step).id == -1) {
              move(id, build_step); //Grab what's on that position and build in future rounds
            }
          } else if (build_dist > 1) {
            if (pos_ok(p+build_step)) {
              move(id, build_step);
              build_done = true;  //It's not directly a building action but it's aimed at that objective
            }
          }
        } else {  //Seems like there aren't any strategic positions, so build anywhere, it's okey
          for (uint i = 0; not build_done and i < dirs.size(); ++i) {
            np = p + dirs[i];
            if (pos_ok(np) and cell(np).is_empty() and built_barricades < max_num_barricades()) {
              build(id,dirs[i]);    //construct
              build_done = true;
            }
          }
        }
      }
    }
    if (not build_done) {
      if (citizen(id).life <= 40) { //Low on health, better search for food
        search_food(id);
      } else {
        search_money(id);
      }
    }
  }

  //It's nighttime and builders better avoid warriors or hide in a barricade (while it is safe)
  void run_and_hide(const int& id) {
    if (true) { //For now, current strategy is to always avoid warriors during night, no hiding in barricades
      avoid_warriors(id);
    } else {  //Desperate measure: enemy is too ofensive on builders, gotta do the hiding play
      Pos p = citizen(id).pos;

      if (pos_ok(p) and cell(p).b_owner != me()) { //If not in a barricade
        Dir barr_step;

        if (bfs_next_step(p, barr_step, 6) != -1) {  //If empty ally barricade available on the map, go there
          if (pos_ok(p+barr_step)) {
            move(id, barr_step);
          }
        } else {
          avoid_warriors(id); //No hiding spot available, run for your life
        }
      } else {
        if (pos_ok(p) and cell(p).b_owner == me() and cell(p).resistance < 80) {  //Someone's breaking your barricade, escape before it's too late
          avoid_warriors(id); //There's an enemy about to break your barricade, run for your life
        }
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

  //To survive through the night, a bazooka will surely increase your chances and a gun could be helpful
  //Because there's no time to be picky, pick the closest stronger weapon
  void find_stronger_weapon(const int& id) {
    Pos p = citizen(id).pos;
    if (citizen(id).weapon == Gun) {  //Warrior currently has a Gun, the only stronger weapon is the Bazooka
      Dir baz_step;
      int baz_dist = -1;
      if (bazookas_available) baz_dist = bfs_next_step(p, baz_step, 7);
      if (baz_dist != -1 and (is_day() or (is_night() and baz_dist < 8))) {
        if (pos_ok(p+baz_step)) {
          move(id, baz_step);
        }
      } else { 
        if (baz_dist == -1) bazookas_available = false;
        no_bazooka_hunt(id);
      }
    } else {     //Warrior currently has a Hammer, go pick the closest Gun or Bazooka
      Dir baz_step, gun_step;
      int baz_dist = -1, gun_dist = -1;
      if (bazookas_available) baz_dist = bfs_next_step(p, baz_step, 7); 
      if (guns_available) gun_dist = bfs_next_step(p, gun_step, 8);
      if ((baz_dist != -1 and (baz_dist < gun_dist or gun_dist == -1)) and (is_day() or (is_night() and baz_dist < 8))) {
        if (pos_ok(p+baz_step)) {
          move(id, baz_step);
        }
      } else if ((gun_dist != -1 and (gun_dist < baz_dist or baz_dist == -1)) and (is_day() or (is_night() and gun_dist < 8))) {
        if (pos_ok(p+gun_step)) {
          move(id, gun_step);
        }
      } else {   //Okey, no bazookas and no guns in the map (for this round)
        if (gun_dist == -1) guns_available = false;
        if (baz_dist == -1) bazookas_available = false;
        if (is_day()) search_money(id);
        else no_bazooka_hunt(id);
      }
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
      avoid_warriors(id); //Main goal now is to heal so you can go killing again, so warriors won't exec this function many times
    }
  }

  //Follow enemies and kill them (this is one of the functions that will give the most points at the end of the night)
  //Pre: Warrior has a Bazooka 
  void enemy_hunt(const int& id) {
    Pos p = citizen(id).pos;
    Dir wkill_step, ekill_step;
    int wkill_dist = bfs_next_step(p, wkill_step, 10);  //locate closest warrior
    int ekill_dist = bfs_next_step(p, ekill_step, 1);   //locate closest enemy (warrior or builder)

    if (ekill_dist == -1) {  //Highly improbable case: no enemies on the map
      search_money(id);
    } else if ((wkill_dist != -1) and (wkill_dist < ekill_dist+4 or is_day())) {
        if (pos_ok(p+wkill_step)) {
          move(id, wkill_step);
        }
    } else {
      if (pos_ok(p+ekill_step)) {
        move(id, ekill_step);
      }
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
    //DAYTIME: Warriors look for better weapons until they have Bazooka, then they gather Money
    //         Builders build Barricades to the max and then gather Money

    //NIGHTTIME: Warriors aim for weaker enemy citizens (Warriors are preferable objectives under some circumstances)
    //           Builders hide in barricades or run for their life if no barricades left

    guns_available = true;
    bazookas_available = true;
    vector<int> w = warriors(me());
    for(int id : w) {
      if (citizen(id).weapon != Bazooka) {  //Main goal: find a bazooka (but kill the weaklings on your way)
        find_stronger_weapon(id);
      } else {
        kill_and_heal(id);
      }
    }
    
    if (is_night() or round() == 0) maxed_barricades = 0;
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
