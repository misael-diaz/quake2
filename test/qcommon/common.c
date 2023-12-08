#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define STDC17 201710L
#define ERR_ENONE 0x00000000
#define ERR_FATAL 0xffffffff
#define MAX_NUM_ARGVS 0x40
#define MAX_TOKEN_LEN 0x800
#define MAX_TOKEN_CHARS (4 * MAX_TOKEN_LEN)
#define Z_MAGIC 0x1d1d

typedef enum {
	FALSE,
	TRUE
} qboolean;

typedef unsigned char byte;

typedef struct sizebuf_s {
	byte *data;
	qboolean allowoverflow;
	qboolean overflowed;
	int maxsize;
	int cursize;
	int readcount;
} sizebuf_t;

typedef struct zhead_s {
	struct zhead_s *prev;
	struct zhead_s *next;
	short magic;
	short tag;
	int size;
} zhead_t;

typedef void (*xcommand_t) (void);

typedef struct cmd_function_s {
	struct cmd_function_s *next;
	const char *name;
	xcommand_t function;
} cmd_function_t;

int com_argc;
const char *com_argv[MAX_NUM_ARGVS];

sizebuf_t cmd_text;
byte cmd_text_buf[MAX_TOKEN_CHARS];

qboolean cmd_wait = FALSE;

zhead_t z_chain;
int z_count = 0;
int z_bytes = 0;

cmd_function_t *cmd_functions = NULL;

#if defined(__GCC__)
__attribute__ ((access (read_only, 2)))
#endif
int Qcommon_Init(int const argc, const char **argv);
int Cbuf_Init(void);
int Cmd_Init(void);
int Cbuf_AddEarlyCommands(qboolean clear);
int Cbuf_Execute(void);
int Q_Free(void);

#if defined(__GCC__)
__attribute__ ((access (read_only, 2)))
#endif
int main (int const argc, const char **argv)
{
	int rc;
	rc = Qcommon_Init(argc, argv);
	if (rc != ERR_ENONE) {
		return rc;
	}

	rc = Cbuf_Init();
	if (rc != ERR_ENONE) {
		return rc;
	}

	rc = Cmd_Init();
	if (rc != ERR_ENONE) {
		Q_Free();
		return rc;
	}

	printf("%s\n", com_argv[0]);
	qboolean clear = FALSE;
	rc = Cbuf_AddEarlyCommands(clear);
	if (rc != ERR_ENONE) {
		Q_Free();
		return rc;
	}

	rc = Cbuf_Execute();
	if (rc != ERR_ENONE) {
		Q_Free();
		return rc;
	}

	rc = Q_Free();
	if (rc != ERR_ENONE) {
		return rc;
	}

	return 0;
}

#if (__GCC__ > 12) && (__STDC_VERSION__ > STDC17)
__attribute__ ((access (write_only, 1), access (read_only, 2)))
char *va (char *dst, const char *fmt, ...)
{
	va_list args;
	va_start(args);
	vsprintf(dst, fmt, args);
	va_end(args);
	return dst;
}
#else
char *va (char *dst, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf(dst, fmt, args);
	va_end(args);
	return dst;
}
#endif

void Com_Base (FILE *stream, int code, const char *fmt, va_list args)
{
	vfprintf(stream, fmt, args);
}

#if (__GCC__ > 12) && (__STDC_VERSION__ > STDC17)
void Com_Printf (const char* fmt, ...)
{
	va_list args;
	va_start(args);
	Com_Base(stdout, 0, fmt, args);
	va_end(args);
}
#else
void Com_Printf (const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Com_Base(stdout, 0, fmt, args);
	va_end(args);
}
#endif

#if (__GCC__ > 12) && (__STDC_VERSION__ > STDC17)
void Com_Error (int const code, const char* fmt, ...)
{
	va_list args;
	va_start(args);
	Com_Base(stderr, code, fmt, args);
	va_end(args);
}
#else
void Com_Error (int const code, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Com_Base(stderr, code, fmt, args);
	va_end(args);
}
#endif

#if defined(__GCC__)
__attribute__ ((access (read_only, 2)))
#endif
int COM_InitArgv (int const argc, const char **argv)
{
	com_argc = argc;
	if (com_argc > MAX_NUM_ARGVS) {
		Com_Error(ERR_FATAL, "argc > MAX_NUM_ARGVS");
		return ERR_FATAL;
	}

	for (int i = 0; i != argc; ++i) {
		if (argv[i] == NULL || (strlen(argv[i]) + 1) > MAX_TOKEN_LEN) {
			com_argv[i] = "";
		} else {
			com_argv[i] = argv[i];
		}
	}

	return ERR_ENONE;
}

int Z_Init (void)
{
	z_chain.prev = NULL;
	z_chain.next = NULL;
	z_chain.magic = ~Z_MAGIC;
	z_chain.tag = 0xffff;
	z_chain.size = sizeof(zhead_t);
	return ERR_ENONE;
}

