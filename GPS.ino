
void logposition(float loglat, float loglong)
{
	longloglat = loglat * 1000000;
	longloglong = loglong * 1000000;
}

byte checksum(char *str)
{
	byte sum, l;
	sum = 0;
	l = len(str);
	for (int i = 0; i < l; i++)
	{
		sum ^= str[i];
	}
	return sum;
}


void sendToGoogle(Payload stcData)
{
	int latint = (int)stcData.latitude;
	int latdec = (stcData.latitude * 10000) - (latint * 10000);
	int lonint = (int)stcData.longitude;
	int londec = (stcData.longitude * 10000) - (lonint * 10000);

	// Convert altitude to a string
	char falt[8];
	dtostrf(stcData.altitude, 4, 1, falt);

	// Convert speed to a string
	char fspeed[8];
	dtostrf(stcData.groundspeed, 4, 2, fspeed);

	// Convert track to a string
	char ftrack[8];
	dtostrf(stcData.track, 4, 2, ftrack);

	// Convert HDOP to a string
	char fHDOP[4];
	dtostrf(stcData.HDOP, 1, 2, fHDOP);

	// Convert geoid height to a string
	char fgeoh[8];
	dtostrf(stcData.geoidheight, 4, 1, fgeoh);

	//$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
	char *j = gpsstr1;
	j += sprintf(j, "GPGGA,");
	j += sprintf(j, "%.2d%.2d%.2d.%3d,", stcData.hour, stcData.minute, stcData.seconds, stcData.miliseconds); //123519 Fix taken at 12:35 : 19 UTC
	j += sprintf(j, "%.4d.%.4d,%c,", latint, latdec, stcData.lat); // 4807.038,N Latitude 48 deg 07.038' N
	j += sprintf(j, "%.5d.%.4d,%c,", lonint, londec, stcData.lon); // 4807.038,N Latitude 48 deg 07.038' N
	j += sprintf(j, "1,");                    //   1 Fix quality : 1 - Must always be 1 or we wouldn't be here
	j += sprintf(j, "%.2d,", stcData.satellites);        //   08           Number of satellites being tracked
	j += sprintf(j, "%s,", fHDOP);
	j += sprintf(j, "%s,M,", falt);       //   545.4, M      Altitude, Meters, above mean sea level
	j += sprintf(j, "%s,M,,", fgeoh); // Geoid height

	char hexCS1[2];
	sprintf(hexCS1, "%02X", checksum(gpsstr1));
	Serial.print F("$"); Serial.print(gpsstr1); Serial.print F("*"); Serial.println(hexCS1);

	//$GPRMC,233913.000,A,3842.9618,N,00916.8614,W,0.50,50.58,180216,,,A*4A
	char *k = gpsstr2;
	k += sprintf(k, "GPRMC,");
	k += sprintf(k, "%.2d%.2d%.2d.%3d,", stcData.hour, stcData.minute, stcData.seconds, stcData.miliseconds);
	k += sprintf(k, "A,"); // A = OK
	k += sprintf(k, "%.4d.%.4d,%c,", latint, latdec, stcData.lat); // 4807.038,N Latitude 48 deg 07.038' N
	k += sprintf(k, "%.5d.%.4d,%c,", lonint, londec, stcData.lon); // 4807.038,N Latitude 48 deg 07.038' N
	k += sprintf(k, "%s,", fspeed);
	k += sprintf(k, "%s,", ftrack);
	k += sprintf(k, "%.2d%.2d%.2d,,,A", stcData.day, stcData.month, stcData.year);

	char hexCS2[2];
	sprintf(hexCS2, "%02X", checksum(gpsstr2));
	Serial.print F("$"); Serial.print(gpsstr2); Serial.print F("*"); Serial.println(hexCS2);
}
