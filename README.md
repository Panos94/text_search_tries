# Ngram detection tries
Using tries to run search queries in big text files.For memory and time eficiency we are using hashtables,bloomfilters and p threads.

To run :
  type make
       ./project -i text.init -q text.work
  
  The init file must contain in each line the ngrams you are looking for in the text
  The .work file must contain in each line a text (no matter the size) in which you are looking to find the ngrams given
 
