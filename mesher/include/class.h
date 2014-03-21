//==================================================
//　クラス
//==================================================

#ifndef CLASS_H
#define CLASS_H

#include "common.h"

//========== クラス宣言 =========
class Point;
class Index;
class Node;
class SuperNode;
class Edge;
class StraightEdge;
class CircleEdge;
class Tri;
class Boundary;
class DT;
class Quad; //四角形用
//===============================

double dist(Point*,Point*);

//========== クラス定義 ==========

class Point{
public:
  double x,y;

  Point():x(0),y(0){}
  Point( double _x, double _y ):x(_x),y(_y){}

  void set(double _x, double _y){
    x = _x;
    y = _y;
  }
};

class Index{
public:
  int bnd;
  int order;

  Index(int _bnd,int _order){
    bnd = _bnd;
    order = _order;
  }
};

class Node: public Point{
public:
  int    number;
  int    bc;
  double val;
  bool   isOnBnd;
  bool   isPlaced;

  vector< Index > id;
  vector< Tri* >  tsp;
  vector< Quad* > share_quad; //ノードを共有する四角形要素へのポインタ

 Node():Point(){
    isOnBnd  = false;
    isPlaced = false;
    number   = -1;
    bc       = 0;
  }
  
  Node( double _x, double _y, int _number):Point(_x,_y){
    number   = _number;
    isOnBnd  = false;
    isPlaced = false;
    bc=0;
  }
  bool operator==( const Node& obj ){ return ( number == obj.number ); }
  bool operator!=( const Node& obj ){ return ( number != obj.number ); }

  virtual void dummy_func(){}; // 実行時型情報を有効にするための仮想関数

  //====四角形要素のための関数====
  void set_num(int _number){
    number = _number;
  }
};

class SuperNode: public Node{
public:
  SuperNode( double _x,double _y,int _number ):Node(_x,_y,_number){}
};

class Edge{
public:
  Node   *p[2];
  int    bc;
  double val;
  string material;

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
  virtual Point  divPoint(int n,int i) = 0;

  bool operator==( const Edge& obj ){
    return ( (p[0] == obj.p[0] && p[1] == obj.p[1]) || (p[0] == obj.p[1] && p[1] == obj.p[0]) );
  }

  bool operator!=( const Edge& obj ){
    return ( !((p[0] == obj.p[0] && p[1] == obj.p[1]) || (p[0] == obj.p[1] && p[1] == obj.p[0])) );
  }
};

class StraightEdge: public Edge{
 public:
  StraightEdge():Edge(){}

  StraightEdge( Node *_p0, Node *_p1 ):Edge(_p0,_p1){}
  double length(){
    double dx = fabs(p[1]->x - p[0]->x);
    double dy = fabs(p[1]->y - p[0]->y);
    return sqrt( dx*dx + dy*dy );
  }
  
  Point divPoint(int n,int i){
    if(i>n || i==0) cerr << "error divPoint()" << endl;
    Point retP;
    retP.x = ( (n-i+1) * p[0]->x + i * p[1]->x) / (n+1);
    retP.y = ( (n-i+1) * p[0]->y + i * p[1]->y) / (n+1);
    return retP;
  }
};

class CircleEdge: public Edge{
public:
  double rad;
  Point  center;
  double theta;
  double r,baseRad;

  CircleEdge():Edge(){}

  CircleEdge( Node *_p0, Node *_p1,double cx,double cy, double _theta ):Edge(_p0,_p1){
    theta = _theta;
    rad = theta*M_PI/180;
    center.x = cx;
    center.y = cy;

    double dx = fabs(center.x - p[0]->x);
    double dy = fabs(center.y - p[0]->y);
    baseRad = atan2(p[0]->y-center.y,p[0]->x-center.x);
    r = sqrt(dx*dx+dy*dy);
    // エラーチェック 格納情報に矛盾がないか検査
    /*
    if( !EQDBL_CIR(center.x+r*cos(baseRad-rad),p[1]->x)
      || !EQDBL_CIR(center.y+r*sin(baseRad-rad),p[1]->y) ){
        cerr << "error in class:CircleEdge " << endl;
        cerr << "p[0] : " << setprecision(15) << p[0]->x << " , " << setprecision(15) << p[0]->y << endl;
        cerr << "p[1] : " << setprecision(15) << p[1]->x << " , " << setprecision(15) << p[1]->y << endl;
        cerr << "rad : " << setprecision(15) << rad << endl;
        cerr << "baseRad : " << setprecision(15) << baseRad << endl;
	cerr << "center( " << setprecision(15) << cx << " , " << cy << " )" << endl;
        cerr << "r : " << setprecision(15) << r << endl;
        cerr << setprecision(15) << center.x+r*cos(baseRad-rad) << endl;
        cerr << setprecision(15) << center.y+r*sin(baseRad-rad) << endl;
    }	
    */
  }
  
  double length(){
    return fabs(r*rad);
  }
  
