#include <am.h>

static uint64_t boot_time;

static uint64_t read_time() {
  uint32_t low  = *(volatile uint32_t *)0xa0000048;      // 读取低32位
  uint32_t high = *(volatile uint32_t *)(0xa0000048 + 4);  // 读取高32位
  uint64_t time = (((uint64_t)high) << 32) | low;  // 组合成完整的64位时间
  return time;
}

void __am_timer_init() {
  boot_time = read_time();
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint64_t now = read_time();
  uptime->us = now - boot_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
