

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

		if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
		{

			// scale from 0->1023 to tft.width
			p.x = (map(p.x, TS_MINX, TS_MAXX, display.width(), 0)) - 5;
			p.y = (map(p.y, TS_MINY, TS_MAXY, display.height(), 0)) - 15;

			if (p.y > 200)
			{
				if (p.x < BOXSIZE) {
					#ifdef BUZZER
						Blink(BUZZ, 5);
					#endif
						return 1;
				}
				else if (p.x < BOXSIZE * 2) {
					   return 2;
				}
			}
		}
    else
      return 0;

  #endif

}
