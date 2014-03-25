#include "common.h"

bool smallObl( const Tri* left, const Tri* right )
{
  return ( left->obl > right->obl );
}

bool largeArea( const Tri* left, const Tri* right )
{
  return ( left->area < right->area );
}

//============================================================
// 関数名：isCircleEdgecledPt()
// 引数　：三角形p0,p1,p2，調査点p3
// 戻り値：三角形の外接円がp3を含めばtrue 否ならfalse
//---概要------------------------------------------------------
// (参考ホームページ)
// メッシュ生成のプログラミングTIPS
// http://ums.futene.net/wiki/MESH/A5E1A5C3A5B7A5E5C0B8C0AEA4CEA5D7A5EDA5B0A5E9A5DFA5F3A5B054495053.html
//============================================================
bool isCircleEdgecledPt( Node p0,Node p1, Node p2, Node p3 )
{
  double a[6] = {
    p0.x - p3.x,
    p1.x - p3.x,
    p2.x - p3.x,
    p0.y - p3.y,
    p1.y - p3.y,
    p2.y - p3.y,
  };
  double det =
    +( a[0] * (p0.x+p3.x) +a[3] * (p0.y+p3.y) ) * ( a[1]*a[5]-a[2]*a[4] )
    -( a[1] * (p1.x+p3.x) +a[4] * (p1.y+p3.y) ) * ( a[0]*a[5]-a[2]*a[3] )
    +( a[2] * (p2.x+p3.x) +a[5] * (p2.y+p3.y) ) * ( a[0]*a[4]-a[1]*a[3] );

  // 外接円内もしくは外接円上に点p3は存在する
  if( det <= 0 ) return true;
  // 外接円外に点pは存在する
  return false;
}

//============================================================
// 関数名：isCrossed()
// 引数　：線分e1,e2
// 戻り値：e1とe2が交われば true ,そうでなければ false
//---概要------------------------------------------------------
// 渡す線分はStraightEdgeであること．
// (参考ホームページ)
// 月の杜工房 - 2直線の交点を求める　
// http://mf-atelier.sakura.ne.jp/mf-atelier/modules/tips/index.php/program/algorithm/a1.html
//============================================================
bool isCrossed( StraightEdge e1, StraightEdge e2 )
{

  double x1,x2,x3,x4;
  double y1,y2,y3,y4;
  double ksi, eta, delta ;
  double ramda, mu ;

  x1 = e1.p[0]->x;  y1 = e1.p[0]->y;
  x2 = e1.p[1]->x;  y2 = e1.p[1]->y;
  x3 = e2.p[0]->x;  y3 = e2.p[0]->y;
  x4 = e2.p[1]->x;  y4 = e2.p[1]->y;
  
  ksi   = ( y4 - y3 )*( x4 - x1 ) - ( x4 - x3 )*( y4 - y1 );
  eta   = ( x2 - x1 )*( y4 - y1 ) - ( y2 - y1 )*( x4 - x1 );
  delta = ( x2 - x1 )*( y4 - y3 ) - ( y2 - y1 )*( x4 - x3 );
  ramda = ksi / delta;
  mu    = eta / delta;
  if( ramda > 0 && ramda < 1 && mu > 0 && mu < 1 )
    return true;
  return false;
}

//============================================================
// 関数名：normalize() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// 節点の座標を規格化する．
//============================================================
void DT::normalize()
{
  //double maxx,maxy;
  double rx,ry;
  maxx = maxy = DBL_MIN;
  minx = miny = DBL_MAX;

  for(unsigned int i=0; i<node.size(); i++){
    if( minx > node[i]->x ) minx = node[i]->x;
    if( miny > node[i]->y ) miny = node[i]->y;
    if( maxx < node[i]->x ) maxx = node[i]->x;
    if( maxy < node[i]->y ) maxy = node[i]->y;
  }
  
  rx = maxx-minx;
  ry = maxy-miny;
  square = (rx>ry) ? rx:ry ;

  for(unsigned int i=0; i<node.size(); i++){
    node[i]->x = (node[i]->x - minx) / square;
    node[i]->y = (node[i]->y - miny) / square;
  }
  
  for(unsigned int i=0; i<edge.size(); i++){
    if( typeid(*edge[i]) == typeid(CircleEdge) ){
      CircleEdge *c       = dynamic_cast<CircleEdge*>(edge[i]);
      c->center.x  = (c->center.x-minx) / square;
      c->center.y  = (c->center.y-miny) / square;
      c->r        /= square;
    }
  }
}

//============================================================
// 関数名：denormalize() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// 規格化された節点の座標を元に戻す．
//============================================================
void DT::denormalize()
{
  for(unsigned int i=0; i<node.size(); i++){
    node[i]->x = node[i]->x*square + minx;
    node[i]->y = node[i]->y*square + miny;
  }
  for(unsigned int i=0; i<edge.size(); i++){
    if( typeid(*edge[i]) == typeid(CircleEdge) ){
      CircleEdge *c       = dynamic_cast<CircleEdge*>(edge[i]);
      c->center.x  = c->center.x*square + minx;
      c->center.y  = c->center.y*square + miny;
      c->r        *= square;
    }
  }
}

//============================================================
// 関数名：setSuperTriangle() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// 仮想三角形の頂点を所定の位置に設置し，仮想三角形を構成する．
//============================================================
void DT::setSuperTriangle()
{
  node.push_back( new SuperNode( -1.23, -0.50,node.size()+1) );
  node.push_back( new SuperNode(  2.23, -0.50,node.size()+1) );
  node.push_back( new SuperNode(  0.50,  2.50,node.size()+1) );
  Tri *t = new Tri();
  t->set( node[node.size()-3], node[node.size()-2], node[node.size()-1]);
  t->canExist = true;
  tri.push_back(t);
}

