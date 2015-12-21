# xmail-dspam-filter

Simple XMail filter for use with DSPAM (http://dspam.nuclearelephant.com) daemon.

**Configure:**

If you want output with CRLF line endings, you don't need to do anything.  
If you want output with LF line endings, remove `#define OutputCRLF`.

**Compile:**

gcc -o dspam-filter dspam-filter.c

**Install:**

Put dspam-filter anywhere you want, dspam-check.tab in \<mail-root\>/filters/ and
filters.in.tab in \<mail-root\>/ (edit as needed).
