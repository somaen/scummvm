/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#include "dcgf.h"
#include "BParser.h"
#include "BGame.h"
#include "PlatformSDL.h"
#include "common/str.h"
#include "common/util.h"

#define WHITESPACE " \t\n\r"

namespace WinterMute {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
CBParser::CBParser(CBGame *inGame): CBBase(inGame) {
	_whiteSpace = new char [strlen(WHITESPACE) + 1];
	strcpy(_whiteSpace, WHITESPACE);
}


//////////////////////////////////////////////////////////////////////
CBParser::~CBParser() {
	if (_whiteSpace != NULL) delete [] _whiteSpace;
}


//////////////////////////////////////////////////////////////////////
char *CBParser::GetLastOffender() {
	return _lastOffender;
}


//////////////////////////////////////////////////////////////////////
long CBParser::GetObject(char **buf, TokenDesc *tokens, char **name, char **data) {
	SkipCharacters(buf, _whiteSpace);

	// skip comment lines.
	while (**buf == ';') {
		*buf = strchr(*buf, '\n');
		_parserLine++;
		SkipCharacters(buf, _whiteSpace);
	}

	if (! **buf)                  // at end of file
		return PARSERR_EOF;

	// find the token.
	// for now just use brute force.  Improve later.
	while (tokens->id != 0) {
		if (!scumm_strnicmp(tokens->token, *buf, strlen(tokens->token))) {
			// here we could be matching PART of a string
			// we could detect this here or the token list
			// could just have the longer tokens first in the list
			break;
		}
		++tokens;
	}
	if (tokens->id == 0) {
		char *p = strchr(*buf, '\n');
		if (p && p > *buf) {
			strncpy(_lastOffender, *buf, MIN((long int)255, p - *buf)); // TODO, clean
		} else strcpy(_lastOffender, "");

		return PARSERR_TOKENNOTFOUND;
	}
	// skip the token
	*buf += strlen(tokens->token);
	SkipCharacters(buf, _whiteSpace);

	// get optional name
	*name = GetSubText(buf, '\'', '\'');  // single quotes
	SkipCharacters(buf, _whiteSpace);

	// get optional data
	if (**buf == '=') // An assignment rather than a command/object.
		*data = GetAssignmentText(buf);
	else
		*data = GetSubText(buf, '{', '}');

	return tokens->id;
}


//////////////////////////////////////////////////////////////////////
long CBParser::GetCommand(char **buf, TokenDesc *tokens, char **params) {
	if (!*buf) return PARSERR_TOKENNOTFOUND;
	Game->MiniUpdate();
	char *name;
	return GetObject(buf, tokens, &name, params);
}


//////////////////////////////////////////////////////////////////////
void CBParser::SkipCharacters(char **buf, const char *toSkip) {
	char ch;
	while ((ch = **buf) != 0) {
		if (ch == '\n') _parserLine++;
		if (strchr(toSkip, ch) == NULL)
			return;
		++*buf;                     // skip this character
	}
	// we must be at the end of the buffer if we get here
}


//////////////////////////////////////////////////////////////////////
char *CBParser::GetSubText(char **buf, char open, char close) {
	if (**buf == 0 || **buf != open)
		return 0;
	++*buf;                       // skip opening delimiter
	char *result = *buf;

	// now find the closing delimiter
	char theChar;
	long skip = 1;

	if (open == close)            // we cant nest identical delimiters
		open = 0;
	while ((theChar = **buf) != 0) {
		if (theChar == open)
			++skip;
		if (theChar == close) {
			if (--skip == 0) {
				**buf = 0;              // null terminate the result string
				++*buf;                 // move past the closing delimiter
				break;
			}
		}
		++*buf;                     // try next character
	}
	return result;
}


//////////////////////////////////////////////////////////////////////
char *CBParser::GetAssignmentText(char **buf) {
	++*buf;                       // skip the '='
	SkipCharacters(buf, _whiteSpace);
	char *result = *buf;


	if (*result == '"') {
		result = GetSubText(buf, '"', '"');
	} else {
		// now, we need to find the next whitespace to end the data
		char theChar;

		while ((theChar = **buf) != 0) {
			if (theChar <= 0x20)        // space and control chars
				break;
			++*buf;
		}
		**buf = 0;                  // null terminate it
		if (theChar)                  // skip the terminator
			++*buf;
	}

	return result;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
char *CBParser::GetToken(char **buf) {
	static char token[100];
	char *b = *buf, * t = token;
	while (true) {
		while (*b && (*b == ' ' || *b == '\n' || *b == 13 || *b == 10 || *b == '\t')) b++;
		if (*b == ';')
			while (*b && *b != '\n' && *b != 13 && *b != 10) b++;
		else break;
	}

	if (*b == '\'') {
		b++;
		while (*b && *b != '\'') {
			*t++ = *b++;
		}
		*t++ = 0;
		if (*b == '\'') b++;
	} else if (*b == '(' || *b == ')' || *b == '=' || *b == ',' || *b == '[' || *b == ']' ||
	           *b == '%' || *b == ':' || *b == '{' || *b == '}') {
		*t++ = *b++;
		*t++ = 0;
	} else if (*b == '.' && (*(b + 1) < '0' || *(b + 1) > '9')) {
		*t++ = *b++;
		*t++ = 0;
	} else if ((*b >= '0' && *b <= '9') || *b == '.' || *b == '-') {
		while (*b && ((*b >= '0' && *b <= '9') || *b == '.' || *b == '-')) {
			*t++ = *b++;
		}
		*t++ = 0;
	} else if ((*b >= 'A' && *b <= 'Z') || (*b >= 'a' && *b <= 'z') || *b == '_') {
		while (*b && ((*b >= 'A' && *b <= 'Z') || (*b >= 'a' && *b <= 'z') || *b == '_')) {
			*t++ = *b++;
		}
		*t++ = 0;
	} else if (*b == 0) {
		*buf = b;
		return NULL;
	} else {
		// Error.
		return NULL;
	}

	*buf = b;
	return token;
}


//////////////////////////////////////////////////////////////////////
float CBParser::GetTokenFloat(char **buf) {
	char *t = GetToken(buf);
	if (!((*t >= '0' && *t <= '9') || *t == '-' || *t == '.')) {
		// Error situation. We handle this by return 0.
		return 0.;
	}
	float rc = (float)atof(t);
	return rc;
}


//////////////////////////////////////////////////////////////////////
int CBParser::GetTokenInt(char **buf) {
	char *t = GetToken(buf);
	if (!((*t >= '0' && *t <= '9') || *t == '-')) {
		// Error situation. We handle this by return 0.
		return 0;
	}
	int rc = atoi(t);
	return rc;
}


//////////////////////////////////////////////////////////////////////
void CBParser::SkipToken(char **buf, char *tok, char * /*msg*/) {
	char *t = GetToken(buf);
	if (strcmp(t, tok)) return;  // Error
}


//////////////////////////////////////////////////////////////////////
int CBParser::ScanStr(const char *in, const char *format, ...) {
	va_list arg;
	va_start(arg, format);

	int num = 0;
	in += strspn(in, " \t\n\f");

	while (*format && *in) {
		if (*format == '%') {
			format++;
			switch (*format) {
			case 'd': {
				int *a = va_arg(arg, int *);
				in += strspn(in, " \t\n\f");
				*a = atoi(in);
				in += strspn(in, "0123456789+- \t\n\f");
				num++;
				break;
			}
			case 'D': {
				int i;
				int *list = va_arg(arg, int *);
				int *nr = va_arg(arg, int *);
				in += strspn(in, " \t\n\f");
				i = 0;
				while ((*in >= '0' && *in <= '9') || *in == '+' || *in == '-') {
					list[i++] = atoi(in);
					in += strspn(in, "0123456789+-");
					in += strspn(in, " \t\n\f");
					if (*in != ',') break;
					in++;
					in += strspn(in, " \t\n\f");
				}
				*nr = i;
				num++;
				break;
			}
			case 'b': {
				bool *a = va_arg(arg, bool *);
				in += strspn(in, " \t\n\f");
				const char *in2 = in + strspn(in, "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
				int l = (int)(in2 - in);

				*a = (bool)(!scumm_strnicmp(in, "yes", l) || !scumm_strnicmp(in, "true", l) || !scumm_strnicmp(in, "on", l) ||
				            !scumm_strnicmp(in, "1", l));


				in = in2 + strspn(in2, " \t\n\f");
				num++;
				break;
			}
			case 'f': {
				float *a = va_arg(arg, float *);
				in += strspn(in, " \t\n\f");
				*a = (float)atof(in);
				in += strspn(in, "0123456789.eE+- \t\n\f");
				num++;
				break;
			}
			case 'F': {
				int i;
				float *list = va_arg(arg, float *);
				int *nr = va_arg(arg, int *);
				in += strspn(in, " \t\n\f");
				i = 0;
				while ((*in >= '0' && *in <= '9') || *in == '.' || *in == '+' || *in == '-' || *in == 'e' || *in == 'E') {
					list[i++] = (float)atof(in);
					in += strspn(in, "0123456789.eE+-");
					in += strspn(in, " \t\n\f");
					if (*in != ',') break;
					in++;
					in += strspn(in, " \t\n\f");
				}
				*nr = i;
				num++;
				break;
			}
			case 's': {
				char *a = va_arg(arg, char *);
				in += strspn(in, " \t\n\f");
				if (*in == '\'') {
					in++;
					const char *in2 = strchr(in, '\'');
					if (in2) {
						strncpy(a, in, (int)(in2 - in));
						a[(int)(in2 - in)] = 0;
						in = in2 + 1;
					} else {
						strcpy(a, in);
						in = strchr(in, 0);
					}
				} else {
					const char *in2 = in + strspn(in, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789.");
					strncpy(a, in, (int)(in2 - in));
					a[(int)(in2 - in)] = 0;
					in = in2;
				}
				in += strspn(in, " \t\n\f");
				num++;
				break;
			}
			case 'S': {
				char *a = va_arg(arg, char *);
				in += strspn(in, " \t\n\f");
				if (*in == '\"') {
					in++;
					while (*in != '\"') {
						if (*in == '\\') {
							in++;
							switch (*in) {
							case '\\':
								*a++ = '\\';
								break;
							case 'n':
								*a++ = '\n';
								break;
							case 'r':
								*a++ = '\r';
								break;
							case 't':
								*a++ = '\t';
								break;
							case '"':
								*a++ = '"';
								break;
							default:
								*a++ = '\\';
								*a++ = *in;
								break;
							} //switch
							in++;
						} else {
							*a++ = *in++;
						}
					} //while in string
					in++;
					num++;
				} //if string started

				//terminate string
				*a = '\0';
				break;
			}
			}
			if (*format) format++;
		} else if (*format == ' ') {
			format++;
			in += strspn(in, " \t\n\f");
		} else if (*in == *format) {
			in++;
			format++;
		} else {
			num = -1;
			break;
		}
	}

	va_end(arg);

	return num;
}

} // end of namespace WinterMute
