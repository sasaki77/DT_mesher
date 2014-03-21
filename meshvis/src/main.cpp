#define MAIN

#include <GLUT/glut.h>
#include <sstream>
#include "preprocess.h"
#include "gl2ps.h"

Params data;

string IntToStr(int num)
{
  stringstream ss;
  ss << num;
  return ss.str();
}

void drawNum(int id, double x, double y)
{
  glRasterPos3d(x,y,0);
  string s = IntToStr(id);
  for(int i=0;i<s.size();i++){
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, s.at(i));
  }
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  

  // 三角形要素or四角形要素の描画
  glLineWidth(1.0f);
  if(data.form <=2 ) glBegin(GL_TRIANGLES);
  else glBegin(GL_QUADS);
  glColor3d(0.7,0.7,0.7);
  for(int i=0;i<data.elem.size();i++){
    Element e = data.elem[i];
    for(int j=0;j<data.vertexNum;j++) glVertex2d(e.p[j]->z,e.p[j]->r);
  }
  glEnd();

  // 境界の描画
  if(data.isDispBoundary){
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3d(1.0,0.0,0.0);
    for(int i=0;i<data.edge.size();i++){
      Edge *e = data.edge[i];
      if(typeid(*e) == typeid(Str)){
	glVertex2d( e->p[0]->z, e->p[0]->r );
	glVertex2d( e->p[1]->z, e->p[1]->r );
      }else{
	Cir *c = dynamic_cast<Cir*>(e);
      
	double dt = c->rad/9;
	for(double rad1 = 0;fabs(rad1) < fabs(c->rad) ; rad1+=dt){
	  double z1,r1,z2,r2;
	  double rad2 = rad1 + dt;
	  z1 = c->center.z + c->r*cos(c->baseRad-rad1);
	  r1 = c->center.r + c->r*sin(c->baseRad-rad1);
	  z2 = c->center.z + c->r*cos(c->baseRad-rad2);
	  r2 = c->center.r + c->r*sin(c->baseRad-rad2);
	  glVertex2d(z1,r1);
	  glVertex2d(z2,r2);
	} 
      }
    }
    glEnd();
  }

  // 要素間の境界線の描画
  glLineWidth(1.0f);
  glBegin(GL_LINES);
  glColor3d(0.4,0.4,0.4);
  for(int i=0;i<data.elem.size();i++){
    Element e = data.elem[i];
    for(int j=0;j<data.vertexNum;j++){
      glVertex2d( e.p[j]->z , e.p[j]->r );
      glVertex2d( e.p[(j+1)% data.vertexNum]->z , e.p[(j+1)% data.vertexNum]->r );
    }
  }    
  glEnd();

  // ノード番号の出力
  if(data.isDispNodenum){
    glColor3d(0,0,0);
    for(int i=0;i<data.node.size();i++){
      int num = data.node[i].id;
      //if(data.node[i].id > data.firstNodenum ) num += 3;
      drawNum(num,data.node[i].z,data.node[i].r);
    }
  }
  glFlush();
}

void init(string fname)
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  data.input(fname);
  data.normalize();
  //三角形二次要素の表示においてノード番号出力の際，仮想三角形の三点分を考慮する
  //if(data.form==2)
    //for(int i=0;i<data.node.size();i++)
      //if(data.node[i].id > data.elem[0].p[3]->id) data.node[i].id += 0;
  //data.disp();
}

void saveAsEps(){
  FILE *fp;
  //  char file[]="visualized.eps";
  string fname = data.ifname + IntToStr(data.node.size()) + ".eps";
  int state = GL2PS_OVERFLOW, buffsize = 0;
  GLint viewport[4];
  int options=GL2PS_BEST_ROOT | GL2PS_OCCLUSION_CULL | GL2PS_USE_CURRENT_VIEWPORT;
  if( (fp = fopen(fname.c_str(), "wb")) == NULL ){
    cout << "cant open \"" << fname << "\"" << endl;
    exit(0);
  }

  cout << "start to output \"" << fname << "\" ..." << endl; 

  while(state == GL2PS_OVERFLOW){
    buffsize += 1024*1024;
    gl2psBeginPage(fname.c_str(), "gl2psTest", viewport, GL2PS_EPS, GL2PS_NO_SORT, options,
                   GL_RGBA, 0, NULL, 0, 0, 0,
                   buffsize, fp, fname.c_str());
    display();
    state = gl2psEndPage();
  }
  fclose(fp);

  cout << "Done." << endl;
}

void keyboard( unsigned char key, int x, int y ){
  switch(key){
  case 'q': exit(0);
  case 's': saveAsEps(); break;
  case 'n':
    data.isDispNodenum = !(data.isDispNodenum);
    glutPostRedisplay();
    break;
  case 'b':
    data.isDispBoundary = !(data.isDispBoundary);
    glutPostRedisplay();
  default: break;
  }
}

void glMain(int argc,char *argv[])
{
  cout << "----- vismesh ---------------" << endl;

  string fname;
  if( argc == 1 ){
    cout << "input filename?\n>>" << endl;
    cin >> fname;
  }else if( argc == 2 ){
    fname = (string)argv[1];
  }else{
    cout << "check number of arguments" << endl;
    return;
  }
  glutInitWindowSize(600,600);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA);
  glutCreateWindow("mesh visualizer");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  init(fname);
  glutMainLoop();
  cout << "----- vismesh (end) ----------" << endl;
}

int main(int argc, char *argv[])
{
  srand((unsigned)time(NULL));
  glMain(argc,argv);
  return 0;
}
