/*
 * This file was generated by the mknodes program.
 */

#define NCMD 0
#define NPIPE 1
#define NREDIR 2
#define NBACKGND 3
#define NSUBSHELL 4
#define NAND 5
#define NOR 6
#define NSEMI 7
#define NIF 8
#define NWHILE 9
#define NUNTIL 10
#define NFOR 11
#define NCASE 12
#define NCLIST 13
#define NDEFUN 14
#define NARG 15
#define NTO 16
#define NCLOBBER 17
#define NFROM 18
#define NFROMTO 19
#define NAPPEND 20
#define NTOFD 21
#define NFROMFD 22
#define NHERE 23
#define NXHERE 24
#define NNOT 25

struct ncmd {
	int type;
	int linno;
	union node *assign;
	union node *args;
	union node *redirect;
};

struct npipe {
	int type;
	int backgnd;
	struct nodelist *cmdlist;
};

struct nredir {
	int type;
	int linno;
	union node *n;
	union node *redirect;
};

struct nbinary {
	int type;
	union node *ch1;
	union node *ch2;
};

struct nif {
	int type;
	union node *test;
	union node *ifpart;
	union node *elsepart;
};

struct nfor {
	int type;
	int linno;
	union node *args;
	union node *body;
	char *var;
};

struct ncase {
	int type;
	int linno;
	union node *expr;
	union node *cases;
};

struct nclist {
	int type;
	union node *next;
	union node *pattern;
	union node *body;
};

struct ndefun {
	int type;
	int linno;
	char *text;
	union node *body;
};

struct narg {
	int type;
	union node *next;
	char *text;
	struct nodelist *backquote;
};

struct nfile {
	int type;
	union node *next;
	int fd;
	union node *fname;
	char *expfname;
};

struct ndup {
	int type;
	union node *next;
	int fd;
	int dupfd;
	union node *vname;
};

struct nhere {
	int type;
	union node *next;
	int fd;
	union node *doc;
};

struct nnot {
	int type;
	union node *com;
};

union node {
	int type;
	struct ncmd ncmd;
	struct npipe npipe;
	struct nredir nredir;
	struct nbinary nbinary;
	struct nif nif;
	struct nfor nfor;
	struct ncase ncase;
	struct nclist nclist;
	struct ndefun ndefun;
	struct narg narg;
	struct nfile nfile;
	struct ndup ndup;
	struct nhere nhere;
	struct nnot nnot;
};

struct nodelist {
	struct nodelist *next;
	union node *n;
};

struct funcnode {
	int count;
	union node n;
};

struct funcnode *copyfunc(union node *);
void freefunc(struct funcnode *);
