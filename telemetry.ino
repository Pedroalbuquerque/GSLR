void telemetryUpdate(){

  if (Data.fix == 1){	// if GPS fix acquired

    if (fixinMem == 0) fixposition();
      fixinMem = 1;

    warningLevel = setflag(warningLevel, WRN_FIX, FLAGRESET);

    // Run the distance calculating function, passing the memorized position as arguments
    kmflag = GPSDist(homelat, homelon, Data.latitudedeg, Data.longitudedeg, &homedist, &homeazim);


    if (Data.altitude > maxalt + homealt)	// check maximum altitude
      maxalt = Data.altitude - homealt;

    if (kmflag == 0)	// check maximum distance
    {
      kmflagmem = kmflag;
      if (homedist > maxdist)
        maxdist = homedist;
    }
    else
    {
      if (kmflagmem == 0)
      {
        kmflagmem = 1;
        maxdist = homedist;
      }
      else if (homedist > maxdist)
        maxdist = homedist;
    }

    //check maximum speed
    if (Data.groundspeed*1.852 < 1)
      Data.groundspeed = 0;
    if (Data.groundspeed*1.852 > highspeed)
      highspeed = Data.groundspeed*1.852;
  }
  else
    warningLevel = setflag(warningLevel, WRN_FIX, FLAGSET);


}
