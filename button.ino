





void buttonSetup(){
  #if defined(PUSHBUTTON)// if no touchscreen use simple press buttons

    // ### Initialize push-buttons
    pinMode(BUTPIN1, INPUT_PULLUP); // Setup the first button with an internal pull-up
    pinMode(BUTPIN2, INPUT_PULLUP); // Setup the second button with an internal pull-up

  #endif
}


uint8_t buttonPressed(){

  #if defined SERIALBUTTON

      char c=0 ;
      if(Serial.available() > 0) {
        c = Serial.read();
        if((c >= '0') && c<= '9' ) c -= '0';
      }
      return c;

  #elif defined(PUSHBUTTON)// if no touchscreen use simple press buttons

    if (!digitalRead(BUTPIN1))  // process button 1 if pressed - Menu navigation
  		return 1;
  	else if (!digitalRead(BUTPIN2))
  		return 2;
    else
      return 0;

  #elif defined TFT_TOUCH_9341 // if display has touchscreen

		TSPoint p = ts.getPoint();
		pinMode(XM, OUTPUT);
		pinMode(YP, OUTPUT);

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
       DEBUG_MSG("raw (%d,%d) = %d\n",p.x, p.y, p.z);
       //Serial.print("X = "); Serial.print(p.x);
       //Serial.print("\tY = "); Serial.print(p.y);
       //Serial.print("\tPressure = "); Serial.println(p.z);

       switch(SCRROTATION){
         case 0:
            p.x = (map(p.x, TS_MINX, TS_MAXX, 0,display.width())) ;
            p.y = (map(p.y, TS_MINY, TS_MAXY, 0,display.height())) ;
            DEBUG_MSG("remaped rot=0 p:%d,%d - %d\n", p.x,p.y,p.z);
         break;
         case 1:
            p.x = (map(p.x, TS_MINX, TS_MAXX, display.width(),0));
            p.y = (map(p.y, TS_MINY, TS_MAXY, 0,display.height()));
            DEBUG_MSG("remaped rot=1 p:%d,%d - %d\n", p.x,p.y,p.z);
          break;
         case 2:
            p.x = (map(p.x, TS_MINX, TS_MAXX, display.width(),0)) ;
            p.y = (map(p.y, TS_MINY, TS_MAXY, display.height(),0)) ;
            DEBUG_MSG("remaped rot=2 p:%d,%d - %d\n", p.x,p.y,p.z);
          break;
         case 3:
            p.x = (map(p.x, TS_MINX, TS_MAXX, 0,display.width())) ;
            p.y = (map(p.y, TS_MINY, TS_MAXY, display.height(),0)) ;
            DEBUG_MSG("remaped rot=3 p:%d,%d - %d\n", p.x,p.y,p.z);
          break;

       }

      //delay(2000);

			if (p.y > (SCRPIXELY - BTSIZE_Y)){
				if (p.x > BTSIZE_X*2) return 3;
				else if (p.x < BTSIZE_X) return 1;
				else return 2;
			}
      else
        return 0;
		}
    else
      return 0;

  #endif

}
