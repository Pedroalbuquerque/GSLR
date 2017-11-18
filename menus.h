#ifndef MENUS_H
#define MENUS_H



enum type_t: uint8_t { INFO =1, OPTION, CONFIG, };

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
  { 3, OPTION, 1} // dump LOG
};



// Cursor that holds the menu and submenu selected in each moment

class clsCursor {
private:
  uint8_t _maxMenu;
public:
  clsCursor(menuStrcture menu[]);
//  ~clsCursor();
  uint8_t menuIdx, subIdx;
  uint8_t nextMenu();
  uint8_t prevMenu();
  //void clsCursos::nextSub();
  //void clsCursor::prevSub();
  //void clsCursor::homeSub();
  uint8_t maxMenu();
};




#endif
