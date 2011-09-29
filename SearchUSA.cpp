#include <iostream>
#include <cmath>
#include <cstring>
#include <deque>
#include <algorithm>
using namespace std;

#define PI 3.14159265
#define NUMCITIES 200
#define NUMROADS 300

struct city
{
  string name;
  double lat, lon;
};
city* cities[NUMCITIES];
int cur_city=0;
city* goal;

void make_city(string name, double lat, double lon)
{  
  city* c = new city;
  cities[cur_city] = c;
  c->name.assign(name);
  c->lat = lat;
  c->lon = lon;
  cur_city++;
}

city* find_city(string name)
{
  city *city = NULL;
  for(int i=0;i<NUMCITIES;i++)
    if(cities[i] != NULL && cities[i]->name == name)
      city = cities[i];
  return city;
}

struct road
{
  city *a, *b;
  double len;
};
road* roads[NUMROADS];
int cur_road=0;

void make_1way(city *a, city *b, int len)
{
  if (a == NULL || b == NULL) {
    cerr << "Trying to make road to null city: road #" << cur_road << "\n";
    return;
  }
  road *r = new road;
  roads[cur_road] = r;
  r->a = a;
  r->b = b;
  r->len = len;
  cur_road++;
}

void make_road(string name1, string name2, int len)
{
  city *a = find_city(name1);
  city *b = find_city(name2);
  make_1way(a,b,len);
  make_1way(b,a,len);
}
void make_cities();
void make_roads();

struct path
{
  path *prev;
  city *end;
  double cost;
};

struct pathByName {
  path* item;
  pathByName(path* it) { item = it; }
  bool operator()(path* p) {
    bool val = (p->end->name == item->end->name);
    return val;
  }
};

int path_count(path* p) {
  int i=0;
  while(p != NULL) {
    p = p->prev;
    i++;
  }
  return i;
}

double distance(city* city1, city* city2)
{
  //calculate average latitude
  double avglat = city1->lat + city2->lat / 2;
  //distance between two degrees of longitude is dependent on the latitude
  double londist = 69.5 * cos(avglat * PI/180);
  //calculate L2 norm distance
  return sqrt(pow(londist,2) + pow((city2->lat - city1->lat) * 69.5, 2));
}

deque<path*> neighbors(path* cur) {
  deque<path*> n;
  for (int i=0; i<NUMROADS; i++) {
    road *r = roads[i];
    if (r != NULL && r->a->name == cur->end->name) {
      path *p = new path;
      p->end = r->b;
      p->prev = cur;
      p->cost = cur->cost + r->len;
      n.push_back(p);
    }
  }
  return n;
}

struct soln {
  path* p;
  deque<path*> closed;  
};

bool astar_compare(path* p1, path* p2) {
  return (p1->cost + distance(p1->end, goal)) < (p2->cost + distance(p2->end, goal));
}

void print_queue(deque<path*> q) {
  for(deque<path*>::iterator i=q.begin(); i!=q.end(); i++)
    cout << (*i)->end->name << ", ";
  cout << "\n";
  cout.flush();
}

void add_paths(deque<path*> open, deque<path*> closed, deque<path*> paths) {
  deque<path*>::iterator i,j;
  
  //add neighbors to open list, duplicates included
  for(i=paths.begin(); i!=paths.end(); i++) {
    j = find_if(closed.begin(), closed.end(), pathByName(*i));
    if (j != closed.end()) continue; //neighbor already in closed, don't add
    j = find_if(open.begin(), open.end(), pathByName(*i));
    if (j != open.end()) { //found duplicate path
      if ((*i)->cost < (*j)->cost) { //new path is shorter
        open.erase(j); //erase old path
        open.push_back(*i); //add new path
      } else continue; //old path is shorter; don't add new one
    } else {
      open.push_back(*i); //neither already closed nor dup, add to open list
    }
  }
}

soln *astar(city* start, city* dest) {
  deque<path*>::iterator i,j;
  deque<path*> open,closed;
  goal = dest;

  path* s = new path;
  s->prev = NULL;
  s->end = start;
  s->cost = 0;
  open.push_back(s);

  while(!open.empty()) {
    path *cur = open.front();
    open.pop_front();
    closed.push_back(cur);

    if (cur->end->name == goal->name) {
      soln* s = new soln;
      s->p = cur;
      s->closed = closed;
      return s;
    }

    deque<path*>n = neighbors(cur);

    add_paths(open,closed,n);

    sort(open.begin(), open.end(), astar_compare);
  }
}

soln* greedy(city* start, city* dest) {
  deque<path*>::iterator i,j;
  deque<path*> open,closed;
  goal = dest;
}

