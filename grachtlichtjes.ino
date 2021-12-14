#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    2

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 200
#define COLUMNS 10
#define ROWS 20

// Brians brain cells have three states, 0=dead,1<alive,-1=resting
// The age is a positive integer
int cell[ROWS][COLUMNS];
long age[ROWS][COLUMNS];
// int livingneighbours[ROWS][COLUMNS];
int rows = ROWS;
int columns = COLUMNS;
int wait = 10;
int brainage = 0;
int fireTime = 1000;
int restTime = 1000;
int inactive = 10000;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  randomStart(wait);
  brainage=0;
}

void loop() {
  long latest;
  long t = millis();
  if (briansBrain(t,wait)) {latest = t;}
  if ((t-latest) > inactive) {
    randomStart(10);
    Serial.print(t);
    Serial.println(" restart");
  }
}

/********************************************************************
 * In each time step, a cell turns on if it was off but had exactly 
 * two neighbors that were on, just like the birth rule for Seeds. 
 * All cells that were "on" go into the "resting" state, which is not 
 * counted as an "on" cell in the neighbor count, and prevents any 
 * cell from being born there. Cells that were in the resting state 
 * go into the off state. 
 */
boolean briansBrain(long t, int wait) {
  int ir, ig, ib;
  long icol = random(0,columns);
  long irow = random(0,rows);

  if (cell[irow][icol] ==  1) { // Firing cell goes to rest after firing period
    if ((t-age[irow][icol]) < fireTime) {
      return false;
    } else {
      cell[irow][icol]=-1;
      Serial.print(t);
      Serial.println(" rest");
      age[irow][icol] = t;
      ir=0; ig=250; ib=0;
    }
  } else if (cell[irow][icol] == -1) { // Resting cell goes off after rest period
    if ((t-age[irow][icol]) < restTime) {
      return false;
    } else {
      cell[irow][icol]=0;
      Serial.print(t);
      Serial.println(" off");
      age[irow][icol] = t;
      ir=50; ig=50; ib=50;
    } 
  } else if (cell[irow][icol] == 0) { // Ready cell fires if it has two firing neighbours
    int living = livingNeighbours(irow,icol);
    if (living!= 2) {
      return false;
    } else {
      cell[irow][icol]=1;
      Serial.print(t);
      Serial.println(" fire");
      age[irow][icol] = t;
      ir=250; ig=0; ib=0;
    }
  }
  strip.setPixelColor(rowcolumn(irow,icol), strip.Color(ig,ir,ib)); //  Set pixel's color (in RAM)
  strip.show();                  //  Update strip to match
  delay(wait);
 
  return true;
}

/**
 * Returns the index of a row and column
 * 0 9 10
 * 1 8 11
 * 2 7 12
 * 3 6 etc
 * 4 5
 * 
 */
int rowcolumn(int irow, int icol) {
  int i;
  irow = irow % rows;
  icol = icol % columns;
  if (icol%2==0) {
    i = (irow + icol*rows);
  } else {
    i = (rows-1 - irow + icol*rows);
  }
  // Serial.print("row ");
  // Serial.print(irow);
  // Serial.print(" col ");
  // Serial.print(icol);
  // Serial.print(" index ");
  // Serial.println(i);
  return i;
}

/***************************************
 * Count the number of living neighbours
 */
int livingNeighbours(int irow,int icol) {
  int alive = 0;
  for (int ixc=-1;ixc<2;ixc++) {
    for (int iyc=-1;iyc<2;iyc++) {
      if (!(ixc==0 && iyc==0)) {
        if (cell[irow+iyc][icol+ixc]>0) {alive=alive+1;}
      }
    }
  }
  return alive;
}

/*********************************************************
 * Start with a random field of firing and resting neurons
 */
void randomStart(int wait) {
  // Fade from black to white
  // for (int iw=0;iw<50;iw++) {
  //   allLights(iw,iw,iw);
  // }
  int ir, ig, ib;
  for (int icol=0;icol<columns;icol++) {
    for (int irow=0;irow<rows;irow++) {
      long r = random(-1,2);
      if (r == 1) {
        cell[icol][irow] = 1;
        ir=250, ig=0; ib=0;  
      } else if (r == -1) {
        cell[icol][irow]=-1;
        ir=0, ig=250; ib=0;
      } else {
        ir=50, ig=50; ib=50;
      }
      strip.setPixelColor(rowcolumn(irow,icol), strip.Color(ig,ir,ib)); //  Set pixel's color (in RAM)
      strip.show();                  //  Update strip to match
    }
    delay(wait);
  }
  Serial.println("Random start done");
}

/***********************************
 * Set all lights to the same colour
 */
void allLights(int ir, int ig, int ib) { 
  for (int icol=0;icol<columns;icol++) {
    for (int irow=0;irow<rows;irow++) {
      strip.setPixelColor(rowcolumn(irow,icol), strip.Color(ig,ir,ib)); //  Set pixel's color (in RAM)
      strip.show();                  //  Update strip to match
    }
  }
}

/**
 * Return a random colour with high saturation and value
 */
void randomColour() {
  int hue = random(0,360);
}
