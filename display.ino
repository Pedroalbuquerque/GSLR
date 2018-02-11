void displaySetup(){

  #ifdef LCD
    pinMode(PIN_LCD_LIGHT, OUTPUT); //LCD backlight, LOW = backlight ON
  #endif


	// ### Initialize  display

	#ifdef TFT_TOUCH_9341
    #ifdef  TFT_SPI
      display.begin();
    #else
		  display.reset();
      uint16_t identifier = display.readID();
      if (identifier == 0x9341){
  			display.begin(identifier);
  		}
  		else
  		{
  			Serial.print(F("Unknown LCD driver chip: "));
  			Serial.println(identifier, HEX);
  			return;
  		}
    #endif
    display.fillScreen(BLACK);
    display.setTextSize(CHARSCALE);
    display.setRotation(SCRROTATION);
    display.setTextColor(WHITE, BLACK);
    display.setTextWrap(true);
    displaySetCursor(2, 0);
    display.println(F("GPS TELEMETRY"));
    display.println(F("Ver:")); display.println(VERSION);
    delay(3000);
    displaySetCursor(8, 0);
    display.setTextColor(WHITE);  display.setTextSize(2);
    display.println("    Please wait...");
    display.println("   Analizing LOG...");

	#endif

  #ifdef LCD // display SETUP (Nokia LCD)
		display.begin();
		displayReset(); //Cleanup the LCD
		display.setContrast(60); // you can change the contrast around to adapt the display for the best viewing!
		display.println("Starting LCD display!");
		display.print("Ver. "); display.println(VERSION);
		display.display(); // show splash screen
		delay(1000);
	#endif

	#ifdef TFT_ST7735	// display Setup OLED LCD
		display.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
		// POSITION CURSOR ON TOP LEFT
		displayReset();
		display.println("Starting TFT display!");
		display.print("Ver:"); display.println(VERSION);
		delay(5000);
	#endif

	#ifdef TFT_ILI9340 // display Setup OLED LCD
		display.begin();
		displayReset();
		display.println("Starting TFT display!");
		display.print("Ver:"); display.println(VERSION);
		Serial.println("TFT Display ILI9340 - initialized");
	#endif




}

void displaywarning(int warningcode){
	displaySetCursor(SCRLINES, 0);
	display.setTextColor(WHITE, BLACK);
	display.print(fill(strtmp, SCRCHARS, ' ', true));
	displaySetCursor(SCRLINES, 0);
	if (warningcode & WRN_LINK) //link lost
	{
		#ifdef DEBUG
			Serial.print F("link Lost");
		#endif

		display.setTextColor(WHITE, RED);
		display.print(" LNK ");
	}
	if (warningcode & WRN_FIX)  // GPS fix lost
	{
		#ifdef DEBUG
			Serial.println F("GPS fix Lost");
		#endif

		display.setTextColor(BLACK, ORANGE);
		display.print(" GPS ");

	}
	displaySetCursor(0, 0);
	display.setTextColor(WHITE, BLACK);

	#ifdef LCD
		display.display();
	#endif
}

