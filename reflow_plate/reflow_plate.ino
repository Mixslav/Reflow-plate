// Mislav Stipić 2022
// "Reflow plate" za lemljenje SMD komponenti na PCB ploče

// pinovi
int ThermistorPin = A0;  // termistor
int relay_pin = 8;       // relej

float temp_value_1 = 170;  // željena vrijednost temperature
float temp_value_2 = 190;  //
//                  ^^^^
String temp = "1"; //(1 ili 2 temp)

// za računat temperaturu
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

// računanje prosjeka temp
float niz_temp[5] = {};  // matrica vrijednosti zadnjih 5 mjerenja
float avr;               // var za rčunanje prosjeka

// vrijeme za paljenje ledice
int ledState = LOW;                      // ledState used to set the LED
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long interval = 1000;           // interval at which to blink (milliseconds)


unsigned long predhodno = 0;   // za racunanje prosjeka
unsigned long trajanje = 500;  // mjeri svako 500 ms



void setup() {
  
Serial.begin(115200);


pinMode(LED_BUILTIN, OUTPUT); // indikator grijanja

pinMode(relay_pin, OUTPUT); // digitalni izlaz za kontrolu releja

digitalWrite(relay_pin, HIGH);  // high je za ugasit, low je za upalit reflow plate
}



void loop() {
  
    unsigned long currentMillis = millis();


// mjerenje temperature

  if (currentMillis - predhodno >= trajanje) {

    predhodno = currentMillis;    // save the last time you blinked the LED
  
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15;
  


  for(int i = 4; i>=0; i--){
    
    
    niz_temp[i] = niz_temp[i-1];    // spremi zadnja 4 mjerenja
  }
  
 niz_temp[0] = Tc;                 // spremi novo mjerenje u prvi član
  
 avr = (niz_temp[0] + niz_temp[1] + niz_temp[2] + niz_temp[3] + niz_temp[4]) / 5; // prosjek zadnjih 5 mjerenja
    

  Serial.println(" Temperatura: "); 
  Serial.print(avr);
  Serial.print(" C");

  }

      currentMillis = millis();
      
  if (currentMillis - previousMillis >= interval) {
    
    interval = map(Tc, 0, 255, 700, 10);  // blinkaj ledicu ovisno o intervalu
    previousMillis = currentMillis;       // save the last time you blinked the LED

    if (ledState == LOW) {                // if the LED is off turn it on and vice-versa:
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    if(avr > 170)     // ako je prosječna temp veća od 170 , upali signalnu ledicu kontinuiranu što znači da je ploča spremna za rad
    ledState = HIGH;


    digitalWrite(LED_BUILTIN, ledState);    // set the LED with the ledState of the variable:


  }

if (Serial.available() > 0)     // unosimo 1 ili 2 za kontrolu u konzoli, 2 je za ponovo paljenje kad temperatura padne ispod 170
  temp = Serial.readString();

  //Serial.println(temp); 

// ON / OFF upravljanje


if(temp == "1"){ 
  if( avr < temp_value_1){
    
  digitalWrite(relay_pin, LOW);     // ako je temperatura unutar +/- 5% upali ploču   (negirano upravljanje)
  }


  if( avr >= temp_value_1 + 3){
    
  digitalWrite(relay_pin, HIGH);     // ako je temperatura unutar +/- 3°C ugasi ploču   (negirano upravljanje)
  }
  
}

if(temp == "2"){
  if( avr < temp_value_2){
    
  digitalWrite(relay_pin, LOW);     // ako je temperatura unutar +/- 5% upali ploču   (negirano upravljanje)
  }


  if( avr >= temp_value_2 + 3){
    
  digitalWrite(relay_pin, HIGH);     // ako je temperatura unutar +/- 3°C ugasi ploču  (negirano upravljanje) 
  }
  
}


}



///