//============================================================
// 関数名：canConvDescending() --- class DT
// 引数　：同一境界上での節点の順序a,b,c
// 戻り値：a,b,cが降順に並べかえられれば true ,そうでなければ false
//---概要------------------------------------------------------
// 同一境界上での節点の順序a,b,cを比較し降順か昇順か判断する．
//============================================================
bool canConvDescending( int a,int b,int c )
{
  int order[3];
  int max = INT_MIN;
  int temp;

  // a != b && a != c && b != c が保証されているとする
  order[0] = a;
  order[1] = b;
  order[2] = c;

  for(int i=0; i<3; i++)
    if( max < order[i] )
      max = order[i];

  while( order[0] != max ){
    temp     = order[0];
    order[0] = order[1];
    order[1] = order[2];
    order[2] = temp;
  }
  
  if( order[1] > order[2] )
    return true;

  return false;
}

//============================================================
// 関数名：removeIlligalTri() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要------------------------------------------------------
// 境界が要素を含むか判定する．どの境界にも属さない要素は存在できない．
//============================================================
void DT::removeIllegalTri()
{
  Tri *t;

  for(unsigned int i=0; i<tri.size(); i++){
    t           = tri[i];
    t->canExist = false;

    // 仮想三角形の頂点に接する三角形は除去
    if( typeid(*t->p[0]) == typeid(SuperNode) ||
	typeid(*t->p[1]) == typeid(SuperNode) ||
	typeid(*t->p[2]) == typeid(SuperNode) )
      continue;

    // ひとつでも境界上に位置しない点を持つ三角形は有効
    if( !t->p[0]->isOnBnd || !t->p[1]->isOnBnd || !t->p[2]->isOnBnd ){
      t->canExist = true;
      continue;
    }
    
    // 三角形の節点と境界の関係を調べる
    for( unsigned int i0=0; i0<t->p[0]->id.size(); i0++){
      for( unsigned int i1=0; i1<t->p[1]->id.size(); i1++){
        for( unsigned int i2=0; i2<t->p[2]->id.size(); i2++){
          int tb[3] = { t->p[0]->id[i0].bnd, t->p[1]->id[i1].bnd, t->p[2]->id[i2].bnd, };

	  // 同一境界上の節点で構成される三角形
          if( tb[0] == tb[1] && tb[0] == tb[2] ){
            //順列判定
            if( canConvDescending( t->p[0]->id[i0].order,
				   t->p[1]->id[i1].order,
				   t->p[2]->id[i2].order ) ){

	      // 三節点は同一の境界上に位置する
              if( bnd[tb[0]]->isOuter ){
                // 同一の外部境界上の三節点
                t->canExist = true;
                //t->current = bnd[tb[0]].current;
                //t->permeability = bnd[tb[0]].permeability;
              }else{
                // 同一の内部境界上の三節点
                for(unsigned int j=0; j<bnd.size(); j++){
                  for(unsigned int k=0; k<bnd[j]->inc_id.size(); k++){
                    if( k == tb[0] ) continue;
                    if( bnd[j]->inc_id[k] == tb[0] ){
                      t->canExist = true;
                      //t->current = bnd[j].current;
                      //t->permeability = bnd[j].permeability;
                      goto LOOP_END;
                    }
                  }
                }
              }
              goto LOOP_END;
            }
          }else{
            // ここの処理は検討不足の可能性がある．

            // 三節点は同一の境界上に位置しない
            // 1.ひとつでも外部境界上にあり，またひとつでも内部境界上にあれば
            //   その要素は外部境界が有するパラメータを引き継ぐ
            // 2.全て外部境界上にあり，ひとつの外部境界がもうひとつの外部境界を含んでいれば
            //   その要素は全てを包含する外部境界が有するパラメータを引き継ぐ
            // 3.全て内部境界上にあれば，
            //   その要素は内部境界を包含する外部境界が有するパラメータを引き継ぐ

            if( !bnd[tb[0]]->isOuter && !bnd[tb[1]]->isOuter && !bnd[tb[2]]->isOuter ){
              // 全て内部境界上
              bool flg[] = { false, false, false};
              for(unsigned int j=0; j<bnd.size(); j++){
                flg[0] = flg[1] = flg[2] = false;

		for(unsigned int k=0; k<bnd[j]->inc_id.size(); k++)
                  for(int tbi=0; tbi<3; tbi++)
                    if( !flg[tbi] && bnd[j]->inc_id[k] == tb[tbi] )
		      flg[tbi] = true;

		if( flg[0] && flg[1] && flg[2] ){
                  t->canExist = true;
                  //t->current = bnd[j].current;
                  //t->permeability = bnd[j].permeability;
                  goto LOOP_END;
                }
              }
	      
            }else{
              bool flg1,flg2;
              for(int tbi=0; tbi<3; tbi++){
                flg1 = flg2 = false;
                for(unsigned int j=0; j<bnd[tb[tbi]]->inc_id.size(); j++){
                  if( !flg1 && bnd[tb[tbi]]->inc_id[j] == tb[(tbi+1)%3] )
		    flg1 = true;
                  if( !flg2 && bnd[tb[tbi]]->inc_id[j] == tb[(tbi+2)%3] )
		    flg2 = true;
                }
                if( flg1 && flg2 ){
                  t->canExist = true;
                  //t->current = bnd[tb[tbi]].current;
                  //t->permeability = bnd[tb[tbi]].permeability;
                  goto LOOP_END;
                }
              }
            }
          }
        }
      }
    }
LOOP_END:;
  }
}