#if defined(__GCC__)
__attribute__ ((access (read_only, 2)))
#endif
int Qcommon_Init (int const argc, const char **argv)
{
	int rc = 0;
	rc = Z_Init();
	if (rc != ERR_ENONE) {
		return rc;
	}

	rc = COM_InitArgv(argc, argv);
	if (rc != ERR_ENONE) {
		return rc;
	}

	return rc;
}

#if defined(__GCC__)
__attribute__ ((access (write_only, 1), access (read_only, 2)))
#endif
void SZ_Init (sizebuf_t *buf, byte *data, int length)
{
	memset(buf, 0, sizeof(sizebuf_t));
	// most fields were set by memset but being explicit does not hurt
	buf->data = data;
	buf->allowoverflow = FALSE;
	buf->overflowed = FALSE;
	buf->maxsize = length;
	buf->cursize = 0;
	buf->readcount = 0;
}

#if defined(__GCC__)
__attribute__ ((access (read_write, 1)))
#endif
void SZ_Clear (sizebuf_t* buf)
{
	buf->cursize = 0;
	buf->overflowed = FALSE;
}

#if defined(__GCC__)
__attribute__ ((access (read_write, 1)))
#endif
void *SZ_GetSpace (sizebuf_t *buf, int length)
{
	if ((buf->cursize + length) >= buf->maxsize) {

		if (!buf->allowoverflow) {
			char *msg = "SZ_GetSpace: overflow without allow overflow\n";
			Com_Error(ERR_FATAL, msg);
			return NULL;
		}

		if (length >= buf->maxsize) {
			char *msg = "SZ_GetSpace: length %d exceeds buffer size %d\n";
			Com_Error(ERR_FATAL, msg, length, MAX_TOKEN_CHARS);
			return NULL;
		}

		Com_Printf("SZ_GetSpace: overflow\n");
		SZ_Clear(buf);
		buf->overflowed = TRUE;
	}

	void *data = buf->data + buf->cursize;
	buf->cursize += length;
	return data;
}

#if defined(__GCC__)
__attribute__ ((access (write_only, 1), access (read_only, 2)))
#endif
int SZ_Write (sizebuf_t *buf, const void *src, int length)
{
	void *dst = SZ_GetSpace(buf, length);
	if (!dst) {
		return ERR_FATAL;
	}

	memcpy(dst, src, length);
	return ERR_ENONE;
}

#if defined(__GCC__)
__attribute__ ((access (read_only, 1)))
#endif
int COM_AddParam (const char *parm)
{
	if (com_argc == MAX_NUM_ARGVS) {
		Com_Error(ERR_FATAL, "COM_AddParam: MAX_NUM_ARGVS\n");
		return ERR_FATAL;
	}

	com_argv[com_argc] = parm;
	++com_argc;

	return ERR_ENONE;
}

int COM_Argc (void)
{
	return com_argc;
}

const char *COM_Argv (int const i)
{
	if (i < 0 || i >= com_argc || !com_argv[i]) {
		return "";
	} else {
		return com_argv[i];
	}
}

void COM_ClearArgv (int const i)
{
	if (i < 0 || i >= com_argc || !com_argv[i]) {
		return;
	}

	com_argv[i] = "";
}

int Cbuf_Init (void)
{
	memset(cmd_text_buf, 0, sizeof(cmd_text_buf));
	SZ_Init(&cmd_text, cmd_text_buf, sizeof(cmd_text_buf));
	return ERR_ENONE;
}

int Cbuf_AddText (const char *text)
{
	int const length = strlen(text);
	return SZ_Write(&cmd_text, text, length);
}

int Cbuf_AddEarlyCommands (qboolean clear)
{
	int rc;
	int i = 0;
	char cmd[MAX_TOKEN_LEN];
	const char *fmt = "set %s %s\n";
	while (i < COM_Argc()) {

		const char *arg = COM_Argv(i);
		if (strcmp(arg, "+set")) {
			++i;
			continue;
		}

		int const len = strlen(COM_Argv(i + 1)) +
			        strlen(COM_Argv(i + 2)) +
				8;

		if (len >= MAX_TOKEN_LEN) {
			char msg[] = "Cbuf_AddEarlyCommands: cmd length %d exceeds the "
				     "MAX_TOKEN_LEN %d\n";
			Com_Error(ERR_FATAL, msg, len, MAX_TOKEN_LEN);
			return ERR_FATAL;
		}

		rc = Cbuf_AddText(va(cmd, fmt, COM_Argv(i + 1), COM_Argv(i + 2)));
		if (rc != ERR_ENONE) {
			return rc;
		}

		if (clear) {
			COM_ClearArgv(i);
			COM_ClearArgv(i + 1);
			COM_ClearArgv(i + 2);
		}
		i += 3;
	}

	return ERR_ENONE;
}

#if defined(__GCC__)
__attribute__ ((access (read_only, 1)))
#endif
void Cmd_ExecuteString (const char *line)
{
	Com_Printf("%s\n", line);
}

