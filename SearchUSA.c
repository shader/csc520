#include <iostream>
#include <cmath>
#include <cstring>
using namespace std;

#define PI 3.14159265

struct city
{
  string name;
  double lat, lon;
};
city* cities[200];
int cur_city=0;

void make_city(string name, double lat, double lon)
{  
  city* c = new city;
  cities[cur_city] = c;
  c->name.assign(name);
  c->lat = lat;
  c->lon = lon;
  cur_city++;
}

double distance(city* city1, city* city2)
{
  double avglon = city1->lon + city2->lon / 2;
  double londist = 111.2 * cos(avglon * PI/180);
  return sqrt(pow(londist,2) + pow((city2->lat - city1->lat) * 111.2, 2));
}

int main()
{
  make_city("toledo", 12,124);
  cout << cities[0]->name;
}
