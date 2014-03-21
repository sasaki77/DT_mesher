#include "preprocess.h"
#define BUFF_SIZE 1024

void tokenize(string str,vector<string> &varg)
{
  char minibuff[BUFF_SIZE];
  stringstream args;

  args << str;
  varg.clear();
  while( args.getline(minibuff,BUFF_SIZE,',') )
    varg.push_back((string)minibuff);
}

void DT::input(string &ifname)
{
  ifstream ifs((ifname+".in").c_str());

  if( ifs == NULL ){
    cout << "cannot open \"" << (ifname+".in") << "\"" << endl;
    exit(0);
  }else{
    cout << "open \"" << (ifname+".in") << "\"" << endl;
  }

  char buff[BUFF_SIZE];
  int  read_state = 0;

  while( ifs.getline( buff,BUFF_SIZE ) ){
    if( buff[0] == '#' || buff[0] == '\n' || buff[0] == '\r' || buff[0] == '\0') continue;

    if(      (string)buff == "$begin_params"   ) read_state = 1;
    else if( (string)buff == "$begin_node"     ) read_state = 2;
    else if( (string)buff == "$begin_edge"     ) read_state = 3;
    else if( (string)buff == "$begin_boundary" ) read_state = 4;

    if( read_state == 0 ) continue;

    while( ifs.getline(buff,BUFF_SIZE) ){
      if( buff[0] == '#' || buff[0] == '\n' || buff[0] == '\r' || buff[0] == '\0') continue;

      stringstream     ss;
      vector< string > vs;
      char             minibuff[BUFF_SIZE];

      ss << (string) buff;
      while( ss.getline(minibuff,BUFF_SIZE,' '))
        if( minibuff[0] != '\0')
          vs.push_back( (string) minibuff );

      if( ( (string) buff ).substr(0,4) == "$end") break;

      vector< string > varg; // デリミタ','による分割結果を格納する変数
      list<Node*>      p;    // 新規ノード
      Boundary        *newb; // 新規境界
      static double    scale = 1;

      switch( read_state ){
      case 1: 
        // 事前に必要なパラメータの入力
	if(vs[0] == "$form"){
	  form = atoi( vs[1].c_str() );
	  if(vs.size() > 2 && vs[2] == "-obl")
	    // 偏平率分割が指定されている場合
	    useOblDivide = true;
	}
	if(vs[0] == "$scale")    scale    = atof( vs[1].c_str() );
	if(vs[0] == "$interval") interval = atof( vs[1].c_str() ) * scale;
        break;

      case 2:
        node.push_back( new Node(scale * atof(vs[1].c_str()),
				 scale * atof(vs[2].c_str()),
				 atoi( vs[0].c_str()) ) );
	
        node.at( node.size()-1 )->isOnBnd = true;
        break;

      case 3:
	// 線の種類
        if(vs[3] == "$str"){
          edge.push_back( new StraightEdge( node[atoi(vs[1].c_str())-1],
				   node[atoi(vs[2].c_str())-1] ) );

	}else if(vs[3] == "$cir"){
          tokenize( vs[4],varg );
          edge.push_back( new CircleEdge( node[atoi(vs[1].c_str())-1],
				   node[atoi(vs[2].c_str())-1], 
				   scale * atof(varg[1].c_str()),
				   scale * atof(varg[2].c_str()),
				   atof(varg[3].c_str())));
        }
	// 境界条件
        if(vs[5] == "$dirichlet"){
          edge[edge.size()-1]->bc  = 1;
          edge[edge.size()-1]->val = atof(vs[6].c_str());
        }else if(vs[5] == "$neumann"){
          edge[edge.size()-1]->bc  = 0;
          edge[edge.size()-1]->val = 0;
        }
	
	// 表面の材料
	if(vs[7] == "$Cu" || vs[7] == "$none"){
	  edge[edge.size()-1]->material = vs[7].c_str();
	}
        break;
	
      case 4:
        tokenize( vs[1],varg );

	for(unsigned int i=0; i<varg.size(); i++){
          p.push_back( node[ atoi(varg[i].c_str())-1 ] );
          // ノードと境界順序の設定
          Index id(bnd.size(),i);
          node[ atoi(varg[i].c_str())-1 ]->id.push_back(id);
        }
	
        newb = new Boundary(p,vs[2]=="$true");
        tokenize(vs[3],varg);
        for(unsigned int i=0; i<varg.size(); i++){
          newb->inc_id.push_back( atoi(varg[i].c_str())-1 );
        }
        bnd.push_back( newb );
        break;
      }
    }
  }
  
  firstNodenum = node.size();
}

