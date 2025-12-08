//
// Created by 0xtriboulet on 12/7/2025.
//

#ifndef EMERALD_TEMPLATE_HELPERS_H
#define EMERALD_TEMPLATE_HELPERS_H

#ifndef _DEBUG
#define DFR(module, function) \
DECLSPEC_IMPORT decltype(function) module##$##function;

#define DFR_LOCAL(module, function) \
DECLSPEC_IMPORT decltype(function) module##$##function; \
decltype(module##$##function) * function = module##$##function;
#else
#define DFR_LOCAL(module, function)
#define DFR(module, function) \
decltype(function) *module##$##function = function;
#endif // end of _DEBUG

#endif //EMERALD_TEMPLATE_HELPERS_H