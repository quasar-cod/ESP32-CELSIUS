time_t now;
tm tmn;
String dateYMD(){
  time(&now);
  localtime_r(&now, &tmn);
  String a = String(tmn.tm_year + 1900);
  String m = "00" + String(tmn.tm_mon + 1);// +1 per ESP8266
  String mm = m.substring(m.length()-2);
  String g = "00" + String(tmn.tm_mday);
  String gg = g.substring(g.length()-2);
  String amg = a + ":" + mm + ":" + gg;
  // Serial.println("debug");
  // Serial.println(tmn.tm_year);
  // Serial.println(tmn.tm_mon);
  // Serial.println(tmn.tm_mday);
  // Serial.println(a);
  // Serial.println(m);
  // Serial.println(mm);
  // Serial.println(g);
  // Serial.println(gg);
  return(amg);
}
String timeHMS(){
  time(&now);
  localtime_r(&now, &tmn);
  String h = "00" + String(tmn.tm_hour);
  String hh = h.substring(h.length()-2);
  String m = "00" + String(tmn.tm_min);
  String mm = m.substring(m.length()-2);
  String s = "00" + String(tmn.tm_sec);
  String ss = s.substring(s.length()-2);
  String hms = hh + ":" + mm + ":" + ss;
  return(hms);
}
String timeHM(){
  time(&now);
  localtime_r(&now, &tmn);
  String h = "00" + String(tmn.tm_hour);
  String hh = h.substring(h.length()-2);
  String m = "00" + String(tmn.tm_min);
  String mm = m.substring(m.length()-2);
  String hms = hh + ":" + mm + ":00";
  return(hms);
}
String timeM(){
  time(&now);
  localtime_r(&now, &tmn);
  String m = "00" + String(tmn.tm_min);
  String mm = m.substring(m.length()-2);
  return(mm);
}
String timeS(){
  time(&now);
  localtime_r(&now, &tmn);
  String s = "00" + String(tmn.tm_sec);
  String ss = s.substring(s.length()-2);
  return(ss);
}
