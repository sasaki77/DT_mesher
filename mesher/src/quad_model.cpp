#include "preprocess.h"

Tri *searchTri(Tri *tBase,Node *p);
double dist( double x1, double y1, double x2, double y2);
double getDet(double x0,double y0,double x1,double y1,double x2,double y2);
bool isCrossed( StraightEdge e1, StraightEdge e2 );

//============================================================
// 関数名：generateFineMesh_quad() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// 節点を設定されたintervalの間隔で自動追加してメッシュの細分割を行う
//============================================================
void DT::generateFineMesh_quad()
{
  // 追加節点数
  int addnum_x = fabs(maxx - minx)/interval +1; 
  int addnum_y = fabs(maxy - miny)/interval +1; 
  
  int edgeNodeSize = node.size(); // はじめに境界を形成する点の数 

  for(int i=0; i < addnum_x; i++){
    for(int j=0; j < addnum_y; j++){
      double x = minx + i*interval;
      double y = miny + j*interval;

      // 新しく設定する点の生成
      Node *np = new Node(x,y,node.size() + 1);
      normalizePoint(np);

      bool canset = true;
      //境界の点の近くにあるかを確認.あったらcontinue
      for(int k=0; k < edgeNodeSize; k++){
	if(fabs(np->x - node[k]->x) < CALC_EPS && 
	   fabs(np->y - node[k]->y) < CALC_EPS) 
	  canset = false;
      }
      
      // 点が置けなかった場合生成した点をdelete,continue
      if(!canset) {
	delete(np); 
	continue;
      }

      // 点を含む、又は点を境界にもつ三角形の設定
      Tri *placed_tri; 
      placed_tri = searchTri(tri[0],np);

      // 点が内部にあるか、辺上にあるか.内部にある場合:pIndex<0.境界の場合その辺の番号:0~2
      int pIndex; 
      pIndex = placed_tri->getPointPlace(np);

      // 点が境界上でかつ、その三角形が存在できない三角形の場合
      // 隣の三角形にplaced_triを移し、再度pIndexを設定
      if(pIndex != -1 && !placed_tri->canExist){
	placed_tri = placed_tri->nei[pIndex];
	pIndex = placed_tri->getPointPlace(np);
      }
      
      //置いた点が存在出来ない三角形なら無視
      if(!placed_tri->canExist) {
	delete(np);
	continue;
      }

      // 円境界で存在出来る点か判別
      // その三角形に円境界がある場合その点が存在出来るかを判別
      if( !canExistInTri(placed_tri,np)) {
	delete(np);
	continue;
      }

      //境界上における境界線の追加
      if(pIndex >= 0){
	StraightEdge e;
	e.set( placed_tri->p[pIndex] , placed_tri->p[(pIndex+1)%3] );
	for(  int k=0;k<edge.size();k++ ){
	  if( e == *edge[k] ){
	    canset = false;
// 	    np->isOnBnd = true;
// 	    //新しい境界を生成
// 	    updateBnd(edge[k],np);
// 	    //円の場合の処理
// 	    if( typeid(*edge[k]) == typeid(CircleEdge) ){
// 	      //円境界に点を移動
// 	      //moveNewCircleEdgePoint(edge[k],np);
// 	    }
// 	    //制約線分の新規作成
// 	    makeEdges2( edge[k],np );
	    break;
	  }
	}
      }

      // 点が置けなかった場合生成した点をdelete,continue
      if(!canset) {
	delete(np); 
	continue;
      }     
      

      node.push_back(np);

      //三角形分割
      if(pIndex < 0){
	Tri *ts[3];
	stack< Tri* > triStack;
	div3Tri( ts, placed_tri, np);
	for(int k=0;k<3;k++) triStack.push( ts[k] );
	swapingAlg( &triStack, np );
      }else{
	Tri *ts[4];
	stack< Tri* > triStack;
	div4Tri( ts, placed_tri, placed_tri->nei[pIndex], np, pIndex );
	for(int k=0;k<4;k++) triStack.push( ts[k] );
	swapingAlg( &triStack, np );
      }
    }
  }
}

