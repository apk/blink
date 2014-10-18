// Duemilanove w/ ATmega328
// 

#include <FastLED.h>
#define NUM_LEDS 3
#define DATA_PIN 6

CRGB leds[NUM_LEDS];

void setup() { 
   FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
   Serial.begin(9600);
}

#define LIM 251

struct B {

  int r, g, b, t, i, lm, wt;

  B (int l = LIM) {
     lm = l;

     r = 33;
     g = 33;
     b = 33;

     t = 0;
     i = 0;
     wt = 0;
  }

  void set (int dl, int nr, int ng, int nb) {
    r = nr;
    g = ng;
    b = nb;
    wt = 100;
  }

  int nxt(int i, int c) {
    if (c & 1) {
      return i < lm ? i + 1 : lm;
    } else {
      return i > 0 ? i - 1 : 0;
    }
  }

  void stpx(int lim) {
    if (wt > 0) {
      wt --;
      return;
    }
    if (i < 1) {
      i = lim;
      t = random(7)+0;
    }
    i --;
    r=nxt(r,t);
    g=nxt(g,t>>1);
    b=nxt(b,t>>2);
  }

  void stp(int lim, CRGB &rgb) {
    stpx(lim);
    rgb = CRGB::CRGB(g,r,b); 
  }
};

B b1, b2, b3, q (250);

int wd = 0;
int wv = 0;

void loop() {
//  for(int i = 0; i < 100; i ++) {
    
  //  leds[2] = CRGB::CRGB(i,100,0); 
    b1.stp(LIM,leds[0]);
    b2.stp(LIM+3,leds[1]);
    b3.stp(LIM+5,leds[2]);
    q.stpx(164);
    FastLED.show();
    analogWrite(9, q.r);
    analogWrite(10, q.g);
    analogWrite(11, q.b);
    //delay(30);
 // }
  delay(40);
  wd ++;
  if (wd > 300) {
    wv ++;
    wd = 0;
    //Serial.write('A'+(wv & 31));
  }
}

int rp = 0;
int rq = 25;
int ra = 0;
int rb = 0;
int rc = 0;

void serialEvent () {
  while (Serial.available()) {
    int v = Serial.read ();
    if (v == 'd') {
      q.set (rc, 0,0,0);
    } else if (v == 'w') {
      q.set (rc, 255, 255, 255);
    } else if (v == 'r') {
      q.set (rc, 255, 0, 0);
    } else if (v == 'g') {
      q.set (rc, 0, 255, 0);
    } else if (v == 'b') {
      q.set (rc, 0, 0, 255);
    } else if (v == 'y') {
      q.set (rc, 255, 255, 0);
    } else if (v == 'm') {
      q.set (rc, 255, 0, 255);
    } else if (v == 'c') {
      q.set (rc, 0, 255, 255);
    } else if (v >= '0' && v <= '9') {
      rc = 10 * rc + (v - '0');
      continue;
    } else if (v == ',') {
      rp = rq;
      rq = ra;
      ra = rb;
      rb = rc;
      rc = 0;
      continue;
    } else if (v == '.') {
      q.set (rq, ra, rb, rc);
    } else if (v == 'i') {
      b1.set (rq, ra, rb, rc);
    } else if (v == 'j') {
      b2.set (rq, ra, rb, rc);
    } else if (v == 'k') {
      b3.set (rq, ra, rb, rc);
    } else {
      Serial.write('A'+((v >> 4) & 15));
      Serial.write('A'+(v & 15));
    }
    rp = 0;
    rq = 25;
    ra = 0;
    rb = 0;
    rc = 0;    
  }
}