int Cbuf_Execute (void)
{
	int i;
	char line[MAX_TOKEN_LEN];
	while (cmd_text.cursize) {

		qboolean matched = FALSE;
		char *text = (char *) cmd_text.data;
		for (i = 0; i != cmd_text.cursize; ++i) {
			if (text[i] == '\n') {
				matched = TRUE;
				break;
			}
		}

		int const len = i;
		if (len == 0) {
			Com_Error(ERR_FATAL, "Cbuf_Execute: unexpected error\n");
			return ERR_FATAL;
		}

		if (len == cmd_text.cursize && !matched) {
			Com_Error(ERR_FATAL, "Cbuf_Execute: maybe trash in cmd buffer\n");
			return ERR_FATAL;
		}

		if (len >= MAX_TOKEN_LEN) {
			char msg[] = "Cbuf_Execute: cmd len %d exceeds the MAX_TOKEN_LEN "
				     "%d\n";
			Com_Error(ERR_FATAL, msg, len, MAX_TOKEN_LEN);
			return ERR_FATAL;
		}

		memcpy(line, text, len);
		line[len] = '\0';

		if (len == cmd_text.cursize) {
			cmd_text.cursize = 0;
		} else {
			++i;
			cmd_text.cursize -= i;
			memmove(text, text + i, cmd_text.cursize);
		}

		Cmd_ExecuteString(line);

		if (cmd_wait) {
			cmd_wait = FALSE;
			break;
		}
	}

	return ERR_ENONE;
}

void *Z_Free (void *ptr)
{
	if (!ptr) {
		return NULL;
	}

	zhead_t *z = ((zhead_t *) ptr) - 1;

	if (z->magic != Z_MAGIC) {
		Com_Error(ERR_FATAL, "Z_Free: bad magic\n");
		return ptr;
	}

	if (z->prev) {
		z->prev->next = z->next;
	}

	if (z->next) {
		z->next->prev = z->prev;
	}

	--z_count;
	z_bytes -= z->size;

	free(z);
	z = NULL;
	return z;
}

int Z_TagFree (short const tag)
{
	zhead_t *z;
	zhead_t *next;
	qboolean sw = FALSE;
	for (z = z_chain.next; z; z = next) {
		next = z->next;
		if (tag == z->tag) {
			void *ptr = ((void *) (z + 1));
			ptr = Z_Free(ptr);
			if (ptr) {
				Com_Error(ERR_FATAL, "Z_TagFree\n");
				sw = TRUE;
			}
		}
	}

	int rc = (sw)? ERR_FATAL : ERR_ENONE;
	return rc;
}

void *Z_TagMalloc (int const sizeObj, short const tag)
{
	int const size = sizeObj + sizeof(zhead_t);
	zhead_t *z = malloc(size);
	if (!z) {
		Com_Error(ERR_FATAL, "Z_TagMalloc: failed to allocate %d bytes\n", size);
		return NULL;
	}

	memset(z, 0, size);
	z->magic = Z_MAGIC;
	z->tag = tag;
	z->size = size;

	z->next = z_chain.next;
	z->prev = &z_chain;

	if (z_chain.next) {
		z_chain.next->prev = z;
	}

	z_chain.next = z;

	++z_count;
	z_bytes += size;

	return ((void *) (z + 1));
}

void *Z_Malloc (int size)
{
	short const tag = 0;
	return Z_TagMalloc(size, tag);
}

void Cmd_List_f (void)
{
	int count = 0;
	cmd_function_t *cmd = NULL;
	for (cmd = cmd_functions; cmd; cmd = cmd->next, ++count) {
		Com_Printf("%s\n", cmd->name);
	}

	Com_Printf("%d commands\n", count);
}

void Cmd_Wait_f (void)
{
	cmd_wait = TRUE;
}

#if defined(__GCC__)
__attribute__ ((access (read_only, 1)))
#endif
int Cmd_AddCommand (const char* name, xcommand_t function)
{
	cmd_function_t *cmd = NULL;
	for (cmd = cmd_functions; cmd; cmd = cmd->next) {
		if (!strcmp(name, cmd->name)) {
			Com_Printf("Cmd_AddCommand: %s already defined\n", name);
			return ERR_ENONE;
		}
	}

	void *ptr = Z_Malloc(sizeof(cmd_function_t));
	if (!ptr) {
		return ERR_FATAL;
	}

	cmd = ((cmd_function_t*) ptr);
	cmd->name = name;
	cmd->function = function;
	cmd->next = cmd_functions;
	cmd_functions = cmd;

	return ERR_ENONE;
}

int Cmd_Init (void)
{
	int rc;
	rc = Cmd_AddCommand("cmdlist", Cmd_List_f);
	if (rc != ERR_ENONE) {
		return rc;
	}

	rc = Cmd_AddCommand("wait", Cmd_Wait_f);
	if (rc != ERR_ENONE) {
		return rc;
	}

	return rc;
}

int Q_Free (void)
{
	return Z_TagFree(0);
}


#if defined(__GCC__)
__attribute__ ((access (read_only, 1)))
#endif
char *CopyString (const char *src)
{
	int const sz = strlen(src) + 1;
	void *ptr = Z_Malloc(sz);
	if (!ptr) {
		return NULL;
	}

	char *dst = ptr;
	strcpy(dst, src);
	return dst;
}
