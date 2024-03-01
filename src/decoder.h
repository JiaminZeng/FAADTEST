
#include "common.h"
#include "structs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mp4.h"
#include "syntax.h"
#include "error.h"
#include "output.h"
#include "filtbank.h"
#include "drc.h"

#ifdef SBR_DEC

#include "sbr_dec.h"
#include "sbr_syntax.h"

#endif
#ifdef SSR_DEC
#include "ssr.h"
#endif

#ifdef ANALYSIS
uint16_t dbg_count;
#endif