  Point divPoint(int n,int i){
    if(i>n || i==0) cerr << "error divPoint()" << endl;
    Point retP;
    retP.x = center.x + r * cos( baseRad - rad * i / (n+1) );
    retP.y = center.y + r * sin( baseRad - rad * i / (n+1) );
    return retP;
  }
};

class Tri{
public:
  Node   *p[3];
  Node   *p2[3];
  Tri    *nei[3];
  bool   canExist;
  double obl,area;

  Tri(){
    for(int i=0;i<3;i++){
      p[i]     = NULL;
      p2[i]    = NULL;
      nei[i]   = NULL;
      canExist = false;
    }
  }
  
  double length(int p0,int p1){
    StraightEdge e(p[p0],p[p1]);
    return e.length();
  }
  
  void calcParms()
  {
    // 余弦定理により内角を計算
    double       theta[3];
    StraightEdge e[3];
    double       max,a,b,c;

    for(int i=0;i<3;i++) e[i].set( p[i], p[(i+1)%3] );

    a = e[0].length();
    b = e[1].length();
    c = e[2].length();

    theta[0] = acos( (b*b + c*c - a*a)/(2*b*c) );
    theta[1] = acos( (c*c + a*a - b*b)/(2*c*a) );
    theta[2] = acos( (a*a + b*b - c*c)/(2*a*b) );

    max = DBL_MIN;
    for(int i=0;i<3;i++)
      if(max<theta[i])
        max = theta[i];
    obl = max;

    // 外積により面積を計算
    Point A,B;
    A.set( p[2]->x - p[0]->x,p[2]->y - p[0]->y );
    B.set( p[1]->x - p[0]->x,p[1]->y - p[0]->y );
    area = 0.5*fabs(A.x*B.y - A.y*B.x);
  }
  
  void set(Node *p0,Node *p1,Node *p2){
    p[0] = p0;
    p[1] = p1;
    p[2] = p2;
  }
  
  void setNei( Tri *t0,Tri *t1,Tri *t2 ){
    nei[0] = t0;
    nei[1] = t1;
    nei[2] = t2;
  }

  //====四角形要素分割のための関数(cf.quad_model.cpp)====
  double       getArea();
  StraightEdge getLongestEdge();
  StraightEdge getMiddleEdge();
  StraightEdge getShortestEdge();
  int          getPointPlace(Node *np);
  Point        getCenterPoint();
};

class Surface{
 public:
  Node*  p[3];
  string material;

  Surface(Node *p0, Node *p1, Node *p2,string m){
    p[0] = p0;
    p[1] = p1;
    p[2] = p2;
    material = m;
  }
};

class Boundary{
public:
  list< Node* > p;
  vector< int > inc_id;
  bool          isOuter;
  
  Boundary( list< Node* > _p , bool _isOuter){
    p = _p;
    isOuter = _isOuter;
  }
};

class DT{
 private:
  double maxx, maxy, minx, miny, square; //for denormalize
  int    firstNodenum; // 入力ノード数
  double interval; //四角形要素分割用の格子点間隔
  
 public:

  string ifname;
  string ofname;
  int    form;
  bool   useOblDivide; // 四角形要素分割の際に偏平率分割をするか否か

  vector< Node* >     node;
  vector< Tri*  >     tri;
  vector< Quad* >     quad;
  vector< Edge* >     edge;
  vector< Boundary* > bnd;

  DT(){
    interval = -1;
    useOblDivide = false;
  }

  ~DT(){
    for(unsigned int i=0;i<node.size();i++)
      if(node[i] != NULL) delete node[i];
    for(unsigned int i=0;i<tri.size();i++)
      if(tri[i] != NULL) delete tri[i];
    for(unsigned int i=0;i<edge.size();i++)
      if(edge[i] != NULL) delete edge[i];
    for(unsigned int i=0;i<bnd.size();i++)
      if(bnd[i] != NULL) delete bnd[i];
  }

  void input(string &);
  void output(string &);
  void output_tri1(string);
  void output_tri2(string);
  void output_quad1(string);
  void output_quad2(string);
  void memo();
  void disp();

  void model();

  void normalize();
  void denormalize();
  void setSuperTriangle();
  void generateBnd();
  void generateFineMesh();
  void makeEdges(Edge*&,Node*,int);
  void updateBnd(Edge*,Node*);
  void laplace();
  void div3Tri(Tri *[3],Tri *, Node *);
  void div4Tri(Tri *[4],Tri *, Tri *, Node *, int);
  vector< Tri* > swapingAlg( stack< Tri* > *, Node *);
  void swapTri(Tri *tA,Tri *tB, int diagPtA, int diagPtB );
  void removeIllegalTri();
  
  double getWorstObl()
  {
    double maxobl = DBL_MIN;
    
    for(unsigned int i=0; i<tri.size(); i++){
      if(!tri[i]->canExist) continue;
      tri[i]->calcParms();
      if(maxobl < tri[i]->obl) maxobl = tri[i]->obl;
    }
    
    maxobl = maxobl/ M_PI * 180;
    return maxobl;
  }
  