//============================================================
// 関数名：div3Tri() --- class DT
// 引数　：分割結果格納用の変数*t[3]，*pを含む三角形*tBase
//        新規追加ノード*p，
// 戻り値：なし
//---概要------------------------------------------------------
// 三角形の頂点と点*pをむすび，3つの小三角形を作り*t[3]に格納する．
//============================================================
void DT::div3Tri( Tri *t[3], Tri *tBase, Node *p )
{
  Tri temp = *tBase;
  t[0] = tBase;
  t[1] = new Tri();
  t[2] = new Tri();
  
  t[0]->set( temp.p[0], temp.p[1], p );
  t[1]->set( temp.p[1], temp.p[2], p );
  t[2]->set( temp.p[2], temp.p[0], p );

  t[0]->setNei( temp.nei[0], t[1], t[2] );
  t[1]->setNei( temp.nei[1], t[2], t[0] );
  t[2]->setNei( temp.nei[2], t[0], t[1] );

  // 外部の三角形と小三角形の関係の更新
  for(int i=0; i<3; i++){
    if( t[i]->nei[0] == NULL ) continue;
    for(int j=0; j<3; j++)
      if( t[i]->nei[0]->nei[j] == tBase )
        t[i]->nei[0]->nei[j] = t[i];
  }
  
  t[0]->canExist = t[1]->canExist = t[2]->canExist = temp.canExist;
  tri.push_back(t[1]);
  tri.push_back(t[2]);
}

//============================================================
// 関数名：div4Tri() --- class DT
// 引数　：分割結果格納用の変数*t[4]，着目している三角形*tBase
//        その隣の三角形*tBaseNei，新規追加ノード*p，
//        *pを含む(三角形*tBaseの)辺の開始点のインデックスstartPt
// 戻り値：なし
//---概要------------------------------------------------------
// ふたつの三角形*tBaseと*tBaseNeiについて，最長辺上の点*pと各対頂角を
// 結ぶことで4つの三角形を作る．隣接要素の更新を行い，
//============================================================
void DT::div4Tri( Tri *t[4], Tri *tBase, Tri *tBaseNei, Node *p, int startPt )
{
  Tri tb  = *tBase;
  Tri tbn = *tBaseNei;
  int startPtn;

  for(int i=0;i<3;i++){
    if( tbn.p[i] == tb.p[(startPt+1)%3] ) startPtn = i;
  }
  
  t[0] = tBase;
  t[1] = new Tri();
  t[2] = tBaseNei;
  t[3] = new Tri();

  t[0]->set(  tb.p[ (startPt+1)%3  ],   tb.p[(startPt+2)%3]  , p );
  t[1]->set(  tb.p[ (startPt+2)%3  ],   tb.p[startPt]        , p );
  t[2]->set( tbn.p[ (startPtn+1)%3 ],  tbn.p[(startPtn+2)%3] , p );
  t[3]->set( tbn.p[ (startPtn+2)%3 ],  tbn.p[startPtn]       , p );

  t[0]->setNei(  tb.nei[ (startPt+1)%3  ], t[1], t[3] );
  t[1]->setNei(  tb.nei[ (startPt+2)%3  ], t[2], t[0] );
  t[2]->setNei( tbn.nei[ (startPtn+1)%3 ], t[3], t[1] );
  t[3]->setNei( tbn.nei[ (startPtn+2)%3 ], t[0], t[2] );

  // 外部の三角形と小三角形の関係の更新
  for(int i=0; i<4; i++){
    if( t[i]->nei[0] == NULL ) continue;
    for(int j=0; j<3; j++)
      if( t[i]->nei[0]->nei[j] == tBase || t[i]->nei[0]->nei[j] == tBaseNei )
        t[i]->nei[0]->nei[j] = t[i];			
  }
  
  t[1]->canExist = true;
  if(t[2]->canExist)
    t[3]->canExist = true;

  tri.push_back(t[1]);
  tri.push_back(t[3]);
}

//============================================================
// 関数名：getDet()
// 引数　：調査する点(x0,y0),線分((始点)(x1,y1),(終点)(x2,y2))
// 戻り値：行列式の計算値
//---概要------------------------------------------------------
// 調査する点が線分の左右にあるか判断するための行列式を返す．
// 調査する点は戻り値が0ならば線分上，0より大きいなら左側，
// ０より小さいなら右側に位置する
//============================================================
double getDet(double x0,double y0,double x1,double y1,double x2,double y2)
{
  return ((x1-x0)*(y2-y0) - (y1-y0)*(x2-x0));
}