//============================================================
// 関数名：setTriMidNode() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// 三角形から四角形を生成する際に必要となる各辺の中点を生成する.
//============================================================
void DT::setTriMidNode()
  {
    for(int i=0; i<tri.size(); i++){
      if(!tri[i]->canExist) continue;
      for(int j=0; j<3; j++){
	// すでに中点が設定されていれば無視
	if( tri[i]->p2[j] != NULL ) continue;

	// 中点に新しい点を設定
	StraightEdge e(tri[i]->p[j],tri[i]->p[(j+1)%3]);
	Node* new_node = new Node((e.p[0]->x + e.p[1]->x)/2.0, (e.p[0]->y + e.p[1]->y)/2.0,node.size()+1);

	// 新しい点が境界上であれば位置を再設定し、新しい境界を生成
	// (境界が円境界である場合、位置が変わってくるため再設定している)
	for(unsigned int k=0;k<edge.size();k++){
	  if( e == *edge[k]){
	    new_node->isOnBnd = true;
	    Point p = edge[k]->divPoint(1,1);
	    new_node->set(p.x,p.y);
	    updateBnd(edge[k],new_node);
	    makeEdges(edge[k],new_node,2);
	  }
	}
      
	tri[i]->p2[j] = new_node;
      
	// 新しく設定した点を共有する他の三角形があれば、その三角形に点を設定する
	for(int k=0;k<3;k++){
	  if(tri[i]->nei[j] == NULL) continue;
	  if(tri[i]->nei[j]->nei[k] == tri[i]) tri[i]->nei[j]->p2[k] = new_node;
	}
	node.push_back(new_node);
      }
    }
  }

//============================================================
// 関数名：makeQuad() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// 始め最長辺を共有している三角形によって四角形を生成する.
// その後,最長辺と2番目に長い辺を共有している三角形によって四角形を生成する.
//============================================================
void DT::makeQuad()
{
  // for文のlを変えると合成する四角形の条件が変わる.
  // l = 0 の時：最長辺と最長辺
  // l = 1 の時：最長辺と二番目に長い辺
  // l = 2 の時：二番目に長い辺と一番短い辺
  for(int l=0; l<1; l++){
    for(int i=0; i < tri.size(); i++){
      if(tri[i]->canExist == false) continue;

      StraightEdge e1;
      if(l==0 || l ==1)
	e1 = tri[i]->getLongestEdge(); //三角形の最長辺を取得
      else
	e1 = tri[i]->getMiddleEdge();

      bool edge_flag = false;
      for(int j=0; j < edge.size(); j++){
	if(e1 == *edge[j]) {
	  edge_flag = true;
	  break;
	}
      }
      if(edge_flag) continue;

      // 最長辺を共有する三角形の取得
      int neiIndex = 0; //1~3:最長辺を共有する三角形
      for(int j=0; j < 3; j++ ){
	if(tri[i]->p[j] == e1.p[0]) {
	  neiIndex = j;
	  break;
	}
      }
      Tri *neiTri = tri[i]->nei[neiIndex];
      if(neiTri->canExist == false) continue;
      StraightEdge e2;
      if(l==0)
	e2 = neiTri->getLongestEdge(); //最長辺を共有する三角形の最長辺を取得
      else if(l==1 || l==2)
	e2 = neiTri->getMiddleEdge();
      else
	e2 = neiTri->getShortestEdge();

      if(e1 == e2){
	//Node *p1,*p4; //p1:tri[i]の最長辺ではない点,p4:p1の対角の点
	int numP1,numP4;
      
	//p1の捜索
	for(int j=0; j<3; j++){
	  if(e1.p[0] == tri[i]->p[j] && e1.p[1] == tri[i]->p[(j+1)%3]){
	    //p1 = tri[i]->p[(j+2)%3];
	    numP1 = (j+2)%3;
	    break;
	  }
	}

	//p4の捜索
	for(int j=0; j<3; j++){
	  if(e2.p[0] == neiTri->p[j] && e2.p[1] == neiTri->p[(j+1)%3]){
	    //p4 = neiTri->p[(j+2)%3];
	    numP4 = (j+2)%3;
	    break;
	  }
	}
	
	// 移動させる重心の点の参照
	Quad largeQuad(tri[i]->p[numP1], e1.p[0], neiTri->p[numP4], e1.p[1]); //４つに分割する前の大きな四角形
	Point cp = largeQuad.getCenterPoint();
	Node *cnp = tri[i]->p2[(numP1 + 1)%3];
	
	// e1の最長辺が境界であれば無視
	// 	  bool edge_flag = false;
	// 	  StraightEdge e0(e1.p[0],cnp);
	// 	  for(int j=0; j < edge.size(); j++){
	// 	    if(e0 == *edge[j]) {
	// 	      edge_flag = true;
	// 	      break;
	// 	    }
	// 	  }
	// 	  if(edge_flag) continue;
	
	// 重心の点の移動
	cnp->set(cp.x,cp.y);
	
	// ４つの四角形の生成
	Quad *newQuad[4];
	
	newQuad[0] = new Quad(tri[i]->p[numP1],tri[i]->p2[numP1],cnp,tri[i]->p2[(numP1+2)%3]);
	newQuad[1] = new Quad(tri[i]->p2[numP1],e1.p[0],neiTri->p2[(numP4+2)%3],cnp);
	newQuad[2] = new Quad(neiTri->p[numP4],neiTri->p2[numP4],cnp,neiTri->p2[(numP4+2)%3]);
	newQuad[3] = new Quad(tri[i]->p2[(numP1+2)%3],cnp,neiTri->p2[numP4],e1.p[1]);
	
	for(int j=0; j<4; j++){
	  quad.push_back( newQuad[j] );
	}
	
	tri[i]->canExist =  neiTri->canExist =  false;
      }
    }
  }
}  

