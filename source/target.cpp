#include "target.h"

#include "swd_interface.h"

#include "libs/error/error_stack.h"
#include "libs/log/log_default.h"

using namespace Err;
using namespace Log;

enum MEMAPRegister {
  kMEMAP_CSW = 0x00,
  kMEMAP_TAR = 0x04,
  kMEMAP_DRW = 0x0C,
};

static uint8_t reg_in_bank(uint8_t address) {
  return (address & 0xF) >> 2;
}

Target::Target(DebugAccessPort *dap, uint8_t mem_ap_index)
    : _dap(*dap),
      _mem_ap_index(mem_ap_index),
      _current_ap_bank(-1) {}

Error Target::select_bank_for_address(uint8_t address) {
  uint8_t bank = address >> 4;
  if (_current_ap_bank != bank) {
    Check(_dap.select_ap_bank(_mem_ap_index, bank));
    _current_ap_bank = bank;
  }
  return success;
}

Error Target::write_ap(uint8_t address, uint32_t data) {
  Check(select_bank_for_address(address));

  return _dap.write_ap_in_bank(reg_in_bank(address), data);
}

Error Target::post_read_ap(uint8_t address) {
  Check(select_bank_for_address(address));

  return _dap.post_read_ap_in_bank(reg_in_bank(address));
}

Error Target::read_ap_pipelined(uint8_t nextAddress, uint32_t *lastData) {
  Check(select_bank_for_address(nextAddress));
  return _dap.read_ap_in_bank_pipelined(reg_in_bank(nextAddress), lastData);
}

Error Target::initialize() {
  // We only use one AP.  Go ahead and select it and configure CSW.
  Check(write_ap(kMEMAP_CSW, 2));  // Configure for 4-byte transactions.

  return success;
}

Error Target::read_words(uint32_t target_addr,
                         void *host_buffer,
                         size_t count) {
  uint32_t *host_buffer_as_words = static_cast<uint32_t *>(host_buffer);

  // Configure MEM-AP for auto-incrementing 32-bit transactions.
  Check(write_ap(kMEMAP_CSW, (1 << 4) | 2));
  
  // Load Transfer Address Register with first address.
  Check(write_ap(kMEMAP_TAR, target_addr));

  // Transfer using pipelined reads.
  Check(post_read_ap(kMEMAP_DRW));
  for (size_t i = 0; i < count; ++i) {
    Check(read_ap_pipelined(kMEMAP_DRW, &host_buffer_as_words[i]));
  }

  return success;
}

Error Target::write_words(void const *host_buffer, uint32_t target_addr,
                          size_t count) {
  uint32_t const *host_buffer_as_words =
      static_cast<uint32_t const *>(host_buffer);

  // Configure MEM-AP for auto-incrementing 32-bit transactions.
  Check(write_ap(kMEMAP_CSW, (1 << 4) | 2));
  
  // Load Transfer Address Register with first address.
  Check(write_ap(kMEMAP_TAR, target_addr));

  for (size_t i = 0; i < count; ++i) {
    Check(write_ap(kMEMAP_DRW, host_buffer_as_words[i]));
  }

  return success;
}