//============================================================
// 関数名：searchTri() --- class DT
// 引数　：設置点*p，探査開始位置の三角形*tBase
// 戻り値：設置点*pを内包する三角形
//---概要------------------------------------------------------
// 設置点*pを内包する三角形をローソンの探査法により探査する．
// なんかやたらバグが発生する．こまる．
// detがゼロになった場合の処理が良くないみたい．要検討．
//============================================================
extern DT *dt;
Tri *searchTri(Tri *tBase,Node *p)
{
  double det;
  double _x = p->x, _y = p->y;
  double dx,dy;

  srand((unsigned)time(NULL));

  for(int i=0;i<3;i++){
    det = getDet(_x                   ,  _y,
		 tBase->p[i]->x       , tBase->p[i]->y,
		 tBase->p[(i+1)%3]->x , tBase->p[(i+1)%3]->y);

    // 二点の座標が同じ->三角形として矛盾，終了する
    if( EQDBL(tBase->p[i]->x,tBase->p[(i+1)%3]->x) &&
	EQDBL(tBase->p[i]->y,tBase->p[(i+1)%3]->y) ){
      cout << "tBase = "   << *tBase      << endl;
      cout << " tBase->p[" << i << "] = " << endl << *(tBase->p[i]) << endl;
      cout << " tBase->p[" << (i+1)%3 << "] = " << endl << *(tBase->p[(i+1)%3]) << endl;
      dt->disp();
      exit(EXIT_FAILURE);
    }

    if( EQDBL(det,0) ){
      // 直線上に節点が存在する時
      // 節点をわずかに移動させる
      dx  = OUT_EPS * (rand()%10+1);
      dy  = OUT_EPS * (rand()%10+1);
      _x -= 10*OUT_EPS/2;
      _x += dx;
      _y -= 10*OUT_EPS/2;
      _y += dy;
      i--;
      continue;
    }else if( det < 0 ){
      // 設置点が辺の右側にあれば移動
      tBase = tBase->nei[i];
      i = -1;
    }
  }
  return tBase;
}

//============================================================
// 関数名：swapTri() --- class DT
// 引数　：分割対象の二つの三角形*tA,*tBとそれぞれの対角点のインデックス
// 戻り値：なし
//---概要------------------------------------------------------
// 二つの三角形に対してスワッピングアルゴリズムを実行する．
//============================================================
void DT::swapTri(Tri *tA,Tri *tB,int diagPtA,int diagPtB)
{
  Tri tmpA = *tA,tmpB = *tB;

  tmpA.set( tA->p[diagPtA] , tB->p[diagPtB]       , tB->p[(diagPtB+1)%3] );
  tmpB.set( tA->p[diagPtA] , tA->p[(diagPtA+1)%3] , tB->p[diagPtB]       ); 

  tmpA.setNei( tB               , tB->nei[diagPtB]       , tA->nei[(diagPtA+2)%3] );
  tmpB.setNei( tA->nei[diagPtA] , tB->nei[(diagPtB+2)%3] , tA                     );

  // 外部の三角形と変換した三角形の隣接関係を更新
  if( tA->nei[diagPtA] != NULL ){
    for(int i=0; i<3; i++)
      if( tA->nei[diagPtA]->nei[i] != NULL && tA->nei[diagPtA]->nei[i] == tA )
        tA->nei[diagPtA]->nei[i] = tB;
  }
  if( tB->nei[diagPtB] != NULL ){
    for(int i=0; i<3; i++)
      if( tB->nei[diagPtB]->nei[i] != NULL && tB->nei[diagPtB]->nei[i] == tB )
        tB->nei[diagPtB]->nei[i] = tA;
  }
  
  *tA = tmpA;
  *tB = tmpB;
}

//============================================================
// 関数名：swapingAlg() --- class DT
// 引数　：三角形のスタック *triStack，新規追加ノード *p
// 戻り値：*pに接する三角形群 vector<Tri*> triSharePt
//---概要------------------------------------------------------
// 新規追加ノード*pまわりでtriStackが空になるまで三角形を取り出し
// スワッピングアルゴリズムを行う．
// 返却値は偏平率分割，面積分割等に利用する．
//============================================================
vector< Tri* > DT::swapingAlg( stack< Tri* > *triStack, Node *p )
{
  vector< Tri* > triSharePt;

  while( !triStack->empty() ){
    Tri  *tA;                 // スタックから取り出した三角形
    Tri  *tB;                 // tAに隣接する三角形(pの対辺側)
    StraightEdge  diagLine,shareLine;  // 対角線，共有線
    int  diagPtA=0,diagPtB=0; // pで対角線を構成する点
    bool shareIsCond = false;
    bool diagIsCond  = false;

    tA = triStack->top();
    triStack->pop();

    bool haventA = true;
    
    // tAは節点pに接する三角形である．もし三角形群triSharePtがtAを持っていなければ
    // tAをtriSharePtに追加する．
    for(unsigned int i=0; i<triSharePt.size(); i++)
      if( tA == triSharePt[i] ) haventA = false;

    if(haventA) triSharePt.push_back( tA );

    // 対角線のtA側の端点を得る．
    for(int j=0; j<3; j++){
      if( tA->p[j] == p ){
        diagPtA = j;
        break;
      }
    }
    
    // tBを得る．tBはpの対辺側に位置し，tAに隣接する三角形である．
    if( tA->nei[(diagPtA+1)%3] != NULL )
      tB = tA->nei[(diagPtA+1)%3];
    else continue;
    
    // 対角線のtB側の端点を得る．
    for(int j=0; j<3; j++){
      if( tB->nei[j] != NULL && tB->nei[j] == tA ){
        diagPtB = (j+2)%3;
        break;
      }
    }

    // 対角線およびtA.tBが共有する(接する)線を生成する．
    diagLine.set(  tA->p[diagPtA]       , tB->p[diagPtB]       );
    shareLine.set( tA->p[(diagPtA+1)%3] , tA->p[(diagPtA+2)%3] );
    if( !isCrossed( diagLine, shareLine ) ) continue;

    // 対角線，共有線が制約線分か否か判定する．
    for(unsigned int j=0; j<edge.size(); j++){
      if( shareLine == *edge[j] ){
        shareIsCond = true;
        break;
      }
      if( diagLine == *edge[j] ){
        diagIsCond = true;
        break;
      }
    }

    if( shareIsCond ){
      // 共有線が制約線分であれば分割は行わない．
      continue;
    }else if(diagIsCond || shareLine.length() > diagLine.length() ){
      // 対角線が制約線分であるか，delaunay条件を満たすならばスワッピングを実行
      swapTri( tA, tB, diagPtA, diagPtB );

      // スワッピングの結果，tBはpに接する三角形となるので，triSharePtが
      // tBを既に格納していなければtBをtriSharePtに追加する．
      bool haventB = true;
      for(unsigned int i=0; i<triSharePt.size(); i++)
        if( tB == triSharePt[i] ) haventB = false;
      
      if( haventB )
	triSharePt.push_back( tB );

      triStack->push(tA);
      triStack->push(tB);
    }
  }  

  return triSharePt;
}

