# split words
cat $1 \
  | tr -s -c "a-zA-Z" "\n" \
  | sort -u \
  > $2