void DT::memo()
{
  int trioutputnum=0;
  for(unsigned int i=0;i<tri.size();i++)
    if( tri[i]->canExist ) trioutputnum++;
  ofstream ofs("precision.txt", std::ios::out | std::ios::app);
  ofs << node.size()-3 << '\t' << trioutputnum << '\t' // -3 は仮想三角形の節点数を考慮
      << getAveragedLength() << '\t' << getAveragedArea() 
      << '\t' << getAveragedObl() << '\t' << getWorstObl() << '\t' 
      << getNodenumOnCir() << '\t' ;
}

void DT::output_tri1(string ofname)
{
  // 節点要素関係ファイル
  ofstream ofselem((ofname+".elem").c_str());

  ofselem << form << endl;

  int nodenum=0;
  for(unsigned int i=0; i<node.size(); i++)
    if( typeid(*node[i]) == typeid(Node) ) nodenum++;

  int trioutputnum=0;
  for(unsigned int i=0; i<tri.size(); i++)
    if( tri[i]->canExist ) trioutputnum++;
  ofselem << nodenum << endl;
  ofselem << trioutputnum << endl;

  int supernodeweight=0;
  for(unsigned int i=0; i<node.size(); i++){
    if( typeid(*node[i]) == typeid(SuperNode) ){
      supernodeweight++;
      continue;
    }
    
    node[i]->number -= supernodeweight;
    ofselem << node[i]->number << ' ';
    if( fabs(node[i]->x) < OUT_EPS ) ofselem << 0 << ' ';
    else ofselem << node[i]->x << ' ';
    if( fabs(node[i]->y) < OUT_EPS ) ofselem << 0 << endl;
    else ofselem << node[i]->y << endl;
  }

  int tn=0;
  for(unsigned int i=0; i<tri.size(); i++){
    if( tri[i]->canExist ){
      tn++;
      ofselem << tn << ' ' << tri[i]->p[0]->number
	            << ' ' << tri[i]->p[1]->number
	            << ' ' << tri[i]->p[2]->number << endl;
    }
  }

  ofstream ofsbc((ofname+".bc").c_str());
  
  // 境界条件ファイル
  ofsbc << "$begin_bc" << endl;
  for( unsigned int i=0; i<edge.size(); i++){
    if( edge[i]->bc != 0 ){
      edge[i]->p[0]->bc  = edge[i]->p[1]->bc  = edge[i]->bc;
      edge[i]->p[0]->val = edge[i]->p[1]->val = edge[i]->val;
    }
  }
  
  for(unsigned int i=0; i<node.size(); i++)
    if(node[i]->bc == 1)
      ofsbc << node[i]->number << " " << node[i]->val << endl;
  ofsbc << "$end" << endl << endl;
  
  // 材料ファイル
  vector< StraightEdge > surf;
  for(unsigned int i=0; i<edge.size(); i++){
    if( edge[i]->material != "" && edge[i]->material != "$none" ){
      StraightEdge e( edge[i]->p[0], edge[i]->p[1] );
      e.material = edge[i]->material;
      surf.push_back(e);
    }
  }

  ofsbc << "$begin_material" << endl;
  ofsbc << surf.size()       << endl;
  
  for(unsigned int i=0;i<surf.size();i++){
    ofsbc << surf[i].p[0]->number << " "
	  << surf[i].p[1]->number << " " 
	  << surf[i].material << endl;
  }
  ofsbc << "$end" << endl;
}


