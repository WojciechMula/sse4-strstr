size_t strstr_naive(const char * hay, int size, const char *needle, int needlesize) {
  const char first = needle[0];
  const int maxpos = size - needlesize;
  for(int i = 0; i < maxpos; i++) {
    if(hay[i] != first) {
       i++;
       while( i < maxpos && hay[i] != first ) i++;
       if ( i == maxpos ) break;
    }
    int j = 1;
    for( ; j < needlesize; ++j)
      if(hay[ i + j ] != needle[ j ] ) break;
    if( j == needlesize) return i;
  }
  return std::string::npos;
}