//============================================================
// 関数名：TriToQuad() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// makeQuad()で四角形に変換されなかった三角形を
// 重心と各辺の中点を利用して３つの三角形に変換する
//============================================================
void DT::TriToQuad()
{
  for(int i=0; i < tri.size(); i++){
    if(tri[i]->canExist == false) continue;
    
    //tri[i]の重心のノードを設定
    Point cp = tri[i]->getCenterPoint();
    Node *cnp = new Node(cp.x, cp.y, node.size());
    node.push_back( cnp );
    cnp->set_num( node.size() );

    //三角形を３つの四角形に変換
    for(int j=0; j<3; j++){
      Quad *newQuad = new Quad( tri[i]->p[ j%3 ], tri[i]->p2[j%3], cnp, tri[i]->p2[(j+2)%3]);
      quad.push_back( newQuad );
    }

    tri[i]->canExist = false;
  }
}

//============================================================
// 関数名：TriToQuad() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// ラプラシアン法により節点の配置を修正し要素形状を改善する
//============================================================
void DT::laplaceQuad()
{
  const double EPS_LAPLACE = 1e-15;

  // 節点周りの四角形の要素の参照を得る
  for(int i=0; i < node.size(); i++){
    for(int j=0; j < quad.size(); j++){
      for(int k=0; k<4; k++){
	if( node[i] == quad[j]->p[k] ){
	  node[i]->share_quad.push_back( quad[j] );
	  break;
	}
      }
    }
  }

  double max_dr = DBL_MIN;
  do{   
    for(int i=0; i < node.size(); i++){
      if( typeid(*node[i]) == typeid(SuperNode) ) continue;
      if(node[i]->isOnBnd) continue;
      max_dr = DBL_MIN;

      //node[i]の移動
      double x0 = node[i]->x, y0 = node[i]->y;
      getPolyCenter(node[i]->share_quad, node[i]);
      
      double dr = dist(x0, y0, node[i]->x, node[i]->y);
      if( dr > max_dr) max_dr = dr;
    }
  }while(max_dr > EPS_LAPLACE);
}

