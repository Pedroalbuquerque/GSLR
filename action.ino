

void menuAction(uint8_t button){


	if (button == 1)  // process button 1 if pressed - Menu navigation
	{
		//button1 = 0;
		changeMenu();
		displaymenu(menuPage, true);
		warningLevel = setflag(warningLevel, 0xFF, FLAGRESET); // reset all warning to force re-evaluation
	}
	if (button == 2)// process button 2 if pressed - Function within Menu
	{
		//button2 = 0;
		switch (menuPage)
		{
		case 1: // Navigate
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
			displaymenu(menuPage, true);
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
			displaymenu(menuPage, true);
			break;
		}
		#ifdef LCD
			display.display();
		#endif
		warningLevel = setflag(warningLevel, 0xFF, FLAGRESET); // reset all warning to force re-evaluation
	}

}