//============================================================
// 関数名：makeEdges() --- class DT
// 引数　：境界辺 *e, 新規追加ノード *new_node，追加番号 num
// 戻り値：なし
//---概要------------------------------------------------------
// 境界辺の分割を行う．
// 分割位置は*new_nodeとnumにより指定する．
// numは境界辺をn個の点で分割した際，num番目の点を指す．
// 二分割であればnum=2である．
// (注意1) この関数は必ずupdateBnd()より後に呼び出すこと．
//        (updateBnd()をこの関数に組み込むような改良を行うと良い)
// (注意2) num=2以外の動作確認はしていない．また直線はnumに対応していない
//        通常，引数numに２を指定すれば問題ない．
//============================================================
void DT::makeEdges(Edge* &e,Node* new_node,int num)
{
  // e上の節点new_nodeにより，eをnum個の境界線分に分割する．
  if( typeid(*e) ==typeid(CircleEdge) ){
    CircleEdge *c  = dynamic_cast<CircleEdge*>(e);
    CircleEdge *c1 = new CircleEdge( new_node, c->p[1],  c->center.x, c->center.y,  c->theta*(num-1)/num );
    CircleEdge *c2 = new CircleEdge( c->p[0] , new_node, c->center.x, c->center.y, c->theta/num         );

    c1->bc       = c2->bc       = c->bc;
    c1->val      = c2->val      = c->val;
    c1->material = c2->material = c->material;

    e = c1;
    edge.push_back(c2);
  }else{
    StraightEdge *c  = dynamic_cast<StraightEdge*>(e);
    StraightEdge *c1 = new StraightEdge( new_node, c->p[1] );
    StraightEdge *c2 = new StraightEdge( c->p[0],  new_node);

    c1->bc       = c2->bc       = c->bc;
    c1->val      = c2->val      = c->val;
    c1->material = c2->material = c->material;

    e = c1;
    edge.push_back(c2);
  }
}

//==================================================
// 関数名：updateBnd() --- class DT
// 引数　：境界辺 *e, 新規追加ノード *new_node
// 戻り値：なし
//---概要--------------------------------------------
// 境界辺*eを新規追加ノード*new_nodeで分割する前準備
// 境界辺*eを有する境界bnd[i]を走査し該当箇所に新規追加ノード
// のポインタnew_nodeを挿入する
//==================================================
void DT::updateBnd(Edge* e,Node *new_node)
{
  for(unsigned int i=0; i<bnd.size(); i++){
    // bnd[i]上のどの位置に新規追加ノードnew_nodeが来るのかを調べ，該当箇所に
    // 新規追加ノードを挿入する．
    int  cnt = 0;
    bool ins_flag = false;
    StraightEdge  test_e;

    list< Node* >::iterator itr = bnd[i]->p.begin();
    list< Node* >::iterator pre = bnd[i]->p.end();

    // p.end()はpの最後の要素の次の位置を指していることに注意
    // preにはpの最後の要素を格納しておく．そのためにデクリメントが必要．
    pre--;
    while( itr != bnd[i]->p.end() ){
      test_e.set( *pre, *itr );

      if( *e == test_e ){
        ins_flag = true;
        bnd[i]->p.insert( itr, new_node);
        Index id( i, cnt);
        new_node->id.push_back(id);
	break;
      }
      
      pre = itr;
      itr++;
      cnt++;
    }
    if(ins_flag){
      // new_nodeを挿入したら．挿入以後(cnt以上のorderを持つ)ノードの番号を1足す．
      list< Node* >::iterator sub = bnd[i]->p.begin();

      while( sub != bnd[i]->p.end() ){
	for(unsigned int j=0; j< (*sub)->id.size(); j++){
	  if(*sub == new_node)
	    continue;

	  if((*sub)->id[j].bnd == i && (*sub)->id[j].order >= cnt )
	    (*sub)->id[j].order++;
	}
	sub++;
      }
    }
  }
}