void DT::output_tri2(string ofname)
{
  vector< Surface > surf;

  // 二次要素の三角形を生成
  for( unsigned int i=0; i<tri.size(); i++){
    if(!tri[i]->canExist) continue;

    for(int j=0; j<3; j++){
      if( tri[i]->p2[j]!=NULL ) continue;
      StraightEdge e( tri[i]->p[j], tri[i]->p[(j+1)%3] );
      Node* new_node = new Node((e.p[0]->x + e.p[1]->x)/2.0,
				(e.p[0]->y + e.p[1]->y)/2.0,
				node.size()+1);
      
      for(unsigned int k=0; k<edge.size(); k++){
        if( e == *edge[k] ){
          new_node->isOnBnd = true;

	  Point p = edge[k]->divPoint(1,1);
          new_node->set(p.x,p.y);

	  // 表面材料が指定してある場合
	  if( edge[k]->material != "" && edge[k]->material != "$none" ){
	    Surface s(edge[k]->p[0], new_node, edge[k]->p[1], edge[k]->material);
	    surf.push_back(s);
	  }

          makeEdges(edge[k], new_node, 2);
        }
      }
      tri[i]->p2[j] = new_node;

      for(int k=0;k<3;k++){
        if(tri[i]->nei[j] == NULL) continue;
        if(tri[i]->nei[j]->nei[k] == tri[i]) tri[i]->nei[j]->p2[k] = new_node;
      }
      node.push_back(new_node);
    }
  }
  
  // 節点要素関係ファイル
  ofstream ofselem((ofname+".elem").c_str());
  ofselem << form << endl;

  int nodenum=0;
  for(unsigned int i=0; i<node.size(); i++)
    if( typeid(*node[i]) == typeid(Node) ) nodenum++;

  int trioutputnum=0;
  for(unsigned int i=0; i<tri.size(); i++)
    if( tri[i]->canExist ) trioutputnum++;
  ofselem << nodenum << endl;
  ofselem << trioutputnum << endl;

  int supernodeweight=0;
  for(unsigned int i=0; i<node.size(); i++){
    if( typeid(*node[i]) == typeid(SuperNode) ){
      supernodeweight++;
      continue;
    }
    
    node[i]->number -= supernodeweight;
    ofselem << node[i]->number << ' ';
    if( fabs(node[i]->x) < OUT_EPS ) ofselem << 0 << ' ';
    else ofselem << node[i]->x << ' ';
    if( fabs(node[i]->y) < OUT_EPS ) ofselem << 0 << endl;
    else ofselem << node[i]->y << endl;

  }

  int tn=0;
  for(unsigned int i=0; i<tri.size(); i++){
    if(tri[i]->canExist){
      tn++;
      ofselem << tn << ' ' << tri[i]->p[0]->number
	            << ' ' << tri[i]->p[1]->number
	            << ' ' << tri[i]->p[2]->number;

      ofselem << ' ' << tri[i]->p2[0]->number
	      << ' ' << tri[i]->p2[1]->number
	      << ' ' << tri[i]->p2[2]->number << endl; 
    }
  }

  cout << " ---\"" << ofname+".elem" << "\" was generated." << endl;

  // 境界条件ファイル
  ofstream ofsbc((ofname+".bc").c_str());
  ofsbc << "$begin_bc" << endl;
  
  for(unsigned int i=0; i<edge.size(); i++){
    if( edge[i]->bc != 0 ){
      edge[i]->p[0]->bc  = edge[i]->p[1]->bc  = edge[i]->bc;
      edge[i]->p[0]->val = edge[i]->p[1]->val = edge[i]->val;
    }
  }
  for(unsigned int i=0; i<node.size(); i++)
    if(node[i]->bc == 1) ofsbc << node[i]->number << " " << node[i]->val << endl;
  ofsbc << "$end" << endl << endl;

  // 材料ファイル
  ofsbc << "$begin_material" << endl;
  ofsbc << surf.size() << endl;
  for(unsigned int i=0;i<surf.size();i++){
    ofsbc << surf[i].p[0]->number << " " << surf[i].p[1]->number << " " 
	  << surf[i].p[2]->number << " " << surf[i].material << endl;
  }
  ofsbc << "$end" << endl;

  cout << " ---\"" << ofname+".bc" << "\" was generated." << endl;
}

