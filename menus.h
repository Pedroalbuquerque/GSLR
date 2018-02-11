#ifndef MENUS_H
#define MENUS_H


enum type_t: uint8_t { INFO =1, OPTION, CONFIG, };
enum nav_t: uint8_t { navMENU = 1, navSUB, };

typedef struct{
  uint8_t ID;           // menu ID 0-n
  type_t  type;           // menu type
  uint8_t entries;       // number of entries in this menu
} menuStrcture;


// Array that defines all menus available
menuStrcture menu[] =  {
  { 0, INFO, 4}, // display Altitude, Latitude, Longiude, Speed
  { 1, INFO, 3}, // maxAltitude, maxDistance, maxSpeed
  { 2, OPTION, 2},  //setHome, Reset maxValues
  { 3, OPTION, 1}, // dump LOG
  { 4, OPTION, 1}, //??
  { 5, OPTION, 1}
};



// Cursor that holds the menu and submenu selected in each moment

class Cursor {
private:
  uint8_t _maxMenu;
public:
  Cursor();
  //void init(menuStrcture menu[]);
  nav_t navigate;
  //  ~Cursor();
  uint8_t menuIdx, subIdx;
  uint8_t nextMenu();
  uint8_t prevMenu();
  uint8_t maxMenu();

  //void clsCursos::nextSub();
  //void Cursor::prevSub();
  //void Cursor::homeSub();
  //uint8_t maxM();
  void update(uint8_t button);

};

// cursor object instance
Cursor menuCursor; //define a cursor for the menu array

void menuAction(uint8_t button, Cursor myCursor);

/*
Cursor::init(menuStrcture menu[]){
  _maxMenu = sizeof(menu)/sizeof(menuStrcture);

  navigate = navMENU;
  menuIdx = 0;
  subIdx = 0;

}
*/

Cursor::Cursor( ) {
  _maxMenu = sizeof(menu)/sizeof(menuStrcture);
  menuIdx = 0 ;
  subIdx = 0;
  navigate = navMENU;
  return ;
}

uint8_t Cursor::nextMenu(){
  navigate = navMENU;
  update(btnUP);
  return menuIdx;
}

uint8_t Cursor::prevMenu(){
  navigate = navMENU;
  update(btnDOWN);
  return menuIdx;
}

uint8_t Cursor::maxMenu(){
  return _maxMenu;
}

void Cursor::update(uint8_t button){
  switch(button){
    case btnUP:
        if(navigate == navMENU){
          menuIdx++;
          if(menuIdx > _maxMenu) menuIdx = 0;  // if on base position move to next menu
          subIdx = 0;  // just for safety as navMENU should already mean
        }
        break;
    case btnDOWN:
        if(navigate == navMENU){
          menuIdx--;
          if(menuIdx > _maxMenu ) menuIdx = _maxMenu;  // if on base position move to next menu
          subIdx = 0;  // just for safety as navMENU should already mean
        }
        break;
    case btnSELECT:
        if (subIdx == 0){
            if(navigate == navMENU){
                navigate = navSUB;
                subIdx = 1;
            }
            else{
              navigate = navMENU;
              subIdx = 0;
            }
            return;  // if just switching navigate mode do not execute action
          }
        break;
  }
  if(navigate == navSUB){
      menuAction(button, menuCursor); // when entering a menu action a submenu is already selected
  }
}


#endif
