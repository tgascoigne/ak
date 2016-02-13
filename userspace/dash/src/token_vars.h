
/* Array indicating which tokens mark the end of a list */
static const char tokendlist[] = {
    1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1,
};

static const char *const tokname[] = {
    "end of file", "newline",   "\";\"",     "\"&\"",       "\"&&\"", "\"||\"",  "\"|\"",    "\"(\"",
    "\")\"",       "\";;\"",    "\"`\"",     "redirection", "word",   "\"!\"",   "\"case\"", "\"do\"",
    "\"done\"",    "\"elif\"",  "\"else\"",  "\"esac\"",    "\"fi\"", "\"for\"", "\"if\"",   "\"in\"",
    "\"then\"",    "\"until\"", "\"while\"", "\"{\"",       "\"}\"",
};

#define KWDOFFSET 13

static const char *const parsekwd[] = {"!",   "case", "do", "done", "elif",  "else",  "esac", "fi",
                                       "for", "if",   "in", "then", "until", "while", "{",    "}"};
