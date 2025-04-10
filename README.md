Na arduino plocicu je spojena jedna LED dioda na pin 8 koja blinka 5 puta nakon čega se pokreće sleep mode

funckije sleep modea:
sleep_enable();
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  
  sleep_cpu();  // Enter sleep mode

  prekidanje sleep mode-a se događa klikom na pushbutton na pinu 2 ili internim timerom koji je postaljen na 16 sekundi
