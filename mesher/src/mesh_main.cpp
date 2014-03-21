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
// �֐����FisCircleEdgecledPt()
// �����@�F�O�p�`p0,p1,p2�C�����_p3
// �߂�l�F�O�p�`�̊O�ډ~��p3���܂߂�true �ۂȂ�false
//---�T�v------------------------------------------------------
// (�Q�l�z�[���y�[�W)
// ���b�V�������̃v���O���~���OTIPS
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

  // �O�ډ~���������͊O�ډ~��ɓ_p3�͑��݂���
  if( det <= 0 ) return true;
  // �O�ډ~�O�ɓ_p�͑��݂���
  return false;
}

//============================================================
// �֐����FisCrossed()
// �����@�F����e1,e2
// �߂�l�Fe1��e2�������� true ,�����łȂ���� false
//---�T�v------------------------------------------------------
// �n��������StraightEdge�ł��邱�ƁD
// (�Q�l�z�[���y�[�W)
// ���̓m�H�[ - 2�����̌�_�����߂�@
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
// �֐����Fnormalize() --- class DT
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//---�T�v------------------------------------------------------
// �ߓ_�̍��W���K�i������D
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
// �֐����Fdenormalize() --- class DT
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//---�T�v------------------------------------------------------
// �K�i�����ꂽ�ߓ_�̍��W�����ɖ߂��D
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
// �֐����FsetSuperTriangle() --- class DT
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//---�T�v------------------------------------------------------
// ���z�O�p�`�̒��_������̈ʒu�ɐݒu���C���z�O�p�`���\������D
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
// �֐����FcanConvDescending() --- class DT
// �����@�F���ꋫ�E��ł̐ߓ_�̏���a,b,c
// �߂�l�Fa,b,c���~���ɕ��ׂ�������� true ,�����łȂ���� false
//---�T�v------------------------------------------------------
// ���ꋫ�E��ł̐ߓ_�̏���a,b,c���r���~�������������f����D
//============================================================
bool canConvDescending( int a,int b,int c )
{
  int order[3];
  int max = INT_MIN;
  int temp;

  // a != b && a != c && b != c ���ۏ؂���Ă���Ƃ���
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
// �֐����FremoveIlligalTri() --- class DT
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//---�T�v------------------------------------------------------
// ���E���v�f���܂ނ����肷��D�ǂ̋��E�ɂ������Ȃ��v�f�͑��݂ł��Ȃ��D
//============================================================
void DT::removeIllegalTri()
{
  Tri *t;

  for(unsigned int i=0; i<tri.size(); i++){
    t           = tri[i];
    t->canExist = false;

    // ���z�O�p�`�̒��_�ɐڂ���O�p�`�͏���
    if( typeid(*t->p[0]) == typeid(SuperNode) ||
	typeid(*t->p[1]) == typeid(SuperNode) ||
	typeid(*t->p[2]) == typeid(SuperNode) )
      continue;

    // �ЂƂł����E��Ɉʒu���Ȃ��_�����O�p�`�͗L��
    if( !t->p[0]->isOnBnd || !t->p[1]->isOnBnd || !t->p[2]->isOnBnd ){
      t->canExist = true;
      continue;
    }
    
    // �O�p�`�̐ߓ_�Ƌ��E�̊֌W�𒲂ׂ�
    for( unsigned int i0=0; i0<t->p[0]->id.size(); i0++){
      for( unsigned int i1=0; i1<t->p[1]->id.size(); i1++){
        for( unsigned int i2=0; i2<t->p[2]->id.size(); i2++){
          int tb[3] = { t->p[0]->id[i0].bnd, t->p[1]->id[i1].bnd, t->p[2]->id[i2].bnd, };

	  // ���ꋫ�E��̐ߓ_�ō\�������O�p�`
          if( tb[0] == tb[1] && tb[0] == tb[2] ){
            //���񔻒�
            if( canConvDescending( t->p[0]->id[i0].order,
				   t->p[1]->id[i1].order,
				   t->p[2]->id[i2].order ) ){

	      // �O�ߓ_�͓���̋��E��Ɉʒu����
              if( bnd[tb[0]]->isOuter ){
                // ����̊O�����E��̎O�ߓ_
                t->canExist = true;
                //t->current = bnd[tb[0]].current;
                //t->permeability = bnd[tb[0]].permeability;
              }else{
                // ����̓������E��̎O�ߓ_
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
            // �����̏����͌����s���̉\��������D

            // �O�ߓ_�͓���̋��E��Ɉʒu���Ȃ�
            // 1.�ЂƂł��O�����E��ɂ���C�܂��ЂƂł��������E��ɂ����
            //   ���̗v�f�͊O�����E���L����p�����[�^�������p��
            // 2.�S�ĊO�����E��ɂ���C�ЂƂ̊O�����E�������ЂƂ̊O�����E���܂�ł����
            //   ���̗v�f�͑S�Ă��܂���O�����E���L����p�����[�^�������p��
            // 3.�S�ē������E��ɂ���΁C
            //   ���̗v�f�͓������E���܂���O�����E���L����p�����[�^�������p��

            if( !bnd[tb[0]]->isOuter && !bnd[tb[1]]->isOuter && !bnd[tb[2]]->isOuter ){
              // �S�ē������E��
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
// �֐����Fdiv3Tri() --- class DT
// �����@�F�������ʊi�[�p�̕ϐ�*t[3]�C*p���܂ގO�p�`*tBase
//        �V�K�ǉ��m�[�h*p�C
// �߂�l�F�Ȃ�
//---�T�v------------------------------------------------------
// �O�p�`�̒��_�Ɠ_*p���ނ��сC3�̏��O�p�`�����*t[3]�Ɋi�[����D
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

  // �O���̎O�p�`�Ə��O�p�`�̊֌W�̍X�V
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
// �֐����Fdiv4Tri() --- class DT
// �����@�F�������ʊi�[�p�̕ϐ�*t[4]�C���ڂ��Ă���O�p�`*tBase
//        ���ׂ̗̎O�p�`*tBaseNei�C�V�K�ǉ��m�[�h*p�C
//        *p���܂�(�O�p�`*tBase��)�ӂ̊J�n�_�̃C���f�b�N�XstartPt
// �߂�l�F�Ȃ�
//---�T�v------------------------------------------------------
// �ӂ��̎O�p�`*tBase��*tBaseNei�ɂ��āC�Œ��ӏ�̓_*p�Ɗe�Β��p��
// ���Ԃ��Ƃ�4�̎O�p�`�����D�אڗv�f�̍X�V���s���C
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

  // �O���̎O�p�`�Ə��O�p�`�̊֌W�̍X�V
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
// �֐����FgetDet()
// �����@�F��������_(x0,y0),����((�n�_)(x1,y1),(�I�_)(x2,y2))
// �߂�l�F�s�񎮂̌v�Z�l
//---�T�v------------------------------------------------------
// ��������_�������̍��E�ɂ��邩���f���邽�߂̍s�񎮂�Ԃ��D
// ��������_�͖߂�l��0�Ȃ�ΐ�����C0���傫���Ȃ獶���C
// �O��菬�����Ȃ�E���Ɉʒu����
//============================================================
double getDet(double x0,double y0,double x1,double y1,double x2,double y2)
{
  return ((x1-x0)*(y2-y0) - (y1-y0)*(x2-x0));
}

//============================================================
// �֐����FsearchTri() --- class DT
// �����@�F�ݒu�_*p�C�T���J�n�ʒu�̎O�p�`*tBase
// �߂�l�F�ݒu�_*p������O�p�`
//---�T�v------------------------------------------------------
// �ݒu�_*p������O�p�`�����[�\���̒T���@�ɂ��T������D
// �Ȃ񂩂₽��o�O����������D���܂�D
// det���[���ɂȂ����ꍇ�̏������ǂ��Ȃ��݂����D�v�����D
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

    // ��_�̍��W������->�O�p�`�Ƃ��Ė����C�I������
    if( EQDBL(tBase->p[i]->x,tBase->p[(i+1)%3]->x) &&
	EQDBL(tBase->p[i]->y,tBase->p[(i+1)%3]->y) ){
      cout << "tBase = "   << *tBase      << endl;
      cout << " tBase->p[" << i << "] = " << endl << *(tBase->p[i]) << endl;
      cout << " tBase->p[" << (i+1)%3 << "] = " << endl << *(tBase->p[(i+1)%3]) << endl;
      dt->disp();
      exit(EXIT_FAILURE);
    }

    if( EQDBL(det,0) ){
      // ������ɐߓ_�����݂��鎞
      // �ߓ_���킸���Ɉړ�������
      dx  = OUT_EPS * (rand()%10+1);
      dy  = OUT_EPS * (rand()%10+1);
      _x -= 10*OUT_EPS/2;
      _x += dx;
      _y -= 10*OUT_EPS/2;
      _y += dy;
      i--;
      continue;
    }else if( det < 0 ){
      // �ݒu�_���ӂ̉E���ɂ���Έړ�
      tBase = tBase->nei[i];
      i = -1;
    }
  }
  return tBase;
}

//============================================================
// �֐����FswapTri() --- class DT
// �����@�F�����Ώۂ̓�̎O�p�`*tA,*tB�Ƃ��ꂼ��̑Ίp�_�̃C���f�b�N�X
// �߂�l�F�Ȃ�
//---�T�v------------------------------------------------------
// ��̎O�p�`�ɑ΂��ăX���b�s���O�A���S���Y�������s����D
//============================================================
void DT::swapTri(Tri *tA,Tri *tB,int diagPtA,int diagPtB)
{
  Tri tmpA = *tA,tmpB = *tB;

  tmpA.set( tA->p[diagPtA] , tB->p[diagPtB]       , tB->p[(diagPtB+1)%3] );
  tmpB.set( tA->p[diagPtA] , tA->p[(diagPtA+1)%3] , tB->p[diagPtB]       ); 

  tmpA.setNei( tB               , tB->nei[diagPtB]       , tA->nei[(diagPtA+2)%3] );
  tmpB.setNei( tA->nei[diagPtA] , tB->nei[(diagPtB+2)%3] , tA                     );

  // �O���̎O�p�`�ƕϊ������O�p�`�̗אڊ֌W���X�V
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
// �֐����FswapingAlg() --- class DT
// �����@�F�O�p�`�̃X�^�b�N *triStack�C�V�K�ǉ��m�[�h *p
// �߂�l�F*p�ɐڂ���O�p�`�Q vector<Tri*> triSharePt
//---�T�v------------------------------------------------------
// �V�K�ǉ��m�[�h*p�܂���triStack����ɂȂ�܂ŎO�p�`�����o��
// �X���b�s���O�A���S���Y�����s���D
// �ԋp�l�͕Ε��������C�ʐϕ������ɗ��p����D
//============================================================
vector< Tri* > DT::swapingAlg( stack< Tri* > *triStack, Node *p )
{
  vector< Tri* > triSharePt;

  while( !triStack->empty() ){
    Tri  *tA;                 // �X�^�b�N������o�����O�p�`
    Tri  *tB;                 // tA�ɗאڂ���O�p�`(p�̑Εӑ�)
    StraightEdge  diagLine,shareLine;  // �Ίp���C���L��
    int  diagPtA=0,diagPtB=0; // p�őΊp�����\������_
    bool shareIsCond = false;
    bool diagIsCond  = false;

    tA = triStack->top();
    triStack->pop();

    bool haventA = true;
    
    // tA�͐ߓ_p�ɐڂ���O�p�`�ł���D�����O�p�`�QtriSharePt��tA�������Ă��Ȃ����
    // tA��triSharePt�ɒǉ�����D
    for(unsigned int i=0; i<triSharePt.size(); i++)
      if( tA == triSharePt[i] ) haventA = false;

    if(haventA) triSharePt.push_back( tA );

    // �Ίp����tA���̒[�_�𓾂�D
    for(int j=0; j<3; j++){
      if( tA->p[j] == p ){
        diagPtA = j;
        break;
      }
    }
    
    // tB�𓾂�DtB��p�̑Εӑ��Ɉʒu���CtA�ɗאڂ���O�p�`�ł���D
    if( tA->nei[(diagPtA+1)%3] != NULL )
      tB = tA->nei[(diagPtA+1)%3];
    else continue;
    
    // �Ίp����tB���̒[�_�𓾂�D
    for(int j=0; j<3; j++){
      if( tB->nei[j] != NULL && tB->nei[j] == tA ){
        diagPtB = (j+2)%3;
        break;
      }
    }

    // �Ίp�������tA.tB�����L����(�ڂ���)���𐶐�����D
    diagLine.set(  tA->p[diagPtA]       , tB->p[diagPtB]       );
    shareLine.set( tA->p[(diagPtA+1)%3] , tA->p[(diagPtA+2)%3] );
    if( !isCrossed( diagLine, shareLine ) ) continue;

    // �Ίp���C���L��������������ۂ����肷��D
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
      // ���L������������ł���Ε����͍s��Ȃ��D
      continue;
    }else if(diagIsCond || shareLine.length() > diagLine.length() ){
      // �Ίp������������ł��邩�Cdelaunay�����𖞂����Ȃ�΃X���b�s���O�����s
      swapTri( tA, tB, diagPtA, diagPtB );

      // �X���b�s���O�̌��ʁCtB��p�ɐڂ���O�p�`�ƂȂ�̂ŁCtriSharePt��
      // tB�����Ɋi�[���Ă��Ȃ����tB��triSharePt�ɒǉ�����D
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
// �֐����FmakeEdges() --- class DT
// �����@�F���E�� *e, �V�K�ǉ��m�[�h *new_node�C�ǉ��ԍ� num
// �߂�l�F�Ȃ�
//---�T�v------------------------------------------------------
// ���E�ӂ̕������s���D
// �����ʒu��*new_node��num�ɂ��w�肷��D
// num�͋��E�ӂ�n�̓_�ŕ��������ہCnum�Ԗڂ̓_���w���D
// �񕪊��ł����num=2�ł���D
// (����1) ���̊֐��͕K��updateBnd()����ɌĂяo�����ƁD
//        (updateBnd()�����̊֐��ɑg�ݍ��ނ悤�ȉ��ǂ��s���Ɨǂ�)
// (����2) num=2�ȊO�̓���m�F�͂��Ă��Ȃ��D�܂�������num�ɑΉ����Ă��Ȃ�
//        �ʏ�C����num�ɂQ���w�肷��Ζ��Ȃ��D
//============================================================
void DT::makeEdges(Edge* &e,Node* new_node,int num)
{
  // e��̐ߓ_new_node�ɂ��Ce��num�̋��E�����ɕ�������D
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
// �֐����FupdateBnd() --- class DT
// �����@�F���E�� *e, �V�K�ǉ��m�[�h *new_node
// �߂�l�F�Ȃ�
//---�T�v--------------------------------------------
// ���E��*e��V�K�ǉ��m�[�h*new_node�ŕ�������O����
// ���E��*e��L���鋫�Ebnd[i]�𑖍����Y���ӏ��ɐV�K�ǉ��m�[�h
// �̃|�C���^new_node��}������
//==================================================
void DT::updateBnd(Edge* e,Node *new_node)
{
  for(unsigned int i=0; i<bnd.size(); i++){
    // bnd[i]��̂ǂ̈ʒu�ɐV�K�ǉ��m�[�hnew_node������̂��𒲂ׁC�Y���ӏ���
    // �V�K�ǉ��m�[�h��}������D
    int  cnt = 0;
    bool ins_flag = false;
    StraightEdge  test_e;

    list< Node* >::iterator itr = bnd[i]->p.begin();
    list< Node* >::iterator pre = bnd[i]->p.end();

    // p.end()��p�̍Ō�̗v�f�̎��̈ʒu���w���Ă��邱�Ƃɒ���
    // pre�ɂ�p�̍Ō�̗v�f���i�[���Ă����D���̂��߂Ƀf�N�������g���K�v�D
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
      // new_node��}��������D�}���Ȍ�(cnt�ȏ��order������)�m�[�h�̔ԍ���1�����D
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
// �֐����FgenerateFineMesh() --- class DT
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//---�T�v--------------------------------------------
// �ߓ_�������ǉ����ă��b�V���̍ו������s���D
// �Ε�����ʐϓ��̏����ɉ����Đߓ_�ʒu���v�Z����
//==================================================
void DT::generateFineMesh()
{
  // �ǉ��ߓ_���̐ݒ�
  int addnum; // �ǉ��ߓ_��
  cout << " number of additional nodes : " << endl;
  cout << " >>" << flush ;
  cin >> addnum;

  // �v�f�̕]���p�ϐ�
  vector< Tri* > rate;
  for(unsigned int i=0; i<tri.size(); i++){
    if( tri[i]->canExist ){
      tri[i]->calcParms();
      rate.push_back( tri[i] );
    }
  }

  // �Ε����~��or�ʐϏ����Ƀ\�[�g
  sort( rate.begin(), rate.end(), ::smallObl );  //�G����
  //sort( rate.begin(), rate.end(), ::largeArea ); //�ʐ�

  // ������
  double min_area = 1;     // �ŏ��ʐ� ����ȉ��̖ʐς̗v�f�͕������Ȃ�
  bool   canSet   = false; // �V�K�m�[�h��ǉ��o����v�f�����݂��邩�ǂ���
  int    setIndex;         // �����Ώۂ̗v�f�̃C���f�b�N�X
  int    tester   = 1;
  
  for(int ii=0; ii<addnum; ii++){

    // ��������O�p�`���擾
    canSet = false;
    for(unsigned int i=0; i<rate.size(); i++){
      if( rate[i]->canExist && rate[i]->area > min_area ){
        canSet   = true;
        setIndex = i;
        break;
      }
    }

    if( !canSet ){
      min_area *= 0.95; // 0 < min_area < 1 �͈̔͂Őݒ�
      ii--;             // �ݒu�ł��Ȃ������ꍇ�̓J�E���^�ϐ�����߂�
      continue;
    }

    Tri *t       = rate[setIndex];
    int startInd = 0;

    // �Œ��ӂ�StraightEdge�Ƃ��Ď擾
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

    // �~�����̓��ꏈ��
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
              // ���S�_�����E�ӂ̉E���ɂ���ꍇ���`��
              double dx = c->center.x - new_node->x;
              double dy = c->center.y - new_node->y;

	      if( dx*dx + dy*dy < c->r*c->r ){
                // new_node���ݒu�ʒu�̉~�����ɂ���ꍇ
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

    // ��e����������ł��邩�ǂ����̔���
    // ��������ł���Ȃ�Γ񕪊����čĊi�[
    for(unsigned int i=0; i<edge.size(); i++){
      if( e == *edge[i] ){
        // ���_�̎擾
        Point p = edge[i]->divPoint(1,1);
        new_node->set(p.x,p.y);
        new_node->isOnBnd = true;

        // ���E�ɒ��_��}��
        updateBnd( edge[i], new_node);
        // ��������̐V�K�쐬
        makeEdges( edge[i], new_node, 2);

        break;
      }
    }

    // �O�p�`����
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

    // �v�f�̍ĕ]��
    // �_��ݒu���邱�Ƃɂ��`�󂪕ω������O�p�`��triSharePt�Ɋi�[����Ă���
    // �ω������O�p�`�̖ʐρC�Ε������Čv�Z���C�܂������ɂ�葝�������v�f��
    // �]���p�ϐ��ɐV�K�i�[����( rate.push_back( Tri* )�̂Ƃ��� )
    vector< Tri* >::iterator itr;
    for(unsigned int i=0; i<triSharePt.size(); i++){
      if( !triSharePt[i]->canExist ) continue;
      // triSharePt[i]���]���p�ϐ��ɑ��݂��Ȃ���Βǉ�
      if( ( itr = find( rate.begin(), rate.end(), triSharePt[i] )) == rate.end() )
        rate.push_back(triSharePt[i]);
    }

    node.push_back(new_node);

    if( addnum>9 && ii%(addnum/10) == 0 && ii!=0){
      laplace();
      cout << " ..." << (int)(ii/(double)addnum*100) << "%" << endl;
    }

    for(unsigned int i=0;i<rate.size();i++) rate[i]->calcParms(); 
    sort( rate.begin(), rate.end(), ::smallObl ); //�Ε���
    //sort( rate.begin(), rate.end(), ::largeArea ); //�ʐ�


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
// �֐����FgenerateBnd() --- class DT
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//---�T�v--------------------------------------------
// ������Ԃ̐ߓ_��z�u���邱�Ƃŋ��E�𐶐�����D
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
      
// 	// ��̎O�p�`�̓�ӂ�����̋��E�����ƌ�������ꍇ�C���̐ߓ_�͌�񂵂ɂ���D
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
// �֐����Fdist()
// �����@�F��_�̍��Wp0(x1,y1),p1(x2,y2)
// �߂�l�F��_�Ԃ̋���
//---�T�v--------------------------------------------
// ����|(x1,y1)-(x2,y2)|���v�Z���Ԃ�
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
// �֐����Flaplace() --- class DT
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//---�T�v--------------------------------------------
// ���v���V�A���@�ɂ��ߓ_�̔z�u���C�����v�f�`������P����
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

      // ���p�`�̍\��
      polyNode.clear();
      for(unsigned int j=0; j<node[i]->tsp.size(); j++){
        for(int k=0; k<3; k++){
          if( node[i]->tsp[j]->p[k] == node[i] ){
            polyNode.push_back( node[i]->tsp[j]->p[(k+2)%3] );
            break;
          }
        }
      }

      // node[i]�̈ړ�
      Point sum( 0, 0 );
      Point pre( node[i]->x, node[i]->y );

      for(unsigned int j=0; j<polyNode.size(); j++){
        sum.x += polyNode[j]->x;
        sum.y += polyNode[j]->y;
      }
      
      node[i]->set(sum.x/(polyNode.size()),sum.y/(polyNode.size()));

      // �ړ���̈ʒu��������邩����
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
// �֐����Fmodel() --- class DT
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//---�T�v--------------------------------------------
// ���b�V�������v���O�����̊
// �e��֐����Ăяo�����߂̊֐�
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
