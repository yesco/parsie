x// Type - type accessing in memory
//
// This is used to encode access of
// data elewment from structured storage.
// Similarly to perl's pack/unpack, it
// contains instructions to access data.
//
//
// Letter followed by count, *=till end
//
// - Numbers
// unsigned signed bytes
//     c       C     1  char
//     s       S     2  short
//     i       I     4  int   (perl:l L)
//     q       Q     8  quad
//     f             4  float
//     d             8  double
//             D    16  long double
//     p       P     8  zt-str ptr/struct

// - Encodings
//     n       N    2/4 unsign netw BigEnd
//     v       V    2/4 SmallEnd
//             W     V  char>256 ? base-128
//     w             V  BER MSG base=129
//     j       J     ?  internal int (pl)

// - Charcters/strings
//     a       A     1  fixed bindary/ascii
//             Z     N  zero-terminated
//     
//     b       B        bit=strings
//     h       H        hex
//     u       U        uuencoded

// - Positioning
//     x       X     1  forward/back
//     @             V  null-fill till N
//     @.            V  byte-offset
//     .          +/-V  null-fill/trunc

//     (       )     V  grouping
//     !x      !X    V  align
//    ...[d]   read length from stream

//     /                #items follows


// Object= "(dddd(dd)6)"
// "(proto n reserved next props (name val))6"
// "(proto:d n:d reserved:d next:d props:(name:d val:d)6 )"

// A Type is an Object:

// "T:Object" == {
//    proto = inherited from;
//    props: [ (field, offset) ]
//           [ (no, field) ]


