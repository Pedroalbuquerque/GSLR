

#include "menus.h"


// Navigation vars like Menu definition and Cursor



clsCursor::clsCursor(menuStrcture menu[]){
  _maxMenu = sizeof(menu)/sizeof(menuStrcture);
}
uint8_t clsCursor::nextMenu(){
  menuIdx ++;
  return menuIdx;
}

uint8_t clsCursor::prevMenu(){
  menuIdx ++;
  return menuIdx;
}

uint8_t clsCursor::maxMenu(){
  return _maxMenu;
}