void DT::output_quad1(string){ 
  // 節点要素関係ファイル
  ofstream ofselem((ofname+".elem").c_str());
  ofselem << form << endl;

  int nodenum=0;
  for(unsigned int i=0; i<node.size(); i++)
    if( typeid(*node[i]) == typeid(Node) ) nodenum++;
  int quadoutputnum=0;

  ofselem << nodenum     << endl;
  ofselem << quad.size() << endl;

  int supernodeweight=0;
  for(unsigned int i=0; i<node.size(); i++){
    if( typeid(*node[i]) == typeid(SuperNode) ){
      supernodeweight++;
      continue;
    }
    
    node[i]->number -= supernodeweight;
    ofselem << node[i]->number << ' ';
    if( fabs(node[i]->x) < OUT_EPS ) ofselem << 0 << ' ';
    else ofselem << node[i]->x << ' ';
    if( fabs(node[i]->y) < OUT_EPS ) ofselem << 0 << endl;
    else ofselem << node[i]->y << endl;
  }

  int qn=0;
  for(unsigned int i=0; i<quad.size(); i++){
    qn++;
    ofselem << qn << ' ' << quad[i]->p[0]->number
	          << ' ' << quad[i]->p[1]->number
	          << ' ' << quad[i]->p[2]->number
	          << ' ' << quad[i]->p[3]->number << endl; 
  }


  // 境界条件ファイル
  ofstream ofsbc((ofname+".bc").c_str());
  ofsbc << "$begin_bc" << endl;
  for(unsigned int i=0; i<edge.size(); i++){
    if( edge[i]->bc != 0 ){
      edge[i]->p[0]->bc   = edge[i]->p[1]->bc = edge[i]->bc;
      edge[i]->p[0]->val = edge[i]->p[1]->val = edge[i]->val;
    }
  }
  for(unsigned int i=0; i<node.size(); i++)
    if(node[i]->bc == 1) ofsbc << node[i]->number << " " << node[i]->val << endl;
  ofsbc << "$end" << endl << endl;

  // 材料ファイル
  vector< StraightEdge > surf;
  for(unsigned int i=0; i<edge.size(); i++){
    if(edge[i]->material != "" && edge[i]->material != "$none"){
      StraightEdge e(edge[i]->p[0],edge[i]->p[1]);
      e.material = edge[i]->material;
      surf.push_back(e);
    }
  }
  
  ofsbc << "$begin_material" << endl;
  ofsbc << surf.size()       << endl;
  for(unsigned int i=0; i<surf.size(); i++){
    ofsbc << surf[i].p[0]->number << " " << surf[i].p[1]->number << " " 
	  << surf[i].material << endl;
  }
  ofsbc << "$end" << endl;
}