//============================================================
// 関数名：getPolyCenter() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// 幾つかの四角形を合成して出来る多角形の重心を返す.
// laplaceQuad()で使用.
//============================================================
void DT::getPolyCenter(vector< Quad* > share_quad,Node *np)
{
  double x=0,y=0;
  double sum_all=0;

  for(int i=0; i < share_quad.size(); i++){
    double sum_quad = share_quad[i]->getArea();
    Point temp_node = share_quad[i]->getCenterPoint();
    //       double weight = 1;
    //       for(int j=0;j<4;j++)
    // 	if(share_quad[i]->p[j]->isOnBnd){
    // 	  weight = 1.02; break;
    // 	} 
    x += sum_quad * temp_node.x;
    y += sum_quad * temp_node.y;
    sum_all += sum_quad; 
  }

  np->x = x/sum_all;
  np->y = y/sum_all;
}

//============================================================
// 関数名：TriToQuad() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// 与えられた点をノーマライズする.
// generateFineMesh_quad()で生成された点のノーマライズで使用.
//============================================================
void DT::normalizePoint(Node *normalize_node)
{
  double rx,ry;

  rx = maxx-minx;
  ry = maxy-miny;
  square = (rx>ry) ? rx:ry ;
  normalize_node->x = (normalize_node->x - minx) / square;
  normalize_node->y = (normalize_node->y - miny) / square;
}


void DT::moveNewCirPoint(Edge* &e, Node *new_node)
{
  CircleEdge *c = dynamic_cast<CircleEdge*>(e);
  Point v1(new_node->x - c->center.x , new_node->y - c->center.y);
  double t  = c->r / sqrt(v1.x * v1.x + v1.y * v1.y);
  new_node->x = c->center.x + t*v1.x;
  new_node->y = c->center.y + t*v1.y;
}

void DT::makeEdges2(Edge* &e, Node *new_node)
{
  if(typeid(*e) == typeid(CircleEdge)){
    CircleEdge *c = dynamic_cast<CircleEdge*>(e);
    double theta1 = 2 * asin( dist(c->p[1]->x,c->p[1]->y,new_node->x,new_node->y) / (2*c->r)) * 180 / M_PI;
    if(c->theta < 0) theta1 *= -1;
    double theta2 = c->theta - theta1;
    CircleEdge *c1 = new CircleEdge(new_node,c->p[1],c->center.x,c->center.y,theta1);
    CircleEdge *c2 = new CircleEdge(c->p[0],new_node,c->center.x,c->center.y,theta2);
    c1->bc = c2->bc = c->bc;
    c1->val = c2->val = c->val;
    c1->material = c2->material = c->material;
    e = c1;
    edge.push_back(c2);
  }else{
    StraightEdge *c = dynamic_cast<StraightEdge*>(e);
    StraightEdge *c1 = new StraightEdge(new_node,c->p[1]);
    StraightEdge *c2 = new StraightEdge(c->p[0],new_node);
    c1->bc = c2->bc = c->bc;
    c1->val = c2->val = c->val;
    c1->material = c2->material = c->material;
    e = c1;
    edge.push_back(c2);
  }
}

//============================================================
// 関数名：canExistOnCircleEdgeEdge() --- class DT
// 引数　：円境界のアドレスe,存在できるか確認する点のアドレスnp,
//        その三角形で境界を構成しない点のアドレスTrip
// 戻り値：CircleEdgecleのEdgeで点が存在できるか否か
//---概要------------------------------------------------------
// 与えられた点が与えられたEdgeで存在できるかを返す
//============================================================
bool DT::canExistOnCirEdge(Edge* &e,Node *np,Node *triP)
{
  CircleEdge *c = dynamic_cast<CircleEdge*>(e);
  double det1 = getDet(triP->x,triP->y,c->p[0]->x,c->p[0]->y,c->p[1]->x,c->p[1]->y);
  double det2 = getDet(c->center.x,c->center.y,c->p[0]->x,c->p[0]->y,c->p[1]->x,c->p[1]->y);

  // 点npが存在する,ある三角形が凸であるならその点は存在出来る
  if((det1 * det2) > 0) return true;
  
  // 凹であり円の外部ならば存在出来る
  if( dist(np->x,np->y,c->center.x,c->center.y) > c->r ){
    return true;
  }else{
    return false;
  }
}

