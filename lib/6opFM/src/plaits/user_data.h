// user_data.h
#pragma once
#include "plaits/resources.h"

namespace plaits {

struct UserData {
  const uint8_t* ptr(int engine_index) const {
    switch(engine_index) {
      case 0: return syx_bank_0;
      case 1: return syx_bank_1;
      case 2: return syx_bank_2;
      case 3: return syx_bank_3;
      case 4: return syx_bank_4;
      case 5: return syx_bank_5;
	  case 6: return syx_bank_6;
      case 7: return syx_bank_7;
      default: return syx_bank_0;  // fallback
    }
  }
};

}  // namespace plaits