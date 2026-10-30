#include <cstdio>
#include "shared/LedStrip.h"
#include "shared/ColorUtils.h"
uint32_t sum(Adafruit_NeoPixel &s){uint32_t t=0;for(auto b:s.buf)t+=b;return t;}
uint32_t wheel(int p){p=255-p; if(p<85)return((255-p*3)<<16)|(p*3); if(p<170){p-=85;return((p*3)<<8)|(255-p*3);} p-=170;return((p*3)<<16)|((255-p*3)<<8);}
int fails=0;
void check(const char*name,bool ok){printf("%s %s\n",ok?"PASS":"FAIL",name); if(!ok)fails++;}
int main(){
  auto &s=strips[0]; const uint32_t cap=MAX_CHANNEL_SUM_PER_STRIP;
  // rainbow: must be untouched
  for(int k=0;k<60;k++) s.setPixelColor(k,wheel(k*256/60));
  auto before=s.buf; showStrip(s);
  check("rainbow passes through unchanged", s.buf==before);
  // sunset pulse gradient: must be capped, and keep its hue ratios
  std::vector<uint32_t> sunset={0x833ab4,0xfd1d1d,0xfcb045};
  for(int k=0;k<60;k++) s.setPixelColor(k,interpolateColorBasedOnCount(sunset,k,60));
  uint32_t raw=sum(s); showStrip(s);
  printf("     sunset raw=%u capped=%u cap=%u (%.0f%% of rainbow -> %.0f%%)\n",raw,sum(s),cap,100.0*raw/cap,100.0*sum(s)/cap);
  check("sunset capped at or under budget", sum(s)<=cap);
  check("sunset not over-dimmed (>95% of budget)", sum(s)>cap*0.95);
  // white: worst case
  for(int k=0;k<60;k++) s.setPixelColor(k,0xFFFFFF);
  showStrip(s); printf("     white per-channel now %u (was 255)\n",s.buf[0]);
  check("white capped", sum(s)<=cap);
  check("show() still called", s.shows==3);
  // empty color lists no longer read garbage
  std::vector<uint32_t> none;
  check("empty gradient -> black", interpolateGradient(none,0.5f)==0);
  check("empty count interp -> black", interpolateColorBasedOnCount(none,3,60)==0);
  return fails;
}