soln* dynamic(city* start, city* dest) {
}

void print_path(path* p) {
  deque<string> names;
  while(p != NULL && p->end != NULL) {
    names.push_front(p->end->name);
    p = p->prev;
  }
  while(!names.empty()) {
    string n = names.front();
    names.pop_front();
    cout << n << ", ";
  }
}

void print_soln(soln* s) {
  cout << "Number of expanded nodes: " << s->closed.size() << "\n";
  cout << "Expanded nodes: "; print_queue(s->closed); cout <<"\n";
  cout << "Number of path nodes: " << path_count(s->p) << "\n";
  cout << "Path length: " << s->p->cost << "\n";
  cout << "Path: "; print_path(s->p); cout << "\n";
}

int main (int argc, char* argv[])
{
  make_cities();
  make_roads();

  if (argc < 4) {
    cout << "Please use ./a.out [astar|greedy|dynamic] [start] [destination]\n";
  } else {
    city* start = find_city(argv[2]);
    if (start == NULL)
      cerr << "Error: start city " << start << " could not be found.";

    city* dest = find_city(argv[3]);
    if (dest == NULL)
      cerr << "Error: destination city " << dest << " could not be found.";
    
    if (strcmp(argv[1], "astar")==0) {
      soln* s = astar(start, dest);
      print_soln(s);
    } else if (strcmp(argv[1], "greedy")==0) {
      soln* s = greedy(start, dest);
      print_soln(s);
    } else if (strcmp(argv[1], "dynamic")==0) {
      soln* s = dynamic(start, dest);
      print_soln(s);
    } else {
      cout << "Please use ./a.out [astar|greedy|dynamic] [start] [destination]\n";
    }
  }
}

