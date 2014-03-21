#include "preprocess.h"

void Params::input(string fname)
{
  double dummy;
  int nodenum,elemnum;
  if(fname == ""){
    cout << " file name" << flush << ">>" ;
    cin >> ifname;
  }else{
    ifname = fname;
  }

  // 要素節点関係ファイル
  ifstream ifs((ifname+".elem").c_str());
  if( ifs==NULL ){
    cerr << " cannot found \"" << (ifname+".elem") << "\"" << endl;
    exit(2);
  }else{
    cout << " open \"" << (ifname+".elem") << "\"" << endl;
  }

  ifs >> form;
  ifs >> nodenum;
  ifs >> elemnum;

  node.resize(nodenum);
  elem.resize(elemnum);

  for(int i=0;i<node.size();i++){
    ifs >> node[i].id;
    ifs >> node[i].z;
    ifs >> node[i].r;
  }

  cout << "   node information has been read." << endl;

  int t;
  int surplusNum = 0;
  switch(form){
  case 1: vertexNum = 3; break;
  case 2: vertexNum = 3; surplusNum = 3;break;
  case 3: vertexNum = 4; break;
  case 4: vertexNum = 4; surplusNum = 4;break;
  }
  for(int i=0;i<elem.size();i++){
    ifs >> dummy;
    for(int j=0;j<vertexNum;j++){
      ifs >> t;
      elem[i].p[j] = &node[t-1];
    }
    for(int i=0; i<surplusNum; i++) ifs >> dummy;
  }
  
  cout << "   elem information has been read." << endl;
  
  // 境界形状ファイル
  ifstream ifsbs((ifname+".bs").c_str());
  int sc,edgenum;
  ifsbs >> firstNodenum;
  ifsbs >> edgenum;
  for(unsigned int i=0;i<edgenum;i++){
    ifsbs >> sc;
    double d1,d2;
    if(sc==0){
      ifsbs >> d1;
      ifsbs >> d2;
      edge.push_back(new Str( &node[d1-1], &node[d2-1]));
    }else{
      double cr,cz,theta;
      ifsbs >> d1;
      ifsbs >> d2;
      ifsbs >> cz;
      ifsbs >> cr;
      ifsbs >> theta;
      edge.push_back(new Cir( &node[d1-1], &node[d2-1], cz,cr,theta));
    }
  }
  cout << "   boundary style information has been read." << endl;
}

void Params::normalize()
{  
  double maxz,maxr;
  double minz,minr;
  double rz,rr;
  double square;
  maxz = maxr = DBL_MIN;
  minz = minr = DBL_MAX;

  for(int i=0;i<node.size();i++){
    if(minz > node[i].z) minz = node[i].z;
    if(minr > node[i].r) minr = node[i].r;
    if(maxz < node[i].z) maxz = node[i].z;
    if(maxr < node[i].r) maxr = node[i].r;
  }
  rz = maxz-minz;
  rr = maxr-minr;
  square = (rz>rr)?rz:rr;
  for(int i=0;i<node.size();i++){
    node[i].z = ( (node[i].z - minz) / square )*1.8 - 0.9;
    node[i].r = ( (node[i].r - minr) / square )*1.8 - 0.9;
  }

  for(int i=0;i<edge.size();i++){
    if( typeid(*edge[i]) == typeid(Cir) ){
      Cir *c = dynamic_cast<Cir*>(edge[i]);
      c->center.z = ( (c->center.z - minz) / square )*1.8 - 0.9;
      c->center.r = ( (c->center.r - minr) / square )*1.8 - 0.9;
      c->r /= square;
      c->r *= 1.8;
    }
  }
}

void Params::disp()
{
  cout << "node size : " << node.size() << endl;
  cout << "elem size : " << elem.size() << endl;
  cout << "node size = " << node.size() << endl;
  for(int i=0;i<node.size();i++)
    cout << "node[" << i << "] : " << node[i] << endl;
  cout << "elem size = " << elem.size() << endl;
  for(int i=0;i<elem.size();i++)
    cout << "elem[" << i << "] : " << elem[i] << endl;
  cout << "edge size = " << edge.size() << endl;
  for(int i=0;i<edge.size();i++)
    cout << "edge[" << i << "] : " << *edge[i] << endl;
}

std::ostream& operator<<(std::ostream& os,/*const*/ Node& obj)
{
  return ( os << "(" << obj.z << " , " << obj.r << " ) "  << obj.id );
}

std::ostream& operator<<(std::ostream& os,/*const*/ Element& obj)
{
  return ( os << "( " << obj.p[0]->id << " , " << obj.p[1]->id << " , " << obj.p[2]->id << " , " /*<< obj.p[3]->id << " , " << obj.p[4]->id << " , " << obj.p[5]->id <<*/ " )" );
}

std::ostream& operator<<(std::ostream& os,/*const*/ Edge& obj)
{
  Edge *e = &obj;
  Cir *c;
  os << "EDGE( " << e->p[0]->id << " , " << e->p[1]->id << " ) " << typeid(*e).name();
  if(typeid(obj) == typeid(Cir)){
    c = dynamic_cast<Cir*>(e);
    os << " (" << c->center.z << "," << c->center.r << "," << c->theta << ")" ;
  }
  os << " bc=" << obj.bc << ", val=" << obj.val;
  return os;
} 