  double getAveragedObl()
  {
    double sum = 0;
    int    cnt = 0;
    
    for(unsigned int i=0;i<tri.size();i++){
      if(!tri[i]->canExist) continue;
      cnt++;
      tri[i]->calcParms();
      sum += tri[i]->obl;
    }
    
    sum /= (double)cnt;
    sum  = sum / M_PI *180;
    return sum;
  }
  
  double getAveragedArea()
  {
    double sum = 0;
    int    cnt = 0;

    for(unsigned int i=0; i<tri.size(); i++){
      if(!tri[i]->canExist) continue;
      cnt++;
      tri[i]->calcParms();
      sum += tri[i]->area;
    }
    
    sum /= (double)cnt;
    return sum;
  }
  
  double getAveragedLength()
  {
    double sum = 0;
    int    cnt = 0;

    for(unsigned int i=0; i<tri.size(); i++){
      if( !tri[i]->canExist ) continue;
      cnt++;
      sum += ( dist( tri[i]->p[0], tri[i]->p[1] )
	     + dist( tri[i]->p[1], tri[i]->p[2] )
	     + dist( tri[i]->p[2], tri[i]->p[0] ) ) / 3;
    }
    
    sum /= (double)cnt;
    return sum;
  }
  double getNodenumOnCir()
  {
    vector<int> a;
    bool        p0isOK, p1isOK;

    for(unsigned int i=0; i<edge.size(); i++){
      if( typeid(*edge[i]) != typeid(CircleEdge) ) continue;

      p0isOK = p1isOK = true;

      for(unsigned int j=0; j<a.size(); j++){
	if( a[j] == edge[i]->p[0]->number ) p0isOK = false;
	if( a[j] == edge[i]->p[1]->number ) p1isOK = false;
      }
      if( p0isOK ) a.push_back( edge[i]->p[0]->number );
      if( p1isOK ) a.push_back( edge[i]->p[1]->number );
    }
    return a.size();
  }

  //---四角形分割のための関数-----
  void cirEdgeDivide(double);
  void generateFineMesh_quad();
  void TriToQuad();
  void laplaceQuad();
  void getPolyCenter(vector< Quad* > share_quad,Node *np);
  void normalizePoint(Node *normalize_node);
  void moveNewCirPoint(Edge*&, Node *);
  void makeEdges2(Edge*&, Node *);
  bool canExistOnCirEdge(Edge*&, Node *,Node *);
  bool canExistInTri(Tri *,Node *);
  Tri *seachCirTri(Node *);
  void addNodeForBnd();
  void makeQuad();
  void setTriMidNode();
  void generateBnd_Quad();
};

class Quad{
 public:
  Node   *p[4];
  Node   *p2[4];
  double area_sum;

  Quad(){
    p[0] = p[1] = p[2] = p[3] = NULL;
    area_sum = 0;
  }

  Quad( Node *p0, Node *p1, Node *p2, Node *p3 ){
    p[0] = p0;
    p[1] = p1;
    p[2] = p2;
    p[3] = p3;
  }
  
  void set( Node *p0, Node *p1, Node *p2, Node *p3 ){
    p[0] = p0;
    p[1] = p1;
    p[2] = p2;
    p[3] = p3;
  }

  Point getCenterPoint(){
    double x,y;
    Tri    divtri[2];
    double sum_tri[2],sum_all;

    divtri[0].set(p[0],p[1],p[2]);
    divtri[1].set(p[2],p[3],p[0]);

    sum_tri[0] = divtri[0].getArea();
    sum_tri[1] = divtri[1].getArea();
    sum_all    = sum_tri[0] + sum_tri[1];

    Point pointc_tri[2];
    pointc_tri[0] = divtri[0].getCenterPoint();
    pointc_tri[1] = divtri[1].getCenterPoint();

    x = ( sum_tri[0] * pointc_tri[0].x + sum_tri[1] * pointc_tri[1].x) / (sum_all);
    y = ( sum_tri[0] * pointc_tri[0].y + sum_tri[1] * pointc_tri[1].y) / (sum_all);

    Point pointc(x,y);

    return pointc;
  }

  double getArea(){
    Point vnode[4];
    double sum;
    vnode[0].set( p[0]->x - p[1]->x ,p[0]->y - p[1]->y );
    vnode[1].set( p[2]->x - p[1]->x ,p[2]->y - p[1]->y );
    vnode[2].set( p[3]->x - p[2]->x ,p[3]->y - p[2]->y );
    vnode[3].set( p[1]->x - p[2]->x ,p[1]->y - p[2]->y );

    sum = fabs((vnode[0].x * vnode[1].y - vnode[0].y * vnode[1].x)) /2 +
          fabs((vnode[2].x * vnode[3].y - vnode[2].y * vnode[3].x)) /2; 

    return sum;
  }

  bool operator==( const Quad& obj ){
    return ( p[0] == obj.p[0] && p[1] == obj.p[1] &&
	     p[2] == obj.p[2] && p[3] == obj.p[3] );
  }
  bool operator!=( const Quad& obj ){
    return !( p[0] == obj.p[0] && p[1] == obj.p[1] &&
	      p[2] == obj.p[2] && p[3] == obj.p[3]);
  }
};

#endif // CLASS_H
