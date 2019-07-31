#pragma once

#include <string>
#include <vector>

#include "langInternals.h"

mylang::cell* allocateType(mylang::cell literal);
mylang::cell evalCell(mylang::cell c, mylang::env* environ);
mylang::cell call(mylang::func f, mylang::cell* args);
