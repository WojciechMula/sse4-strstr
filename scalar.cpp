// Implementation by Daniel Lemire
// https://github.com/WojciechMula/sse4-strstr/issues/2

size_t strstr_naive(const char * hay, size_t size, const char *needle, size_t needlesize) {

  if (size == needlesize) {
    return memcmp(hay, needle, size) == 0 ? 0 : std::string::npos;
  }

  const char first = needle[0];
  const ssize_t maxpos = ssize_t(size) - ssize_t(needlesize) + 1;
  for(ssize_t i = 0; i < maxpos; i++) {
    if(hay[i] != first) {
       i++;
       while( i < maxpos && hay[i] != first ) i++;
       if ( i == maxpos ) break;
    }
    size_t j = 1;
    for( ; j < needlesize; ++j)
      if(hay[ i + j ] != needle[ j ] ) break;
    if( j == needlesize) return i;
  }
  return std::string::npos;
}