//==================================================
// 関数名：generateFineMesh() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要--------------------------------------------
// 節点を自動追加してメッシュの細分割を行う．
// 偏平率や面積等の条件に応じて節点位置を計算する
//==================================================
void DT::generateFineMesh()
{
  // 追加節点数の設定
  int addnum; // 追加節点数
  cout << " number of additional nodes : " << endl;
  cout << " >>" << flush ;
  cin >> addnum;

  // 要素の評価用変数
  vector< Tri* > rate;
  for(unsigned int i=0; i<tri.size(); i++){
    if( tri[i]->canExist ){
      tri[i]->calcParms();
      rate.push_back( tri[i] );
    }
  }

  // 偏平率降順or面積昇順にソート
  sort( rate.begin(), rate.end(), ::smallObl );  //扁平率
  //sort( rate.begin(), rate.end(), ::largeArea ); //面積

  // 初期化
  double min_area = 1;     // 最小面積 これ以下の面積の要素は分割しない
  bool   canSet   = false; // 新規ノードを追加出来る要素が存在するかどうか
  int    setIndex;         // 分割対象の要素のインデックス
  int    tester   = 1;
  
  for(int ii=0; ii<addnum; ii++){

    // 分割する三角形を取得
    canSet = false;
    for(unsigned int i=0; i<rate.size(); i++){
      if( rate[i]->canExist && rate[i]->area > min_area ){
        canSet   = true;
        setIndex = i;
        break;
      }
    }

    if( !canSet ){
      min_area *= 0.95; // 0 < min_area < 1 の範囲で設定
      ii--;             // 設置できなかった場合はカウンタ変数を一つ戻す
      continue;
    }

    Tri *t       = rate[setIndex];
    int startInd = 0;

    // 最長辺をStraightEdgeとして取得
    StraightEdge e( t->p[0], t->p[1] );

    for(int i=1; i<3; i++){
      if( e.length() < t->length(i,(i+1)%3) ){
        e.set(t->p[i],t->p[(i+1)%3]);
        startInd = i;
      }
    }

    Node *new_node = new Node( (e.p[0]->x + e.p[1]->x)/2.0,
			       (e.p[0]->y + e.p[1]->y)/2.0,
			       node.size()+1);

//     cout << "===== itr : " << ii << " ====================" << endl;
//     cout << "information of new node" << endl;
//     cout << *new_node << endl;
//     cout << "objective tri" << endl;
//     cout << *t << endl;

    // 円凹部の特殊処理
    bool isCircleEdgecled = false;
    
    Tri *tmp = t->nei[startInd];
    if( tmp->canExist ){
      for(unsigned int i=0; i<edge.size() && !isCircleEdgecled; i++){
        if( typeid(*edge[i]) != typeid(CircleEdge) )
	  continue;

	for(int j=0;j<3;j++){
          if( *edge[i] == StraightEdge( tmp->p[j] , tmp->p[(j+1)%3] ) ){
            CircleEdge *c     = dynamic_cast<CircleEdge*>(edge[i]);
            double det = getDet( c->center.x,        c->center.y,
				 tmp->p[j]->x,       tmp->p[j]->y,
				 tmp->p[(j+1)%3]->x, tmp->p[(j+1)%3]->y);

	    if( det < 0 ){
              // 中心点が境界辺の右側にある場合凹形状
              double dx = c->center.x - new_node->x;
              double dy = c->center.y - new_node->y;

	      if( dx*dx + dy*dy < c->r*c->r ){
                // new_nodeが設置位置の円内部にある場合
                isCircleEdgecled = true;
                t = tmp;
                e.set( tmp->p[j], tmp->p[(j+1)%3] );
                break;
              }
            }
          }
        }
      }
    }

//     cout << "objective tri after circle process" << endl;
//     cout << *t << endl;
//     cout << "isCircleEdgecled " << isCircleEdgecled << endl;
//     cout << "==================================================" << endl;

    // 辺eが制約線分であるかどうかの判定
    // 制約線分であるならば二分割して再格納
    for(unsigned int i=0; i<edge.size(); i++){
      if( e == *edge[i] ){
        // 中点の取得
        Point p = edge[i]->divPoint(1,1);
        new_node->set(p.x,p.y);
        new_node->isOnBnd = true;

        // 境界に中点を挿入
        updateBnd( edge[i], new_node);
        // 制約線分の新規作成
        makeEdges( edge[i], new_node, 2);

        break;
      }
    }

    // 三角形分割
    stack< Tri* >  triStack;
    vector< Tri* > triSharePt;

    if( !isCircleEdgecled ){    
      Tri *ts[4];
      div4Tri( ts, t, t->nei[startInd], new_node, startInd );
      for(int i=0; i<4; i++)
	triStack.push( ts[i] );
    }else{
      Tri* ts[3];
      div3Tri( ts,searchTri(tri[0],new_node),new_node );
      for(int i=0; i<3; i++)
	triStack.push( ts[i] );
    }

    triSharePt = swapingAlg( &triStack, new_node );

    if( isCircleEdgecled ) removeIllegalTri();

    // 要素の再評価
    // 点を設置することにより形状が変化した三角形はtriSharePtに格納されている
    // 変化した三角形の面積，偏平率を再計算し，また分割により増加した要素を
    // 評価用変数に新規格納する( rate.push_back( Tri* )のところ )
    vector< Tri* >::iterator itr;
    for(unsigned int i=0; i<triSharePt.size(); i++){
      if( !triSharePt[i]->canExist ) continue;
      // triSharePt[i]が評価用変数に存在しなければ追加
      if( ( itr = find( rate.begin(), rate.end(), triSharePt[i] )) == rate.end() )
        rate.push_back(triSharePt[i]);
    }

    node.push_back(new_node);

    if( addnum>9 && ii%(addnum/10) == 0 && ii!=0){
      laplace();
      cout << " ..." << (int)(ii/(double)addnum*100) << "%" << endl;
    }

    for(unsigned int i=0;i<rate.size();i++) rate[i]->calcParms(); 
    sort( rate.begin(), rate.end(), ::smallObl ); //偏平率
    //sort( rate.begin(), rate.end(), ::largeArea ); //面積


//     cout << "-----evaluation list of tri-----" << endl;
//     cout << " min_area = " << min_area << endl;
//     for(unsigned int i=0;i<rate.size();i++){
//       cout << " " << i << "---" << *rate[i] ;
//       cout << " , " << rate[i]->obl << " , " << rate[i]->area << endl;
//       if( i>5 ){
// 	cout << " and more..." << endl;
// 	break;
//       }
//     }
//     cout << "--------------------------------" << endl;
//     cout << "ii " << ii << endl;

  }

  cout << "(...100%)" << endl;
}