void make_cities()
{
  for(int i=0;i<NUMCITIES;i++)
    cities[i]=NULL;

  make_city("albanyGA", 31.58, 84.17);
  make_city("albanyNY", 42.66, 73.78);
  make_city("albuquerque", 35.11, 106.61);
  make_city("atlanta", 33.76, 84.40);
  make_city("augusta", 33.43, 82.02);
  make_city("austin", 30.30, 97.75);
  make_city("bakersfield", 35.36, 119.03);
  make_city("baltimore", 39.31, 76.62);
  make_city("batonRouge", 30.46, 91.14);
  make_city("beaumont", 30.08, 94.13);
  make_city("boise", 43.61, 116.24);
  make_city("boston", 42.32, 71.09);
  make_city("buffalo", 42.90, 78.85);
  make_city("calgary", 51.00, 114.00);
  make_city("charlotte", 35.21, 80.83);
  make_city("chattanooga", 35.05, 85.27);
  make_city("chicago", 41.84, 87.68);
  make_city("cincinnati", 39.14, 84.50);
  make_city("cleveland", 41.48, 81.67);
  make_city("coloradoSprings", 38.86, 104.79);
  make_city("columbus", 39.99, 82.99);
  make_city("dallas", 32.80, 96.79);
  make_city("dayton", 39.76, 84.20);
  make_city("daytonaBeach", 29.21, 81.04);
  make_city("denver", 39.73, 104.97);
  make_city("desMoines", 41.59, 93.62);
  make_city("elPaso", 31.79, 106.42);
  make_city("eugene", 44.06, 123.11);
  make_city("europe", 48.87, 2.33);
  make_city("ftWorth", 32.74, 97.33);
  make_city("fresno", 36.78, 119.79);
  make_city("grandJunction", 39.08, 108.56);
  make_city("greenBay", 44.51, 88.02);
  make_city("greensboro", 36.08, 79.82);
  make_city("houston", 29.76, 95.38);
  make_city("indianapolis", 39.79, 86.15);
  make_city("jacksonville", 30.32, 81.66);
  make_city("japan", 35.68, 220.23);
  make_city("kansasCity", 39.08, 94.56);
  make_city("keyWest", 24.56, 81.78);
  make_city("lafayette", 30.21, 92.03);
  make_city("lakeCity", 30.19, 82.64);
  make_city("laredo", 27.52, 99.49);
  make_city("lasVegas", 36.19, 115.22);
  make_city("lincoln", 40.81, 96.68);
  make_city("littleRock", 34.74, 92.33);
  make_city("losAngeles", 34.03, 118.17);
  make_city("macon", 32.83, 83.65);
  make_city("medford", 42.33, 122.86);
  make_city("memphis", 35.12, 89.97);
  make_city("mexia", 31.68, 96.48);
  make_city("mexico", 19.40, 99.12);
  make_city("miami", 25.79, 80.22);
  make_city("midland", 43.62, 84.23);
  make_city("milwaukee", 43.05, 87.96);
  make_city("minneapolis", 44.96, 93.27);
  make_city("modesto", 37.66, 120.99);
  make_city("montreal", 45.50, 73.67);
  make_city("nashville", 36.15, 86.76);
  make_city("newHaven", 41.31, 72.92);
  make_city("newOrleans", 29.97, 90.06);
  make_city("newYork", 40.70, 73.92);
  make_city("norfolk", 36.89, 76.26);
  make_city("oakland", 37.80, 122.23);
  make_city("oklahomaCity", 35.48, 97.53);
  make_city("omaha", 41.26, 96.01);
  make_city("orlando", 28.53, 81.38);
  make_city("ottawa", 45.42, 75.69);
  make_city("pensacola", 30.44, 87.21);
  make_city("philadelphia", 40.72, 76.12);
  make_city("phoenix", 33.53, 112.08);
  make_city("pittsburgh", 40.40, 79.84);
  make_city("pointReyes", 38.07, 122.81);
  make_city("portland", 45.52, 122.64);
  make_city("providence", 41.80, 71.36);
  make_city("provo", 40.24, 111.66);
  make_city("raleigh", 35.82, 78.64);
  make_city("redding", 40.58, 122.37);
  make_city("reno", 39.53, 119.82);
  make_city("richmond", 37.54, 77.46);
  make_city("rochester", 43.17, 77.61);
  make_city("sacramento", 38.56, 121.47);
  make_city("salem", 44.93, 123.03);
  make_city("salinas", 36.68, 121.64);
  make_city("saltLakeCity", 40.75, 111.89);
  make_city("sanAntonio", 29.45, 98.51);
  make_city("sanDiego", 32.78, 117.15);
  make_city("sanFrancisco", 37.76, 122.44);
  make_city("sanJose", 37.30, 121.87);
  make_city("sanLuisObispo", 35.27, 120.66);
  make_city("santaFe", 35.67, 105.96);
  make_city("saultSteMarie", 46.49, 84.35);
  make_city("savannah", 32.05, 81.10);
  make_city("seattle", 47.63, 122.33);
  make_city("stLouis", 38.63, 90.24);
  make_city("stamford", 41.07, 73.54);
  make_city("stockton", 37.98, 121.30);
  make_city("tallahassee", 30.45, 84.27);
  make_city("tampa", 27.97, 82.46);
  make_city("thunderBay", 48.38, 89.25);
  make_city("toledo", 41.67, 83.58);
  make_city("toronto", 43.65, 79.38);
  make_city("tucson", 32.21, 110.92);
  make_city("tulsa", 36.13, 95.94);
  make_city("uk1", 51.30, 0.00);
  make_city("uk2", 51.30, 0.00);
  make_city("vancouver", 49.25, 123.10);
  make_city("washington", 38.91, 77.01);
  make_city("westPalmBeach", 26.43, 80.03);
  make_city("wichita", 37.69, 97.34);
  make_city("winnipeg", 49.90, 97.13);
  make_city("yuma", 32.69, 114.62);
}

