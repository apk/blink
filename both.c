// Duemilanove w/ ATmega328
// Uno

#include <FastLED.h>
#define NUM_LEDS 7
#define DATA_PIN 6

CRGB leds[NUM_LEDS];

void setup() { 
   FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
   Serial.begin(9600);
}

#define LIM 251

struct SV {
  int val, dv, st, ac;
  void operator= (int x) { val = x; }
  void nxt(int c, int lm) {
    if (c & 1) {
      val = val < lm ? val + 1 : lm;
    } else {
      val = val > 0 ? val - 1 : 0;
    }
  }
  void set (int nv, int dl) {
    if (dl > 0) {
      dv = nv - val;
      st = dl;
      ac = st / 2;
    } else {
      val = nv;
    }
  }
  void ds () {
    if (dv >= 0) {
      ac += dv;
      val += (ac / st);
      ac %= st;
    } else {
      ac -= dv;
      val -= (ac / st);
      ac %= st;
    }
  }
  operator int() { return val; }
};

struct B {

  int t, i, lm, wt, dl;
  SV r, g, b;

  B (int l = LIM) {
     lm = l;

     r = 33;
     g = 33;
     b = 33;

     t = 0;
     i = 0;
     wt = 0;
     dl = 0;
  }

  void set (int d, int nr, int ng, int nb) {
    r.set (nr, d);
    g.set (ng, d);
    b.set (nb, d);
    dl = d;
    if (wt < 250) wt = 100;
  }

  void ena (int bitf, int onoff) {
    if ((onoff & 1) && (bitf & 1)) {
      wt = 0;
    } else if ((onoff & 2) && !(bitf & 1)) {
      wt = 255;
    }
  }

  void stpx(int lim) {
    if (dl > 0) {
        dl --;
        r.ds ();
        g.ds ();
        b.ds ();
        return;
    }
    if (wt > 250) return;
    if (wt > 0) {
      wt --;
      return;
    }
    if (i < 1) {
      i = lim;
      t = random(7)+0;
    }
    i --;
    r.nxt(t, lm);
    g.nxt(t>>1, lm);
    b.nxt(t>>2, lm);
  }

  void stp(int lim, CRGB &rgb) {
    stpx(lim);
    rgb = CRGB::CRGB(g,r,b); 
  }
};

B b[NUM_LEDS], q (250);

int wd = 0;
int wv = 0;

void loop() {
//  for(int i = 0; i < 100; i ++) {
    
  //  leds[2] = CRGB::CRGB(i,100,0); 
  for (int i = 0; i < 7; i ++) {
    b[i].stp(LIM-i,leds[i]);
  }
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

void ena (int bits, int onoff) {
  q.ena (bits >> 7, onoff);
  for (int i = 0; i < 7; i++) {
    b[i].ena (bits >> i, onoff);
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
      b[0].set (rq, ra, rb, rc);
    } else if (v == 'j') {
      b[1].set (rq, ra, rb, rc);
    } else if (v == 'k') {
      b[2].set (rq, ra, rb, rc);
    } else if (v == 'l') {
      b[3].set (rq, ra, rb, rc);
    } else if (v == 'n') {
      b[4].set (rq, ra, rb, rc);
    } else if (v == 'o') {
      b[5].set (rq, ra, rb, rc);
    } else if (v == 'p') {
      b[6].set (rq, ra, rb, rc);
    } else if (v == 'E') {
      ena (rc, 1);
    } else if (v == 'D') {
      ena (~rc, 2);
    } else if (v == 'F') {
      ena (rc, 3);
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
