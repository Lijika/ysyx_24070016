#include "sdb.h"
#include <common.h>

#define MAX_DEVICES 3  // 设备最大数量

typedef struct {
  const char *name;       // 设备名
  uint32_t addr;          // 设备地址
  bool log_reads;         // 是否记录读操作
  bool log_writes;        // 是否记录写操作
  uint64_t last_trace_time; // 上次访问时间
  uint32_t last_data;     // 上次读取数据
  uint64_t min_interval;  // 最小时间间隔 (us)
} DeviceTrace;

#ifdef CONFIG_DEVICE
static DeviceTrace device_table[MAX_DEVICES] = {
  {"rtc", CONFIG_RTC_MMIO, true, false, 0, 0, 1000000},         // RTC 设备 (只记录读，每 1s 记录一次)
  {"serial", CONFIG_SERIAL_MMIO, false, true, 0, 0, 1000000},    // 串口 (只记录写，每 1s 记录一次)
};

void dtrace_log(uint32_t pc, const char *dev, uint32_t addr,int len, uint32_t data, bool is_write) {
  uint64_t now = get_time();

  // 遍历设备表，查找匹配设备
  for (int i = 0; i < MAX_DEVICES; i++) {
    if (device_table[i].addr == addr) {
      DeviceTrace *dev = &device_table[i];

      // 判断是否需要记录
      if ((is_write && !dev->log_writes) || (!is_write && !dev->log_reads)) {
        return;  // 该操作类型不需要记录
      }

      // 过滤时间间隔
      if (now - dev->last_trace_time < dev->min_interval) {
        return;
      }

      // 过滤重复数据（仅适用于读操作）
      if (!is_write && data == dev->last_data) {
        return;
      }

      Log("[dtrace] PC=0x%08x | DEV=%s | %s | Addr=0x%08x | Data=0x%08x\n",
              pc, dev->name, is_write ? "MMIO_W" : "MMIO_R", addr, data);

      // 更新设备状态
      dev->last_trace_time = now;
      if (!is_write) {
        dev->last_data = data;
      }
      return; // 设备已匹配，终止循环
    }
  }
}

#endif