void make_roads()
{
  for(int i=0;i<NUMROADS;i++)
    roads[i]=NULL;

  make_road("albanyNY", "montreal", 226);
  make_road("albanyNY", "boston", 166);
  make_road("albanyNY", "rochester", 148);
  make_road("albanyGA", "tallahassee", 120);
  make_road("albanyGA", "macon", 106);
  make_road("albuquerque", "elPaso", 267);
  make_road("albuquerque", "santaFe", 61);
  make_road("atlanta", "macon", 82);
  make_road("atlanta", "chattanooga", 117);
  make_road("augusta", "charlotte", 161);
  make_road("augusta", "savannah", 131);
  make_road("austin", "houston", 186);
  make_road("austin", "sanAntonio", 79);
  make_road("bakersfield", "losAngeles", 112);
  make_road("bakersfield", "fresno", 107);
  make_road("baltimore", "philadelphia", 102);
  make_road("baltimore", "washington", 45);
  make_road("batonRouge", "lafayette", 50);
  make_road("batonRouge", "newOrleans", 80);
  make_road("beaumont", "houston", 69);
  make_road("beaumont", "lafayette", 122);
  make_road("boise", "saltLakeCity", 349);
  make_road("boise", "portland", 428);
  make_road("boston", "providence", 51);
  make_road("buffalo", "toronto", 105);
  make_road("buffalo", "rochester", 64);
  make_road("buffalo", "cleveland", 191);
  make_road("calgary", "vancouver", 605);
  make_road("calgary", "winnipeg", 829);
  make_road("charlotte", "greensboro", 91);
  make_road("chattanooga", "nashville", 129);
  make_road("chicago", "milwaukee", 90);
  make_road("chicago", "midland", 279);
  make_road("cincinnati", "indianapolis", 110);
  make_road("cincinnati", "dayton", 56);
  make_road("cleveland", "pittsburgh", 157);
  make_road("cleveland", "columbus", 142);
  make_road("coloradoSprings", "denver", 70);
  make_road("coloradoSprings", "santaFe", 316);
  make_road("columbus", "dayton", 72);
  make_road("dallas", "denver", 792);
  make_road("dallas", "mexia", 83);
  make_road("daytonaBeach", "jacksonville", 92);
  make_road("daytonaBeach", "orlando", 54);
  make_road("denver", "wichita", 523);
  make_road("denver", "grandJunction", 246);
  make_road("desMoines", "omaha", 135);
  make_road("desMoines", "minneapolis", 246);
  make_road("elPaso", "sanAntonio", 580);
  make_road("elPaso", "tucson", 320);
  make_road("eugene", "salem", 63);
  make_road("eugene", "medford", 165);
  make_road("europe", "philadelphia", 3939);
  make_road("ftWorth", "oklahomaCity", 209);
  make_road("fresno", "modesto", 109);
  make_road("grandJunction", "provo", 220);
  make_road("greenBay", "minneapolis", 304);
  make_road("greenBay", "milwaukee", 117);
  make_road("greensboro", "raleigh", 74);
  make_road("houston", "mexia", 165);
  make_road("indianapolis", "stLouis", 246);
  make_road("jacksonville", "savannah", 140);
  make_road("jacksonville", "lakeCity", 113);
  make_road("japan", "pointReyes", 5131);
  make_road("japan", "sanLuisObispo", 5451);
  make_road("kansasCity", "tulsa", 249);
  make_road("kansasCity", "stLouis", 256);
  make_road("kansasCity", "wichita", 190);
  make_road("keyWest", "tampa", 446);
  make_road("lakeCity", "tampa", 169);
  make_road("lakeCity", "tallahassee", 104);
  make_road("laredo", "sanAntonio", 154);
  make_road("laredo", "mexico", 741);
  make_road("lasVegas", "losAngeles", 275);
  make_road("lasVegas", "saltLakeCity", 486);
  make_road("lincoln", "wichita", 277);
  make_road("lincoln", "omaha", 58);
  make_road("littleRock", "memphis", 137);
  make_road("littleRock", "tulsa", 276);
  make_road("losAngeles", "sanDiego", 124);
  make_road("losAngeles", "sanLuisObispo", 182);
  make_road("medford", "redding", 150);
  make_road("memphis", "nashville", 210);
  make_road("miami", "westPalmBeach", 67);
  make_road("midland", "toledo", 82);
  make_road("minneapolis", "winnipeg", 463);
  make_road("modesto", "stockton", 29);
  make_road("montreal", "ottawa", 132);
  make_road("newHaven", "providence", 110);
  make_road("newHaven", "stamford", 92);
  make_road("newOrleans", "pensacola", 268);
  make_road("newYork", "philadelphia", 101);
  make_road("norfolk", "richmond", 92);
  make_road("norfolk", "raleigh", 174);
  make_road("oakland", "sanFrancisco", 8);
  make_road("oakland", "sanJose", 42);
  make_road("oklahomaCity", "tulsa", 105);
  make_road("orlando", "westPalmBeach", 168);
  make_road("orlando", "tampa", 84);
  make_road("ottawa", "toronto", 269);
  make_road("pensacola", "tallahassee", 120);
  make_road("philadelphia", "pittsburgh", 319);
  make_road("philadelphia", "newYork", 101);
  make_road("philadelphia", "uk1", 3548);
  make_road("philadelphia", "uk2", 3548);
  make_road("phoenix", "tucson", 117);
  make_road("phoenix", "yuma", 178);
  make_road("pointReyes", "redding", 215);
  make_road("pointReyes", "sacramento", 115);
  make_road("portland", "seattle", 174);
  make_road("portland", "salem", 47);
  make_road("reno", "saltLakeCity", 520);
  make_road("reno", "sacramento", 133);
  make_road("richmond", "washington", 105);
  make_road("sacramento", "sanFrancisco", 95);
  make_road("sacramento", "stockton", 51);
  make_road("salinas", "sanJose", 31);
  make_road("salinas", "sanLuisObispo", 137);
  make_road("sanDiego", "yuma", 172);
  make_road("saultSteMarie", "thunderBay", 442);
  make_road("saultSteMarie", "toronto", 436);
  make_road("seattle", "vancouver", 115);
  make_road("thunderBay", "winnipeg", 440);
}
