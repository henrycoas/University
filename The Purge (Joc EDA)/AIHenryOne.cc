#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME HenryOne


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

  //Will be used for the BFS procedure
  struct queueCell {
    Pos ps;
    int dist;
    Dir first_step;
  };

  struct pos_and_weapon {
    Pos ps;
    WeaponType w;
  };

  //Stores info of the position of Bazookas in the start of the current round
  //Used by: 1.Warriors that want to upgrade their weapon
  vector<Pos> bazooka_coords;

  //Stores info of the position of Guns in the start of the current round
  //Used by: 1.Warriors that want to upgrade their weapon
  vector<Pos> gun_coords;

  //Stores info of the position of current active Barricades 
  //Used by: 1.(Some) builders when night comes
  vector<Pos> barricades_coords;

  //Stores info of the position of Food in the start of the current round
  //Used by: 1.Warriors on low health
  vector<Pos> food_coords;

  //Stores info of the position of Money in the start of the current round
  //Used by: 1.Builders at day
  vector<Pos> money_coords;

  //Stores info of the position of Enemies in the start of the current round
  //Used by: 1.Warriors with bazooka 2.Warriors that cannot upgrade their weapon in this round
  vector<pos_and_weapon> enemy_coords;

  //Number of build Barricades
  int nbarricades = 0;

//Returns true if and only if id corresponds with a citizen from your team
  bool ally_citizen(const int& id) {
    if (id == -1) {
      return false;
    } 
    else {
      vector<int> w = warriors(me());
      vector<int> b = builders(me());

      int size = w.size();
      //We iterate through all warriors
      for(int i = 0; i < size; ++i) {
        if (w[i] == id) return true;
      } 

      size = b.size();
      //We iterate through all builders
      for(int i = 0; i < size; ++i) {
        if (b[i] == id) return true;
      } 
      
      return false;
    }
  }

//MOVEMENT BY OBJECTIVES FUNCTION (Money, Food, Weapons...)
  //Pre: Pos orig and dest are ok positions
  //Returns distance from dest to dest and stores in next_step the first step of the shortest path
  int bfs_next_step(const Pos& orig, const Pos& dest, Dir& next_step) {
    vector< vector<bool> > visited(board_rows(), vector<bool>(board_cols(), false));
    
    //Origin is considered visited
    visited[orig.i][orig.j] = true;

    //
    queue<queueCell> Q;

    //Source cell dist is 0
    queueCell o = {orig, 0};
    Q.push(o);

    int first_moves = 0;

    while (not Q.empty()) {
      queueCell curr = Q.front();
      Pos ps = curr.ps;
      Q.pop();

      if (ps.i == dest.i and ps.j == dest.j) {
        next_step = curr.first_step;
        return curr.dist;
      } 
      else {
        for (int k = 0; k < 4; ++k) {
        Pos adj = ps + dirs[k];

        if (first_moves < 4) {  //for storing the direction of the initial step
          curr.first_step = dirs[k];
          ++first_moves;
        }

        if (pos_ok(adj) and cell(adj).type == Street and (cell(adj).b_owner == -1 or cell(adj).b_owner == me()) and not ally_citizen(cell(adj).id) and not visited[adj.i][adj.j]) {
          visited[adj.i][adj.j] = true;
          queueCell Adjacell = {adj, curr.dist+1, curr.first_step};
          Q.push(Adjacell);
        }
      }
      }
    }
    return -1;
  }

