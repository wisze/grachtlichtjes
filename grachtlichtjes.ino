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
int livingneighbours[ROWS][COLUMNS];
int rows = ROWS;
int columns = COLUMNS;
int wait = 0;
int brainage = 0;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  randomStart(10);
  brainage=0;
}

void loop() {
  // put your main code here, to run repeatedly:
  int l = briansBrain(5);
  brainage += 1;
  Serial.print("Age ");
  Serial.print(brainage);
  Serial.print(", ");
  Serial.print(l);
  Serial.println(" cells alive.");
  if (l==0 || brainage>100) {randomStart(10); brainage=0;}
}

/********************************************************************
 * In each time step, a cell turns on if it was off but had exactly 
 * two neighbors that were on, just like the birth rule for Seeds. 
 * All cells that were "on" go into the "resting" state, which is not 
 * counted as an "on" cell in the neighbor count, and prevents any 
 * cell from being born there. Cells that were in the resting state 
 * go into the off state. 
 */
int briansBrain(int wait) {
  int living=0;

  for (int i=0;i<rows*columns;i++) {
    long icol = random(0,columns);
    long irow = random(0,rows);
    livingneighbours[irow][icol] = livingNeighbours(irow,icol);  
    int ir=50, ig=50, ib=50;
    // Resting cells go off
    if (cell[irow][icol] == -1) {
      cell[irow][icol]=0;
      ir=50; ig=50; ib=50;
    } 
    // Firing cells start resting
    if (cell[irow][icol] == 1) {
      cell[irow][icol]=-1;
      ir=0; ig=250; ib=0;
    } 
    // Off cells start firing if they have two living neighbours
    if (cell[irow][icol] == 0 && livingneighbours[irow][icol] == 2) {
      cell[irow][icol]=1;
      ir=250; ig=0; ib=0;
      living += 1;
    }
    strip.setPixelColor(rowcolumn(irow,icol), strip.Color(ig,ir,ib)); //  Set pixel's color (in RAM)
    strip.show();                  //  Update strip to match
    delay(wait);
  }
  return living;
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
    i = (rows - irow + icol*rows);
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