//============================================================
// 関数名：canExistInTri() --- class DT
// 引数　：存在できるか確認する点のアドレスnp,
//        その内部でnpが存在できるか確認する三角形pTri
// 戻り値：その三角形内で点npが存在できるかを返す
//---概要------------------------------------------------------
// 与えられた点npが与えられた与えられた三角形pTri内で存在できるか返す
//============================================================
bool DT::canExistInTri(Tri *pTri,Node *np)
{
  for(int i=0; i<3; i++){
    StraightEdge e;
    e.set( pTri->p[i], pTri->p[(i+1)%3] );
    for(int j=0; j<edge.size(); j++){
      if( e == *edge[j] ){
	if( typeid(*edge[j]) == typeid(CircleEdge) )
	  if( !canExistOnCirEdge( edge[j], np, pTri->p[(i+2)%3] ) ) return false; 
	break;
      }
    }
  }
  return true;
}

//============================================================
// 関数名：addNodeForBnd() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// 設定したintervalに応じて境界を適切な数に分割する.
// ここで生成する境界の数が少ないと適切に分割できなくなる.
//============================================================
void DT::addNodeForBnd()
{
  double grid_interval;
  cout << "input interval >> ";
  if(interval < 0) cin >> interval;

  double binterval = interval / square;
  //double binterval = 1.0*(M_PI/180.0)*90.0/20.0;

  int repeatNumber = edge.size();
  for(int i=0; i<repeatNumber; i++){
    if( typeid(*edge[i]) == typeid(StraightEdge) ){
      StraightEdge *s = dynamic_cast<StraightEdge*>(edge[i]);
      int divideNumber = (int) (s->length() / binterval) - 1;
            
      for(int j = 1; j <= divideNumber; j++){
	Point p =  s->divPoint(divideNumber , j);
	Node *np = new Node(p.x, p.y, node.size() + 1);
	np->isOnBnd = true;
	//新しい境界を生成
	updateBnd(edge[i],np);
	//制約線分の新規作成
	makeEdges2(edge[i],np);
	node.push_back(np);
      }
    }
     if( typeid(*edge[i]) == typeid(CircleEdge) ){
      CircleEdge *c = dynamic_cast<CircleEdge*>(edge[i]);
      double arc = 2 * M_PI * c->r * (fabs(c->theta) / 360);
      int divideNumber = (int) ceil(arc / binterval) -3;
   
      for(int j = 1; j <= divideNumber; j++){
	Point p =  c->divPoint(divideNumber , j);
	Node *np = new Node(p.x, p.y, node.size() + 1);
	np->isOnBnd = true;
	//新しい境界を生成
	updateBnd(edge[i],np);
	//制約線分の新規作成
	makeEdges2(edge[i],np);
	node.push_back(np);
      }
    }
  }
}

//============================================================
// 関数名：getAre() --- class Tri
// 引数　：なし
// 戻り値：三角形の面積
//---概要------------------------------------------------------
// 三角形の面積を計算して返す.
//============================================================
double Tri::getArea(){
  // ヘロンの公式により面積を取得
  // 計算時間かかる
  double a,b,c,s;
  a = length(0,1);
  b = length(1,2);
  c = length(2,0);
  s = (a+b+c)/2;
  return ( sqrt(s*(s-a)*(s-b)*(s-c)) );
}

//============================================================
// 関数名：getLongestEdge() --- class Tri
// 引数　：なし
// 戻り値：最長辺を返す
//---概要------------------------------------------------------
// 最長辺を見つけて返す.
//============================================================
StraightEdge Tri::getLongestEdge(){
  StraightEdge e[3];
  double max = DBL_MIN;
  int i,index;
  for(i=0;i<3;i++){
    e[i].set( p[i], p[(i+1)%3] );
    if( max < e[i].length() ){
      max = e[i].length();
      index = i;
    }
  }
  return e[index];
}

