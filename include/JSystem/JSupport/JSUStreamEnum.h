#ifndef JSUSTREAMENUM_H
#define JSUSTREAMENUM_H

// Undefine stdio.h macros to avoid clashing with our enum values
#ifdef SEEK_SET
#undef SEEK_SET
#endif
#ifdef SEEK_CUR
#undef SEEK_CUR
#endif
#ifdef SEEK_END
#undef SEEK_END
#endif

enum JSUStreamSeekFrom { SEEK_SET = 0, SEEK_CUR = 1, SEEK_END = 2 };

// EOF from <stdio.h> is typically (-1); our enum uses 1 for the "end-of-file reached" state.
#ifdef EOF
#undef EOF
#endif

enum EIoState { GOOD = 0, EOF = 1 };

#endif