void displaymenu(byte menuPage, bool forceRepaint){

	static byte lastmenu;  //remember last menu for paint or refresh

	switch (menuPage)
	{
		case 0: //NAVIGATION STATUS MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;
				displayReset();
				displaySetCursor(2, 0);
				display.print("1 - STATUS");
				displaySetCursor(3, 0); sprintf(strPRT, "SAT:%i", Data.satellites); display.print(strPRT);
				displaySetCursor(4, 0); sprintf(strPRT, "QUAL:%d", Data.fixquality); display.print(strPRT);
				displaySetCursor(5, 0); sprintf(strPRT, "HDOP:%s", dtostrf(Data.HDOP, 5, 2, strtmp)); display.print(strPRT);
				displaySetCursor(6, 0); sprintf(strPRT, "RX_RSSI:%d", rssi); display.print(strPRT);
				displaySetCursor(7, 0); sprintf(strPRT, "RS BAT:%s V", dtostrf(Data.batteryVolts, 4, 2, strtmp)); display.print(strPRT);
			}
			else // if already in menu, print just info to reduce screen flicker
			{
				displaySetCursor(3, 4);
				display.print(Data.satellites);
				displaySetCursor(4, 5);
				display.print(Data.fixquality);
				displaySetCursor(5, 5);  sprintf(strPRT, "%s", dtostrf(Data.HDOP, 5, 2, strtmp)); display.print(strPRT);
				displaySetCursor(6, 8);	display.print(rssi);
				displaySetCursor(7, 7); sprintf(strPRT, "%s", dtostrf(Data.batteryVolts, 4, 2, strtmp));	display.print(strPRT);
			}
			break;
		}

		case 1: //INFO MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;
				displayReset();
				displaySetCursor(2, 0); display.print("2 - INFO");
				sprintf(strPRT, "ALT:%s m", dtostrf(Data.altitude - homealt, 4, 0, strtmp)); displaySetCursor(3, 0); display.print(strPRT);
				sprintf(strPRT, "SPD:%s Km/h", dtostrf(Data.groundspeed*1.852, 4, 0, strtmp)); displaySetCursor(4, 0); display.print(strPRT);
				sprintf(strPRT, "AZM:%s Deg", dtostrf(homeazim, 4, 0, strtmp)); displaySetCursor(5, 0); display.print(strPRT);
				displaySetCursor(6, 0); sprintf(strPRT, "DST:%s", dtostrf(homedist, 4, 0, strtmp)); display.print(strPRT);
				if (kmflag == 0) display.print(" m "); else display.print(" Km");
				display.setTextSize(CHARSCALE + 2);
				displaySetCursor(8, 0); sprintf(strPRT, "%s Km/h", dtostrf(highspeed, 1, 0, strtmp)); display.print(strPRT);
				display.setTextSize(CHARSCALE);
			}
			else
			{
				sprintf(strPRT, "%s m      ", dtostrf(Data.altitude - homealt, 4, 0, strtmp)); displaySetCursor(3, 4); display.print(strPRT);
				sprintf(strPRT, "%s Km/h      ", dtostrf(Data.groundspeed*1.852, 4, 0, strtmp)); displaySetCursor(4, 4); display.print(strPRT);
				sprintf(strPRT, "%s Deg      ", dtostrf(homeazim, 4, 0, strtmp)); displaySetCursor(5, 4); display.print(strPRT);
				displaySetCursor(6, 4); sprintf(strPRT, "%s", dtostrf(homedist, 4, 0, strtmp)); display.print(strPRT);
				if (kmflag == 0) display.print(" m "); else display.print(" Km"); //Spaces added to allow "m" or "Km" to be erased after large numbers
				display.setTextSize(CHARSCALE + 2);
				displaySetCursor(8, 0); sprintf(strPRT, "%s Km/h", dtostrf(highspeed, 1, 0, strtmp)); display.print(strPRT);
				display.setTextSize(CHARSCALE);
			}
			break;
		}

		case 2: //MAXIMUM MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;
				displayReset();
				displaySetCursor(2, 0); display.print("3 - MAXIMUM");
				displaySetCursor(3, 0); sprintf(strPRT, "MxSpd:%s K/h", dtostrf(highspeed, 4, 0, strtmp)); display.print(strPRT);
				displaySetCursor(4, 0); sprintf(strPRT, "MxAlt:%4ld m", maxalt); display.print(strPRT);
				displaySetCursor(5, 0); sprintf(strPRT, "MxDst:%4ld", maxdist); display.print(strPRT);
				if (kmflag == 0) display.print(" m "); else display.print(" Km");
				displaySetCursor(7, 0); sprintf(strPRT, "GPS Alt:%s m", dtostrf(Data.altitude, 5, 0, strtmp)); display.print(strPRT);
			}
			else
			{
				displaySetCursor(3, 6); sprintf(strPRT, "%s", dtostrf(highspeed, 4, 0, strtmp)); display.print(strPRT);
				displaySetCursor(4, 6); sprintf(strPRT, "%4ld", maxalt); display.print(strPRT);
				displaySetCursor(5, 6); sprintf(strPRT, "%4ld", maxdist); display.print(strPRT);
				if (kmflag == 0) display.print(" m "); else display.print(" Km");
				displaySetCursor(7, 8); sprintf(strPRT, "%s ", dtostrf(Data.altitude, 5, 0, strtmp)); display.print(strPRT);
			}
			break;
		}

		case 3: //COORDINATES MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;
				displayReset();
				displaySetCursor(2, 0); display.print("4 - P. POS");
				displaySetCursor(3, 0); sprintf(strPRT, "LAT:%c %s", Data.lat, dtostrf(Data.latitude, 9, 4, strtmp)); display.print(strPRT);
				displaySetCursor(4, 0); sprintf(strPRT, "LON:%c %s", Data.lon, dtostrf(Data.longitude, 9, 4, strtmp)); display.print(strPRT);
				displaySetCursor(5, 0); sprintf(strPRT, "GOOGLE COORD:"); display.print(strPRT);
				displaySetCursor(6, 0); sprintf(strPRT, "%s", dtostrf(Data.latitudedeg, 10, 6, strtmp)); display.print(strPRT);
				displaySetCursor(7, 0); sprintf(strPRT, "%s", dtostrf(Data.longitudedeg, 10, 6, strtmp)); display.print(strPRT);
			}
			else
			{
				displaySetCursor(3, 4); sprintf(strPRT, "%c %s", Data.lat, dtostrf(Data.latitude, 9, 4, strtmp)); display.print(strPRT);
				displaySetCursor(4, 4); sprintf(strPRT, "%c %s", Data.lon, dtostrf(Data.longitude, 9, 4, strtmp)); display.print(strPRT);
				displaySetCursor(6, 0); sprintf(strPRT, "%s", dtostrf(Data.latitudedeg, 10, 6, strtmp)); display.print(strPRT);
				displaySetCursor(7, 0); sprintf(strPRT, "%s", dtostrf(Data.longitudedeg, 10, 6, strtmp)); display.print(strPRT);
			}
			break;
		}

		case 4: //RECOVERY MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;
				displayReset();
				displaySetCursor(2, 0); display.print("5 - RECOVERY");
				displaySetCursor(3, 0); display.print("GOOGLE COORD:");
				displaySetCursor(4, 0); sprintf(strPRT, "%s", dtostrf(Data.latitudedeg, 9, 6, strtmp)); display.print(strPRT);
				displaySetCursor(5, 0); sprintf(strPRT, "%s", dtostrf(Data.longitudedeg, 9, 6, strtmp)); display.print(strPRT);
				displaySetCursor(6, 0); sprintf(strPRT, "AZM:%ld", homeazim); display.print(strPRT);
				displaySetCursor(7, 0); sprintf(strPRT, "DIS:%ld", homedist); display.print(strPRT);
				if (kmflag == 0) display.print("m"); else display.print("Km");
			}
			else
			{
				displaySetCursor(4, 0); sprintf(strPRT, "%s", dtostrf(Data.latitudedeg, 9, 6, strtmp)); display.print(strPRT);
				displaySetCursor(5, 0); sprintf(strPRT, "%s", dtostrf(Data.longitudedeg, 9, 6, strtmp)); display.print(strPRT);
				displaySetCursor(6, 4); sprintf(strPRT, "%ld", homeazim); display.print(strPRT);
				displaySetCursor(7, 4); sprintf(strPRT, "%ld", homedist); display.print(strPRT);
				if (kmflag == 0) display.print("m"); else display.print("Km");
			}
			break;
		}

		case 5: //TOOLS MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;

				displayReset();
				displaySetCursor(2, 0); display.print("6 - TOOLS");
				displaySetCursor(3, 0); display.print("Curr. Ver.:");
				displaySetCursor(4, 0); sprintf(strPRT, "%s", VERSION); display.print(strPRT);
			}
			break;
		}

		case 6: //LOG MEN
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;

				displayReset();
				displaySetCursor(2, 0); display.print("7 - LOGGER");
				displaySetCursor(3, 0); sprintf(strPRT, "#Records:%lu", mylog.numRecords); display.print(strPRT);
				displaySetCursor(4, 0); sprintf(strPRT, "press B2 to erase!"); display.print(strPRT);
			}
			else
			{
				displaySetCursor(3, 9); sprintf(strPRT, "%lu", mylog.numRecords); display.print(strPRT);
			}
			break;
		}
		case 7: //LOG dump
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;

				displayReset();
				displaySetCursor(2, 0); display.print("8 - Log Dump");
				displaySetCursor(3, 0); sprintf(strPRT, "connect to PC %ld", mylog.numRecords); display.print(strPRT);
				displaySetCursor(4, 0); sprintf(strPRT, "press B2 to dump!"); display.print(strPRT);
			}
			break;
		}
	}
	#ifdef LCD
		display.display();
	#endif
	return;
}