//BUILDERS' FUNCTIONS------------------------------------------------------------------------------

  //During day, builder must build a barricade and upgrade it to the max
  void build_max_barricade(const int& id) {
    Pos p = citizen(id).pos;
    bool some_dir = false;
    Pos np = p;
    Dir d = dirs[0];
    for (uint i = 0; not some_dir and i < dirs.size(); ++i){
      d = dirs[i];
      np = p + d;
      if (pos_ok(np) and cell(np).b_owner == me()) build(id,d); // upgrade
      else if (pos_ok(np) and cell(np).is_empty()) { // pos_ok checks np is in the board
        ++nbarricades;
        build(id,d);    // construct
        some_dir = true;
      }
    }
  }

  //During day and after building all barricades, builders gather money
  void search_money(const int& id) {
    Pos p = citizen(id).pos;
    int money_available = money_coords.size();

    Pos closest_money;
    int dist_cl_money = -1;
    Dir first_money_step;
    int it_cl_money = 0;

    for(int i = 0; i < money_available; ++i) {
      if (pos_ok(money_coords[i])) {   //in the same round, more than 1 warrior would need to find money, so the first one to start going for it will have preference
        int current_dist = bfs_next_step(p, money_coords[i], first_money_step);
        if ((current_dist < dist_cl_money or dist_cl_money == -1) and current_dist != -1) {
          closest_money = money_coords[i];
          dist_cl_money = current_dist;
          it_cl_money = i;
        }
      }
    }

    if (dist_cl_money != -1) { //If money available on the map, go there
      if (pos_ok(p+first_money_step)) {
        move(id,first_money_step);
      }
      money_coords[it_cl_money] = Pos(-1,-1);
    }
    else {  //If not, just move randomly for now
      move(id,dirs[random(0,3)]);
    }
  }

  //It's nighttime and builders better hide in a barricade or run from enemies
  void hide_and_run(const int& id) {
    Pos p = citizen(id).pos;

    if (cell(p).b_owner != me()) {  //Not in a barricade
      vector<Pos> bs = barricades(me());
      bool barr_nearby = false;
      Dir barr_dir;
      for (Dir d : dirs) {
        if (pos_ok(p+d) and cell(p+d).id != -1 and cell(p+d).b_owner == me()) {
          barr_nearby = true;
          barr_dir = d;
        }
      }
      if (barr_nearby) move(id,barr_dir);
      else { // No barricade nearby, move randomly
          Dir random_dir = dirs[random(0,3)];
          if (pos_ok(p+random_dir)) {
            move(id,random_dir);
          }
      }
    }
  }

