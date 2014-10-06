void Display(){
  if(currentpage!=lastpage){
    lcd.clear();
    lastpage = currentpage;
  }
  page_info();
  lcd.setCursor(0,0);
  switch(currentpage){
    case 1:
      lcd.print("ODB-II Interface");
      lcd.setCursor(0,1);
      lcd.print("BT Connection Status");
      lcd.setCursor(0,2);
      lcd.print("Link:");
      if(BT_state){  
        lcd.print("Established!   ");
        lcd.setCursor(0,3);
        lcd.print("Entering now ODB-II!");
        delay(1000);
        currentpage=2;
      }
      else
        lcd.print("Down! Retrying!");
    break;
    case 2:
      lcd.setCursor(0,0);
      lcd.print("Init ELM327 v1.5");
      lcd.setCursor(0,1);
      lcd.print("Set auto protocol ");
      if(ODBcmd("ATSP0")==true){
        lcd.write(0);
        lcd.setCursor(0,2);
        lcd.print("Retrieve PID's ");
        if(ODBcmd("0100")==true){
          lcd.write(0);
          delay(1000);
          currentpage=3;
        }
        else
          lcd.write(1); 
      }
      else{
        lcd.write(1);
        lcd.setCursor(0,2);
        lcd.print("Operation failed!");
        lcd.setCursor(0,3);
        lcd.print("Retrying to Init!");
        delay(1000);
        lcd.clear();
      }
    break;
    case 3:
      lcd.setCursor(0,0);
      lcd.print("RPM:");
      lcd.print(readRPM());
      delay(1000);
    break;
    case 4:
    break;
  }
}

void switch_page_up() {
  lastpage = currentpage;
  currentpage++;
  if(currentpage > pages) {
    currentpage = 1;
    lastpage = pages;
  }
}

void switch_page_down() {
  lastpage = currentpage;
  currentpage--;
  if(currentpage < 1) {
    currentpage = pages;
    lastpage = 1;
  }
}

void page_info() {
  if(pages > 9) {
    lcd.setCursor(16,0);
  } else {
    lcd.setCursor(17,0);
  }
  lcd.print(currentpage);
  lcd.print("/");
  lcd.print(pages);
}