void drawButton( uint8_t pos, uint16_t color,const char *caption){
  uint16_t posX, posY;

    switch (pos) {
      case 1:
          posX = 0;
          posY = SCRPIXELY - BTSIZE_Y;
        break;
      case 2:
          posX = BTSIZE_X ;
          posY = SCRPIXELY - BTSIZE_Y;
          break;
      case 3:
          posX = BTSIZE_X * 2 ;
          posY = SCRPIXELY - BTSIZE_Y;
          break;

    }

  // Draw button§
  display.fillRect(posX, posY, BTSIZE_X, BTSIZE_Y, color);
  display.drawRect(posX, posY, BTSIZE_X, BTSIZE_Y, WHITE);


  // write button caption
  display.setCursor(posX+BTSIZE_X/2- CHARWIDTH, posY+BTSIZE_Y/2- CHARWIDTH);
  display.setTextColor(WHITE);  display.setTextSize(4);
  display.println(caption);

}

void displayButtons(){
  drawButton(1, RED, "<");
  drawButton(2, RED, ">");
  drawButton(3, YELLOW, "*");

}

void displayReset(){
		//display.fillScreen(BLACK);

    display.fillRect(0, ALERTHIGH+1, display.width(), display.height()-ALERTHIGH- BTSIZE_Y-1, BLACK);

		display.setTextSize(CHARSCALE);
		display.setRotation(SCRROTATION);
		display.setTextColor(WHITE, BLACK);
		display.setTextWrap(true);
		display.setCursor(0, 0);
		return;
	}

