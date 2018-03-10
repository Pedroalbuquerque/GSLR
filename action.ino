
void menuAction(uint8_t button, Cursor myCursor){

	// depending on the menu and submenu actions must be executed
	switch(myCursor.menuIdx){
		case 0: // myCursor.navigate
			if(button == btnSELECT){
					#ifdef BUZZER
						Blink(BUZZ, 5);
					#endif
					DEBUG_MSG("[action]fix POS *****************\n");
					fixposition();
			}
			break;
		case 1: // GPS info
										//No action for B2
			break;
		case 2: // Maximum
										//No action for B2
			break;
		case 3: // Coordinates
										//No action for B2
			break;
		case 4: // Model recovery
										//No action for B2
			break;
		case 5: // Utils Adjust LCD brightness
			#ifdef LCD
				switch(button){

						case btnSELECT:
										myCursor.navigate = navMENU;
										myCursor.subIdx = 0;
								break;
						case btnUP:
								level = level + 51;
								if (level > 255) level = 0;
								analogWrite(PIN_LCD_LIGHT, level);
								break;
						case btnDOWN:
								level = level - 51;
								if (level < 0) level = 255;
								analogWrite(PIN_LCD_LIGHT, level);
								break;
				}
			#endif
			break;
		case 6: // LOG erase
				if(button == btnSELECT){
					#ifdef BUZZER
						Blink(BUZZ, 10);
					#endif
					displaySetCursor(1, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
					displaySetCursor(2, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
					displaySetCursor(3, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
					displaySetCursor(2, 0); sprintf(strPRT, "ERASING LOG..."); display.print(strPRT);
					mylog.eraseData(); //erase LOG memory on Select
					displaymenu(myCursor.menuIdx, true);
					// uppon action, return to menu myCursor.navigate mode
					myCursor.navigate = navMENU;
					myCursor.subIdx=0;
				}
			break;
		case 7: // dump log to Googlemaps
			//  display some activity message
			if(button == btnSELECT){

				#ifdef BUZZER
					Blink(BUZZ, 10);
				#endif
				displaySetCursor(1, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
				displaySetCursor(2, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
				displaySetCursor(3, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
				displaySetCursor(2, 0); sprintf(strPRT, "Dumping LOG..."); display.print(strPRT);
				uint16_t logStart = mylog.nextRead;
				Payload logData;

				Serial.println F("\n********   Log dump ********");
				// Read data from log and send it to Google as data is read
				noInterrupts(); // so that no additional log data is saved
				for (uint16_t i = 1; i < mylog.numRecords; i++)
				{
					mylog.readData(logData);
					sendToGoogle(logData);
				}
				mylog.nextRead = logStart;	// reposition nextRead pointer for next read if necessary
				interrupts();
				displaymenu(myCursor.menuIdx, true);

				// uppon action, return to menu myCursor.navigate mode
				myCursor.navigate = navMENU;
				myCursor.subIdx=0;
			}
			break;

	}

}


void menuActionOld(uint8_t button, Cursor mycursor){


	if (button == 1)  // process button 1 if pressed - Menu navigation
	{
		//button1 = 0;
		//changeMenu();
		displaymenu(menuCursor.menuIdx, true);
		warningLevel = setflag(warningLevel, 0xFF, FLAGRESET); // reset all warning to force re-evaluation
	}
	if (button == 2)// process button 2 if pressed - Function within Menu
	{
		//button2 = 0;
		switch (menuCursor.menuIdx)
		{
		case 1: // myCursor.navigate
		#ifdef BUZZER
			Blink(BUZZ, 5);
		#endif
			fixposition();
			break;
		case 2: // GPS info
										//No action for B2
			break;
		case 3: // Maximum
										//No action for B2
			break;
		case 4: // Coordinates
										//No action for B2
			break;
		case 5: // Model recovery
										//No action for B2
			break;
		case 6: // Utils Adjust LCD brightness
			#ifdef LCD
				level = level + 51;
				if (level > 255) level = 0;
				analogWrite(PIN_LCD_LIGHT, level);
			#endif
			break;
		case 7: // LOG erase
			#ifdef BUZZER
				Blink(BUZZ, 10);
			#endif
			displaySetCursor(1, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
			displaySetCursor(2, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
			displaySetCursor(3, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
			displaySetCursor(2, 0); sprintf(strPRT, "ERASING LOG..."); display.print(strPRT);
			mylog.eraseData(); //erase LOG memory on B2
			displaymenu(menuCursor.menuIdx, true);
			break;
		case 8: // dump log to Googlemaps
			//  display some activity message
			#ifdef BUZZER
				Blink(BUZZ, 10);
			#endif
			displaySetCursor(1, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
			displaySetCursor(2, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
			displaySetCursor(3, 0); display.print(fill(strPRT, SCRCHARS, ' ', true));
			displaySetCursor(2, 0); sprintf(strPRT, "Dumping LOG..."); display.print(strPRT);
			uint16_t logStart = mylog.nextRead;
			Payload logData;

			Serial.println F("\n********   Log dump ********");
			// Read data from log and send it to Google as data is read
			noInterrupts(); // so that no additional log data is saved
			for (uint16_t i = 1; i < mylog.numRecords; i++)
			{
				mylog.readData(logData);
				sendToGoogle(logData);
			}
			mylog.nextRead = logStart;	// reposition nextRead pointer for next read if necessary
			interrupts();
			displaymenu(menuCursor.menuIdx, true);
			break;
		}
		#ifdef LCD
			display.display();
		#endif
		warningLevel = setflag(warningLevel, 0xFF, FLAGRESET); // reset all warning to force re-evaluation
	}

}