bool isPointOnLine(double x,double y,double x1,double y1,double x2,double y2){
  double det;
  double tmp;

  det = getDet(x, y, x1, y1, x2, y2);
  
  if( x1 > x2 ) { tmp = x1; x1 = x2; x2 = tmp; }
  if( y1 > y2 ) { tmp = y1; y1 = y2; y2 = tmp; }

  return( EQDBL(det,0) &&
	  ((x1-CALC_EPS) < x && x < (x2+CALC_EPS)) &&
	  ((y1-CALC_EPS) < y && y < (y2+CALC_EPS)) );
}

//==================================================
// 関数名：generateBnd() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要--------------------------------------------
// 初期状態の節点を配置することで境界を生成する．
//==================================================
void DT::generateBnd()
{
   Tri *tBase = tri[0];
   Tri *t[3];
   
   for(int i=0; i<node.size(); i++){
     if( typeid(*node[i]) == typeid(SuperNode) )
       continue;

     stack< Tri* > triStack;
     tBase = searchTri(tBase,node[i]);
     div3Tri(t,tBase,node[i]);

     for(int j=0; j<3; j++)
       triStack.push(t[j]);

     swapingAlg( &triStack, node[i] );
     node[i]->isPlaced = true;
   }
//   vector< Tri > pre(tri.size());
//   for(unsigned int i=0;i<pre.size();i++)
//     pre[i] = *tri[i];
//   bool allPlaced = false;
 
//   while( !allPlaced ){
//     for(unsigned int i=0;i<bnd.size();i++){
//       for(list<Node*>::iterator p = bnd[i]->p.begin();
// 	  p != bnd[i]->p.end(); p++){
// 	if( typeid(**p) == typeid(SuperNode) || (*p)->isPlaced ) continue;
// 	tBase = searchTri( tBase, *p );
// 	cout << endl << (**p) << " // now putting" << endl;
// 	int onlineIndex=-1;
// 	for(int j=0;j<3;j++){
// 	  if( isPointOnLine((*p)->x,(*p)->y,tBase->p[j]->x,tBase->p[j]->y,
// 			    tBase->p[(j+1)%3]->x, tBase->p[(j+1)%3]->y ) ){
// 	    onlineIndex = j;
// 	    cout << "||| " << **p << "||| is ONLINE" << endl;
// 	    break;
// 	  }
// 	}
	
// 	stack< Tri* > triStack;
// 	vector< Tri* > triSharePt;
// 	if(onlineIndex == -1){
// 	  Tri *t[3];
// 	  div3Tri(t,tBase,*p);
// 	  for(int j=0;j<3;j++) triStack.push(t[j]);
// 	}else{
// 	  Tri *t[4];
// 	  div4Tri(t,tBase,tBase->nei[onlineIndex],*p,onlineIndex);
// 	  for(int j=0;j<4;j++) triStack.push(t[j]);	  
// 	}

// 	triSharePt = swapingAlg( &triStack, *p );
      
// 	// 一つの三角形の二辺が同一の境界線分と交差する場合，その節点は後回しにする．
// 	bool is2cross = false;
// 	for(unsigned int j=0;j<triSharePt.size()&&!is2cross;j++){
// 	  Tri* tt = triSharePt[j];
// 	  for(unsigned int k=0;k<edge.size();k++){
// 	    int crosscnt = 0;
// 	    for(int l=0;l<3;l++){
// 	      if( isCrossed( StraightEdge(edge[k]->p[0],edge[k]->p[1]),
// 			     StraightEdge(tt->p[l], tt->p[(l+1)%3]) ) ) crosscnt++;
// 	    }
	    
// 	    if( crosscnt >= 2 ){
// 	      cout << "number " << (*p)->number << " was not put" <<endl;
// 	      is2cross = true;
// 	      break;
// 	    }
// 	  }
// 	}
	
// 	if( is2cross ){
// 	  tri.resize(pre.size());
// 	  for(unsigned int j=0;j<pre.size();j++)
// 	    *tri[j] = pre[j];
// 	}else{
// 	  pre.resize(tri.size());
// 	  for(unsigned int j=0;j<tri.size();j++)
// 	    pre[j] = *tri[j];
// 	  (*p)->isPlaced = true;
// 	}      
//       }
//     }

//     allPlaced = true;
//     for(unsigned int i=0;i<node.size();i++){
//       if( typeid(*node[i]) == typeid(SuperNode) ) continue;
//       if(!node[i]->isPlaced){
// 	allPlaced = false;
// 	break;
//       }
//     }
//   }
}

//==================================================
// 関数名：dist()
// 引数　：二点の座標p0(x1,y1),p1(x2,y2)
// 戻り値：二点間の距離
//---概要--------------------------------------------
// 距離|(x1,y1)-(x2,y2)|を計算し返す
//==================================================
double dist( double x1, double y1, double x2, double y2){
  double dx = fabs( x1 - x2 );
  double dy = fabs( y1 - y2 );

  return ( sqrt( dx*dx + dy*dy ) );
}

double dist( Point *p0, Point *p1 ){
  return( dist( p0->x, p0->y, p1->x, p1->y ));
}