void displaySetCursor(int line, int column) {
  	display.setCursor(column * CHARWIDTH * CHARSCALE, line * CHARHEIGHT*CHARSCALE);
  	return;
  }

void makeHeader(){
    display.setTextSize(CHARSCALE);
    display.setTextColor(WHITE, BLACK);

  	if (Data.batteryVolts < 3.2){
  		display.fillRect(204, 0, 14, 12, RED);
  		display.drawRect(204, 0, 14, 12, BLACK);
  		display.fillRect(216, 0, 14, 12, BLACK);
  		display.drawRect(216, 0, 14, 12, BLACK);
  		display.fillRect(228, 0, 14, 12, BLACK);
  		display.drawRect(228, 0, 14, 12, BLACK);
  	}
  	else if (Data.batteryVolts >= 3.2 && Data.batteryVolts < 3.6){
  		display.fillRect(204, 0, 14, 12, YELLOW);
  		display.drawRect(204, 0, 14, 12, BLACK);
  		display.fillRect(216, 0, 14, 12, YELLOW);
  		display.drawRect(216, 0, 14, 12, BLACK);
  		display.fillRect(228, 0, 14, 12, BLACK);
  		display.drawRect(228, 0, 14, 12, BLACK);
  	}
  	else{
  		display.fillRect(204, 0, 14, 12, GREEN);
  		display.drawRect(204, 0, 14, 12, BLACK);
  		display.fillRect(216, 0, 14, 12, GREEN);
  		display.drawRect(216, 0, 14, 12, BLACK);
  		display.fillRect(228, 0, 14, 12, GREEN);
  		display.drawRect(228, 0, 14, 12, BLACK);
  	}
  	display.setCursor(90, 0);
  	if (Data.hour < 10)
  		display.print("0");
  	display.print(Data.hour);
  	display.print(":");
  	if (Data.minute < 10)
  		display.print("0");
  	display.print(Data.minute);
  	display.drawLine(0, 20, 240, 20, RED);
}
