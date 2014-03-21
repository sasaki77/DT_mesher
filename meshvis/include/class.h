#ifndef CLASS_H
#define CLASS_H

#include "preprocess.h"

class Node{
 public:
  double r,z;
  int id;
  void set( double _z, double _r ){
    z = _z;
    r = _r;
  }
  void set( double _z, double _r, int _id ){
    z = _z;
    r = _r;
    id = _id;
  }
};


class Element{
 public:
  Node *p[4];
};

class Edge{
public:
  Node *p[2];
  int bc;
  double val;

  Edge(){
    p[0] = NULL;
    p[1] = NULL;
  }
  Edge( Node *_p0, Node *_p1 ){
    p[0] = _p0;
    p[1] = _p1;
  }
  void set( Node *_p0, Node *_p1 ){
    p[0] = _p0;
    p[1] = _p1;
  }
  virtual double length() = 0;

  bool operator==( const Edge& obj ){
    return ( (p[0] == obj.p[0] && p[1] == obj.p[1]) || (p[0] == obj.p[1] && p[1] == obj.p[0]) );
  }
  bool operator!=( const Edge& obj ){
    return ( !((p[0] == obj.p[0] && p[1] == obj.p[1]) || (p[0] == obj.p[1] && p[1] == obj.p[0])) );
  }
};

class Str: public Edge{
 public:
 Str():Edge(){}
 Str( Node *_p0, Node *_p1 ):Edge(_p0,_p1){}
  double length(){
    double dz = fabs(p[1]->z - p[0]->z);
    double dr = fabs(p[1]->r - p[0]->r);
    return sqrt( dz*dz + dr*dr );
  }
};

class Cir: public Edge{
 public:  
  double rad;
  Node center;
  double theta;
  double r,baseRad;
 Cir():Edge(){}
 Cir( Node *_p0, Node *_p1,double cz,double cr, double _theta ):Edge(_p0,_p1){
    theta = _theta;
    rad = theta*M_PI/180;
    center.z = cz;
    center.r = cr;

    double dz = fabs(center.z - p[0]->z);
    double dr = fabs(center.r - p[0]->r);
    baseRad = atan2(p[0]->r-center.r,p[0]->z-center.z);
    r = sqrt(dz*dz+dr*dr);
  }
  double length(){
    return fabs(r*rad);
  }
};


class Params
{
 public:
  int form;
  int firstNodenum;
  int vertexNum;
  string ifname;
  bool isDispNodenum;
  bool isDispBoundary;

  vector< Node > node;
  vector< Element > elem;
  vector< Edge* > edge;

 Params():ifname(""),isDispBoundary(true),isDispNodenum(true){}

  void input(string fname);
  void disp();
  void normalize();

};

#endif // CLASS_H



