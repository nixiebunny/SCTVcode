// ------------------------- Four letter words -----------------------------

// four letter word stuff
int rn1;            // first  int of random number
int rn2;            // second int of random number
int rn3;            // third  int of random number
int rn4;            // last   int of random number
int xor1;           // first  int of thing to xor with it
int xor4;           // last   int of thing to xor with it
char FlwStr[] = "piss\n";      // four letters plus a NL and a 0

// A whole bunch of four letter words, no juicy ones.
const char* FLWs[]  =
 {"abet","able","ably","abut","aced","aces","ache","achy","acid","acme",
//  "yowl","yuck","yuks","yule","yups","yurt",
//  "zags","zany","zaps","zeal","zees","zerk","zero","zest","zigs","zinc",
  "zing","zips","ziti","zits","zone","zonk","zoom","zoos","zori"};

int FLWLen = sizeof(FLWs);

// Four letter word draw list
struct item flwList[] = {
  {ItemType::text,40,0,FlwStr,0,0},  // the nasty word itself
  {ItemType::listend,0,0,BlankLn,0,0}
};

void MakeFLW() {
  // strcpy(FlwStr, (char*)&(FLWs[1]));
}
