#define MAIN

#include "common.h"

DT *dt;

int main( int argc, char *argv[] )
{
  cout << "==================== start mesh ====================" << endl;

  dt = new DT();
  dt->form = 0;

  switch( argc ){
  case 2: dt->ifname = dt->ofname = (string) argv[1]; break;
  case 1: break;
  default:
    cout << "error : strange arguments." << endl;
    exit(EXIT_FAILURE);
  }

  if(dt->ifname == ""){
    cout << "input objective file name\n>>" ;
    cin >> dt->ifname;
    dt->ofname = dt->ifname;
  }
  
  dt->input(dt->ifname);

  if( dt->form == 0 ){
    do{
      cout << "choose output format : " << endl;
      cout << " 1. tri-1" << endl;
      cout << " 2. tri-2" << endl;
      cout << " 3. quad-1" << endl;
      cout << " 4. quad-2" << endl << endl;
      cout << ">>" ;
      cin >> dt->form;
    }while( dt->form < 1 || dt->form > 4 );    
  }

  dt->model();

  dt->output(dt->ofname);

  //dt->memo();

  string is_seen = "";
#if DEBUG
  is_seen = "y";
#else
  cout << "launch visualizer ? (y/n) \n >>" << flush ;
  cin >> is_seen;
#endif
  string cmdline;

  if(is_seen == "y"){
    //cmdline = "vis.exe " + dt->ofname; // for Windows_NT
    cmdline = "./meshvis " + dt->ofname; // for Unix-like
    
    cout << cmdline << endl;
    system(cmdline.c_str());
  }

  delete dt;

  cout << "==================== end mesh ====================" << endl;

  return 0;
}