//WARRIORS' FUNCTIONS-------------------------------------------------------------------------------------------------------------

  //The main warrior goal is to find a bazooka but if on the path of looking for it
  //you encounter an enemy with a lower weapon, this function will handle the kill
  void no_bazooka_hunt(const int& id) {
    Pos p = citizen(id).pos;
    int enemy_available = enemy_coords.size();

    Pos closest_enemy;
    int dist_cl_enemy = -1;
    Dir first_enemy_step;
    //int it_cl_enemy = 0;

    if (citizen(id).weapon == Gun) {
      for(int i = 0; i < enemy_available; ++i) {
        if (pos_ok(enemy_coords[i].ps) and enemy_coords[i].w != Gun) {   //in the same round, more than 1 warrior would need to find food, so the first one to start going for it will have preference
          int current_dist = bfs_next_step(p, enemy_coords[i].ps, first_enemy_step);
          if ((current_dist < dist_cl_enemy  or dist_cl_enemy == -1) and current_dist != -1) {
            closest_enemy = enemy_coords[i].ps;
            dist_cl_enemy = current_dist;
            //it_cl_enemy = i;
          }
        }
      }
    } else if (citizen(id).weapon == Hammer) {
        for(int i = 0; i < enemy_available; ++i) {
        if (pos_ok(enemy_coords[i].ps) and enemy_coords[i].w != Gun and enemy_coords[i].w != Hammer) {   //in the same round, more than 1 warrior would need to find food, so the first one to start going for it will have preference
          int current_dist = bfs_next_step(p, enemy_coords[i].ps, first_enemy_step);
          if ((current_dist < dist_cl_enemy or dist_cl_enemy == -1) and current_dist != -1) {
            closest_enemy = enemy_coords[i].ps;
            dist_cl_enemy = current_dist;
            //it_cl_enemy = i;
          }
        }
      }
    }
    //Move accordingly
    if (dist_cl_enemy != -1) { //If enemy available on the map, go there
      if (pos_ok(p+first_enemy_step)) {
        move(id,first_enemy_step);
      }
      //enemy_coords[it_cl_enemy] = Pos(-1,-1);
    }
    else {  //If not, just move randomly for now
      Dir random_dir = dirs[random(0,3)];
      if (pos_ok(p+random_dir)) {
        move(id,random_dir);
      }
    }
  }

  //To kinda survive in the wild, a gun needs to be found
  void find_gun(const int& id) {
    Pos p = citizen(id).pos;
    int gun_available = gun_coords.size();
    Pos closest_gun;
    int dist_cl_gun = -1;
    Dir first_gun_step;
    //int it_cl_gun = 0;

    for(int i = 0; i < gun_available; ++i) {
      if (pos_ok(gun_coords[i])) {   //in the same round, more than 1 warrior would need to find bazooka, so the first one to start going for it will have preference
        int current_dist = bfs_next_step(p, gun_coords[i], first_gun_step);
        if ((current_dist < dist_cl_gun or dist_cl_gun == -1) and current_dist != -1) {
          closest_gun = gun_coords[i];
          dist_cl_gun = current_dist;
          //it_cl_gun = i;
        }
      }
    }
    if (dist_cl_gun != -1) { //If bazooka available on the map, go there
      if (pos_ok(p+first_gun_step)) {
        move(id,first_gun_step);
      }
    }
  }

  //To survive in the wild, a bazooka needs to be found
  void find_bazooka(const int& id) {
    Pos p = citizen(id).pos;
    int baz_available = bazooka_coords.size();
    Pos closest_baz;
    int dist_cl_baz = -1;
    Dir first_baz_step;
    //int it_cl_baz = 0;

    for(int i = 0; i < baz_available; ++i) {
      if (pos_ok(bazooka_coords[i])) {   //in the same round, more than 1 warrior would need to find bazooka, so the first one to start going for it will have preference
        int current_dist = bfs_next_step(p, bazooka_coords[i], first_baz_step);
        if ((current_dist < dist_cl_baz  or dist_cl_baz == -1) and current_dist != -1) {
          closest_baz = bazooka_coords[i];
          dist_cl_baz = current_dist;
          //it_cl_baz = i;
        }
      }
    }
    if (dist_cl_baz != -1) { //If bazooka available on the map, go there
      if (pos_ok(p+first_baz_step)) {
        move(id,first_baz_step);
      }
    }
    else {  //If not, just move randomly for now
      if (cell(citizen(id).pos).weapon == Hammer and gun_coords.size() > 0) {
        find_gun(id);
      } else {
        no_bazooka_hunt(id);
      }
    }
  }

  //Warrior is low on health (will die of 2 or less hits), better BFS for food
  void search_food(const int& id) {
    Pos p = citizen(id).pos;
    int food_available = food_coords.size();

    Pos closest_food;
    int dist_cl_food = -1;
    Dir first_food_step;
    int it_cl_food = 0;

    for(int i = 0; i < food_available; ++i) {
      if (pos_ok(food_coords[i])) {   //in the same round, more than 1 warrior would need to find food, so the first one to start going for it will have preference
        int current_dist = bfs_next_step(p, food_coords[i], first_food_step);
        if ((current_dist < dist_cl_food  or dist_cl_food == -1) and current_dist != -1) {
          closest_food = food_coords[i];
          dist_cl_food = current_dist;
          it_cl_food = i;
        }
      }
    }

    if (dist_cl_food != -1) { //If food available on the map, go there
      if (pos_ok(p+first_food_step)) {
        move(id,first_food_step);
      }
      food_coords[it_cl_food] = Pos(-1,-1);
    }
    else {  //If not, just move randomly for now
      move(id,dirs[random(0,3)]);
    }
  }

  //This is one of the functions that will give the most points at the end of the day
  //Patrol for enemies, if found one without a lower weapon, follow him until you kill him
  void enemy_hunt(const int& id) {
    Pos p = citizen(id).pos;
    int enemy_available = enemy_coords.size();

    Pos closest_enemy;
    int dist_cl_enemy = -1;
    Dir first_enemy_step;
    //int it_cl_enemy = 0;

    for(int i = 0; i < enemy_available; ++i) {
      if (pos_ok(enemy_coords[i].ps)) {   //in the same round, more than 1 warrior would need to find food, so the first one to start going for it will have preference
        int current_dist = bfs_next_step(p, enemy_coords[i].ps, first_enemy_step);
        if ((current_dist < dist_cl_enemy  or dist_cl_enemy == -1) and current_dist != -1) {
          closest_enemy = enemy_coords[i].ps;
          dist_cl_enemy = current_dist;
          //it_cl_enemy = i;
        }
      }
    }

    if (dist_cl_enemy != -1) { //If enemy available on the map, go there
      if (pos_ok(p+first_enemy_step)) {
        cerr << "I'm " << id << "and I'm going " << first_enemy_step << " towards enemy in row " << closest_enemy.i << " and col " << closest_enemy.j << endl;
        move(id,first_enemy_step);
      }
      //enemy_coords[it_cl_enemy] = Pos(-1,-1);
    }
    else {  //If not, just move randomly for now
      Dir random_dir = dirs[random(0,3)];
      if (pos_ok(p+random_dir)) {
        move(id,random_dir);
      }
    }
             
  }

  //Now that you've got a bazooka in your hands, time to go on a killing spree and heal if needed
  void kill_and_heal(const int& id) {
    if (citizen(id).life <= 40) { //Low on health, better search for food
      search_food(id);
    } else {  //Health is ok, focus on killing enemies
      enemy_hunt(id);
    }
  }