//============================================================
// 関数名：getMiddleEdge() --- class Tri
// 引数　：なし
// 戻り値：三角形を構成する辺で2番目に長い辺を返す
//---概要------------------------------------------------------
// 三角形を構成する辺で2番目に長い辺を見つけて返す.
//============================================================
StraightEdge Tri::getMiddleEdge(){
  StraightEdge e[3];
  double max = DBL_MIN;
  int i,index;
  for(i=0;i<3;i++){
    e[i].set( p[i], p[(i+1)%3] );
    if( max < e[i].length() ){
      max = e[i].length();
      index = i;
    }
  }
  StraightEdge etemp = e[0];
  e[0] = e[index];
  e[index] = etemp;
  
  if(e[1].length() > e[2].length())
    return e[1];
  else
    return e[2];
  }

//============================================================
// 関数名：getShortestEdge() --- class Tri
// 引数　：なし
// 戻り値：三角形を構成する辺で3番目に長い辺を返す
//---概要------------------------------------------------------
// 三角形を構成する辺で3番目に長い辺を見つけて返す.
//============================================================
StraightEdge Tri::getShortestEdge(){
  StraightEdge e[3];
  double min = DBL_MIN;
  int i,index;
  for(i=0;i<3;i++){
    e[i].set( p[i], p[(i+1)%3] );
    if( min > e[i].length() ){
      min = e[i].length();
      index = i;
    }
  }
  return e[index];
}

//============================================================
// 関数名：getPointPlace() --- class Tri
// 引数　：捜索する点np
// 戻り値：内部にある場合:-1.境界の場合その辺の番号:0~2を返
//---概要------------------------------------------------------
//  点が内部にあるか、辺上にあるかを確認して返す.
// 内部にある場合:-1.境界の場合その辺の番号:0~2を返す.
//============================================================
int Tri::getPointPlace(Node *np){
  double det;
  double x0,x1,x2,y0,y1,y2;
  
  x0 = np->x;
  y0 = np->y;

  for(int i=0; i<3;i++){
    x1 = p[i]->x;
    y1 = p[i]->y;
    x2 = p[(i+1)%3]->x;
    y2 = p[(i+1)%3]->y;    
    
    det = (x1-x0)*(y2-y0) - (y1-y0)*(x2-x0);
    
    if(fabs(det) <= CALC_EPS) return i; 
  }
  return -1;
}

//============================================================
// 関数名：getPointPlace() --- class Tri
// 引数　：なし
// 戻り値：三角形の重心の点
//---概要------------------------------------------------------
// 重心のノードを返す：境界上にはないと仮定
//============================================================
Point Tri::getCenterPoint(){
  double x,y;
  x = (p[0]->x + p[1]->x + p[2]->x)/3;
  y = (p[0]->y + p[1]->y + p[2]->y)/3;
  
  Point pointc(x,y);
  
  return pointc;
}

void DT::generateBnd_Quad()
{
  Tri *tBase = tri[0];
  Tri *t[3];
  for(int i=0;i<node.size();i++){
    if( typeid(*node[i]) == typeid(SuperNode) )continue;
    tBase = searchTri(tBase,node[i]);
    int pIndex; 
    pIndex = tBase->getPointPlace(node[i]);

    //三角形分割
    if(pIndex < 0){
      Tri *ts[3];
      stack< Tri* > triStack;
      div3Tri( ts, tBase, node[i]);
      for(int k=0;k<3;k++) triStack.push( ts[k] );
      swapingAlg( &triStack, node[i] );
    }else{
      Tri *ts[4];
      stack< Tri* > triStack;
      div4Tri( ts, tBase, tBase->nei[pIndex], node[i], pIndex );
      for(int k=0;k<4;k++) triStack.push( ts[k] );
      swapingAlg( &triStack, node[i] );
    }
    node[i]->isPlaced = true;
  }
}