void DT::output_quad2(string){
  vector< Surface > surf;

  // ２次要素の四角形を生成
  for(unsigned int i=0; i<quad.size(); i++){
    for(int j=0; j<4; j++){
      // 節点を置く位置を決定
      StraightEdge e(quad[i]->p[j], quad[i]->p[(j+1)%4]);
      Node* new_node = new Node((e.p[0]->x + e.p[1]->x)/2.0,
				(e.p[0]->y + e.p[1]->y)/2.0,
				node.size()+1);
      
      // 境界で合った場合中点に移動（円形での処理のため）
      for(unsigned int k=0; k<edge.size(); k++){
        if( e == *edge[k]){
          new_node->isOnBnd = true;
          Point p = edge[k]->divPoint(1,1);
          new_node->set(p.x,p.y);

	  // 表面材料が指定してある場合
	  if( edge[k]->material != "" && edge[k]->material != "$none" ){
	    Surface s(edge[k]->p[0], new_node, edge[k]->p[1], edge[k]->material);
	    surf.push_back(s);
	  }

          makeEdges(edge[k],new_node,2);
        }
      }

      // 点が重なっている場合の処理
      int NodeSetedNum = -1;
      for(int k=0; k < node.size(); k++){
	if( fabs(new_node->x - node[k]->x) < CALC_EPS &&
	    fabs(new_node->y - node[k]->y) < CALC_EPS ){
	  NodeSetedNum = k;
	  break;
	}
      }
      
      if(NodeSetedNum >0){
	delete new_node;
	quad[i]->p2[j] = node[NodeSetedNum];
	continue;
      }

      quad[i]->p2[j] = new_node;

      node.push_back(new_node);
    }
  }
  
  // 節点要素関係ファイル
  ofstream ofselem((ofname+".elem").c_str());
  ofselem << form << endl;
  int nodenum=0;
  for(unsigned int i=0; i<node.size(); i++)
    if( typeid(*node[i]) == typeid(Node) ) nodenum++;
  int quadoutputnum=0;

  ofselem << nodenum     << endl;
  ofselem << quad.size() << endl;

  int supernodeweight=0;
  for(unsigned int i=0; i<node.size(); i++){
    if( typeid(*node[i]) == typeid(SuperNode) ){
      supernodeweight++;
      continue;
    }
    node[i]->number -= supernodeweight;
    ofselem << node[i]->number << ' ';
    if( fabs(node[i]->x) < OUT_EPS ) ofselem << 0 << ' ';
    else ofselem << node[i]->x << ' ';
    if( fabs(node[i]->y) < OUT_EPS ) ofselem << 0 << endl;
    else ofselem << node[i]->y << endl;
  }

  int qn=0;
  for(unsigned int i=0;i<quad.size();i++){
    qn++;
    ofselem << qn << ' ' << quad[i]->p[0]->number
	          << ' ' << quad[i]->p[1]->number
	          << ' ' << quad[i]->p[2]->number
	          << ' ' << quad[i]->p[3]->number; 

    ofselem << ' ' << quad[i]->p2[0]->number
	    << ' ' << quad[i]->p2[1]->number
	    << ' ' << quad[i]->p2[2]->number
	    << ' ' << quad[i]->p2[3]->number
	    << endl; 
  }

  cout << " ---\"" << ofname+".elem" << "\" was generated." << endl;

  // 境界条件ファイル
  ofstream ofsbc((ofname+".bc").c_str());
  ofsbc << "$begin_bc" <<endl;
  for(unsigned int i=0; i<edge.size(); i++){
    if( edge[i]->bc != 0 ){
      edge[i]->p[0]->bc  = edge[i]->p[1]->bc  = edge[i]->bc;
      edge[i]->p[0]->val = edge[i]->p[1]->val = edge[i]->val;
    }
  }
  for(unsigned int i=0; i<node.size(); i++)
    if(node[i]->bc == 1) ofsbc << node[i]->number << " " << node[i]->val << endl;
  ofsbc << "$end" << endl << endl;

  // 材料ファイル
  ofsbc << "$begin_material" << endl;
  ofsbc << surf.size() << endl;
  for(unsigned int i=0;i<surf.size();i++){
    ofsbc << surf[i].p[0]->number << " "
	  << surf[i].p[1]->number << " " 
	  << surf[i].p[2]->number << " "
	  << surf[i].material     << endl;
  }
  ofsbc << "$end" << endl;

  cout << " ---\"" << ofname+".bc" << "\" was generated." << endl;
}