//MISC. FUNCTIONS---------------------------------------------------------------------------------------

  //at the begining of each round, the Pos of the weapons are stored for later use
  void useful_coords() {
    food_coords.clear();
    bazooka_coords.clear();
    money_coords.clear();
    gun_coords.clear();
    enemy_coords.clear();
    for(int i = 0; i < board_rows(); ++i) {
      for(int j = 0; j < board_cols(); ++j) {
        Pos x = Pos(i,j);
        Cell xc = cell(x);
        if (pos_ok(x) and not xc.is_empty()) {
          if (xc.bonus == Food) {
            food_coords.push_back(x);
          } 
          else if (xc.bonus == Money) {
            money_coords.push_back(x);
          } 
          else if (xc.weapon == Gun) {
            gun_coords.push_back(x);
          } 
          else if (xc.weapon == Bazooka) {
            bazooka_coords.push_back(x);
          } 
          else if (xc.id != -1 and not ally_citizen(xc.id) and xc.weapon != Bazooka) { //Current strat: don't chase enemy warriors with bazooka
            pos_and_weapon xpw = {x, xc.weapon};
            if (xc.weapon == Gun) {
              enemy_coords.push_back(xpw);
            } else if (xc.weapon == Hammer) {
              enemy_coords.push_back(xpw);
            } else if (xc.weapon == NoWeapon) {
              enemy_coords.push_back(xpw);
            }
          }
        }
      }
    }
  }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    useful_coords();
    vector<int> w = warriors(me()); //Current strat: Warriors don't give a shit if it's day or night, their motto: bazooka, kill and heal
    for(int id : w) {
      if (citizen(id).weapon != Bazooka) {  //Main goal: find a bazooka (but kill the weaklings on your way)
        find_bazooka(id);
      } else {  //After completing main goal of getting a bazooka, warriors focus on killing and healing if necessary
        kill_and_heal(id);
      }
    }
    
    nbarricades = 0;
    vector<int> b = builders(me());
    if (is_day()) {
      //DAYTIME: warriors look for better weapons until they have bazooka, then they gather money and food
      //         builders build barricades to the max and then gather money and food
     
      for(int id : b) {
        if (nbarricades < max_num_barricades()) {
          build_max_barricade(id);
        }
        search_money(id);
      }
    } else {
      //NIGHTTIME: warriors with bazooka go killing, warriors with gun or hammer aim for builders
      //           builders hide in barricades and run and gather food and money if no more barricades left
      for(int id : b) {
        hide_and_run(id);
      }
    }
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