//==================================================
// 関数名：laplace() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要--------------------------------------------
// ラプラシアン法により節点の配置を修正し要素形状を改善する
//==================================================
void DT::laplace()
{
  const double EPS_LAPLACE = 1e-8;

  for(unsigned int i=0; i<node.size(); i++){
    if(node[i]->isOnBnd || typeid(*node[i]) == typeid(SuperNode))
      continue;

    node[i]->tsp.clear();

    for(unsigned int j=0; j<tri.size(); j++){
      if(!tri[j]->canExist)
	continue;

      for(int k=0; k<3; k++){
        if( node[i] == tri[j]->p[k] ){
          node[i]->tsp.push_back( tri[j] );
          break;
        }
      }
    }
  }


  double          max_dr;
  vector< Node* > polyNode;

  do{
    for(unsigned int i=0; i<node.size(); i++){
      if( node[i]->isOnBnd || typeid(*node[i]) == typeid(SuperNode) )
	continue;

      max_dr = 0;

      // 多角形の構成
      polyNode.clear();
      for(unsigned int j=0; j<node[i]->tsp.size(); j++){
        for(int k=0; k<3; k++){
          if( node[i]->tsp[j]->p[k] == node[i] ){
            polyNode.push_back( node[i]->tsp[j]->p[(k+2)%3] );
            break;
          }
        }
      }

      // node[i]の移動
      Point sum( 0, 0 );
      Point pre( node[i]->x, node[i]->y );

      for(unsigned int j=0; j<polyNode.size(); j++){
        sum.x += polyNode[j]->x;
        sum.y += polyNode[j]->y;
      }
      
      node[i]->set(sum.x/(polyNode.size()),sum.y/(polyNode.size()));

      // 移動後の位置が許されるか判定
      bool canSet = true;

      for(unsigned int j=0; j<node[i]->tsp.size(); j++){
        Tri *tBase     = node[i]->tsp[j];
        Tri *tBaseNei;
        int tBcntrInd  = -1;
        int tBNcntrInd = -1;
	
        for(int k=0; k<3; k++){
          if( tBase->p[k] == node[i] ){
            tBaseNei  = tBase->nei[k];
            tBcntrInd = k;
            break;
          }
        }
        for(int k=0; k<3; k++){
          if( tBaseNei->p[k] == node[i] ){
            tBNcntrInd = k;
            break;
          }
        }
// 	cout << "base" << endl;
// 	cout << *tBase << endl;
// 	cout << "nei" << endl;
// 	cout << *tBaseNei << endl;
// 	if( tBase == NULL ){
// 	  cout << "ERROR in laplace() --- tBase is NULL" << endl;
// 	  exit(EXIT_FAILURE);
// 	}
// 	if( tBaseNei == NULL ){
// 	  cout << "ERROR in laplace() --- tBaseNei is NULL" << endl;
// 	  exit(EXIT_FAILURE);
// 	}

// 	if( tBcntrInd == -1 || tBNcntrInd == -1){
// 	  cout << "ERROR in laplace()" << endl;
// 	  exit(EXIT_FAILURE);
// 	}

        StraightEdge e1(    tBase->p[(tBcntrInd+1)%3],     tBase->p[(tBcntrInd+2)%3] );
        StraightEdge e2( tBaseNei->p[tBNcntrInd],       tBaseNei->p[(tBNcntrInd+1)%3] );
        if(isCrossed(e1,e2)) canSet = false;
      }
      
      if( !canSet )
	node[i]->set( pre.x, pre.y );

      double dr = dist( pre.x, pre.y, node[i]->x, node[i]->y );
      if( dr>max_dr) max_dr = dr;

    }
  }while( max_dr > EPS_LAPLACE );
}

//==================================================
// 関数名：model() --- class DT
// 引数　：なし
// 戻り値：なし
//---概要--------------------------------------------
// メッシュ生成プログラムの基幹
// 各種関数を呼び出すための関数
//==================================================
void DT::model()
{ 
  cout << " ---start normalize()." << endl;
  normalize();
  cout << " ---end normalize()." << endl << endl;

  cout << " ---start setSuperTriangle()." << endl;
  setSuperTriangle();
  cout << " ---end setSuperTriangle()." << endl << endl;

  cout << " ---start generateBnd()." << endl;
  if(form ==  1 || form == 2 || useOblDivide){
    generateBnd();
  }
  else if(form == 3 || form == 4){
    addNodeForBnd();
    generateBnd_Quad();
  }
  cout << " ---end generateBnd()." << endl << endl;

  cout << " ---start removeIlligalTri()." << endl;
  removeIllegalTri();
  cout << " ---end removeIlligalTri()." << endl << endl;

  cout << " ---start generateFineMesh()." << endl;
  if(form == 1 || form == 2 || useOblDivide) generateFineMesh();
  else if(form ==3 || form == 4) generateFineMesh_quad();
  cout << " ---end generateFineMesh()." << endl << endl;

  if(form == 3 || form == 4){
    cout << " ---start setTriMidNode()." << endl;
    setTriMidNode();
    cout << " ---end setTriMidNode()." << endl << endl;

    cout << " ---start makeQuad()." << endl;
    makeQuad();  
    cout << " ---end makeQuad()." << endl << endl;

    cout << " ---start makeQuad()." << endl;
    TriToQuad();
    cout << " ---end makeQuad()." << endl << endl;
  }

  cout << " ---start laplace()." << endl;
  if(form == 1 || form == 2) laplace();
  else if(form ==3 || form == 4) laplaceQuad();
  cout << " ---end laplace()." << endl << endl;

  cout << " ---start denormalize()." << endl;
  denormalize();
  cout << " ---end denormalize()." << endl << endl;  

  disp();

}