void DT::output(string &ofname)
{
  switch(form){
  case 1: output_tri1(ofname); break;
  case 2: output_tri2(ofname); break;
  case 3: output_quad1(ofname); break;
  case 4: output_quad2(ofname); break;
  }

  // femvisのための境界形状の出力
  ofstream ofsbs((ofname+".bs").c_str());
  ofsbs << firstNodenum << endl;
  ofsbs << edge.size()  << endl;
  
  for(unsigned int i=0; i<edge.size(); i++){
    if( typeid(*edge[i]) == typeid(CircleEdge) ){
      CircleEdge *c = dynamic_cast<CircleEdge*>(edge[i]);
      ofsbs << 1 << '\t' << c->p[0]->number
	         << '\t' << c->p[1]->number
	         << '\t' << c->center.x
	         << '\t' << c->center.y
	         << '\t' << c->theta
	    << endl;
    }else{
      StraightEdge *s = dynamic_cast<StraightEdge*>(edge[i]);
      ofsbs << 0 << '\t' << s->p[0]->number << '\t' << s->p[1]->number << endl;
    }
  }
}

void DT::disp()
{
  cout << "===== node =====" << endl;
  cout << "size: " << node.size() << endl;
  for(unsigned int i=0;i<node.size();i++){
    cout << *node[i] << endl;
  }
  cout << "===== Tri =====" << endl;
  cout << "size: " << tri.size() << endl;
  for(unsigned int i=0;i<tri.size();i++){
    cout << *tri[i] << endl;
  }
  cout << "===== Quad =====" << endl;
  cout << "size: " << quad.size() << endl;
  for(unsigned int i=0;i<quad.size();i++){
    cout << *quad[i] << endl;
  }
  cout << "===== edge =====" << endl;
  cout << "size: " << edge.size() << endl;
  for(unsigned int i=0;i<edge.size();i++){
    cout << *edge[i] << endl;
    cout << " length:" << edge[i]->length() << endl;
  }
  cout << "===== bnd =====" << endl;
  cout << "size: " << bnd.size() << endl;
  for(unsigned int i=0;i<bnd.size();i++){
    cout << *bnd[i] << endl;
  }
}

std::ostream& operator<<(std::ostream& os,/*const*/ Node& obj)
{
  os << "NODE( " << obj.x << " , " << obj.y << " ) " << obj.number << " " << obj.isOnBnd
     << " " << obj.isPlaced;
  if( typeid(obj) == typeid(SuperNode) ) os << " *" ;
  os << endl;
  os << " id: ";
  for(unsigned int i=0;i<obj.id.size();i++) os << obj.id[i].bnd << "," << obj.id[i].order << " / " ; 
  return os;
}

std::ostream& operator<<(std::ostream& os,/*const*/ Tri& obj)
{
  os << "TRI( " ;
  for(int i=0;i<3;i++){
    os << obj.p[i]->number;
    if( typeid(*(obj.p[i])) == typeid(SuperNode) ) os << "*";
    os << " ";
  }
  os << ") " << obj.canExist ;
  return os;
} 

std::ostream& operator<<(std::ostream& os,/*const*/ Edge& obj)
{
  Edge *e = &obj;
  CircleEdge *c;
  os << "EDGE( " << obj.p[0]->number << " , " << obj.p[1]->number << " ) " << typeid(obj).name();
  if(typeid(obj) == typeid(CircleEdge)){
    c = dynamic_cast<CircleEdge*>(e);
    os << " (" << c->center.x << "," << c->center.y << "," << c->theta << ")" ;
  }
  os << " bc=" << obj.bc << ", val=" << obj.val;
  return os;
} 

std::ostream& operator<<(std::ostream& os,/*const*/ Boundary& obj)
{
  os << "BND( " ;
  list< Node* >::iterator itr = obj.p.begin();
  while( itr != obj.p.end() ){
    os << (*itr)->number << " " ;
    itr++;
  }
  os << ") " << endl;;
  if(obj.isOuter) os << "Outer Boundary " ;
  else            os << "Inner Boundary " ;
  os << "inc( ";
  for(unsigned int i=0;i<obj.inc_id.size();i++)
    os << obj.inc_id[i] << " " ;
  os << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os,/*const*/ Quad& obj)
{
  os << "QUAD( " ;
  for(int i=0;i<4;i++){
    os << obj.p[i]->number;
    if( typeid(*(obj.p[i])) == typeid(SuperNode) ) os << "*";
    os << " ";
  }
  os << ")";
  return os;
} 
