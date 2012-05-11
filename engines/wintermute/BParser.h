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

#ifndef WINTERMUTE_BPARSER_H
#define WINTERMUTE_BPARSER_H


#define TOKEN_DEF_START         \
	enum                  \
	{                 \
		TOKEN_NONE = 0,
#define TOKEN_DEF(name)         \
	TOKEN_ ## name,
#define TOKEN_DEF_END           \
	TOKEN_TOTAL_COUNT           \
	};
#define TOKEN_TABLE_START(name)     \
	static CBParser::TokenDesc name [] =      \
	        {
#define TOKEN_TABLE(name)       \
	{ TOKEN_ ## name, #name },
#define TOKEN_TABLE_END         \
	{ 0, 0 }                \
	};

#define PARSERR_GENERIC         -3
#define PARSERR_EOF             -2
#define PARSERR_TOKENNOTFOUND   -1

#include "BBase.h"
#include "coll_templ.h"

namespace WinterMute {

class CBParser : public CBBase {
public:
	struct TokenDesc {
		long    id;
		const char  *token;
	};

public:
	int ScanStr(const char *in, const char *format, ...);
	char *GetLastOffender();
	void SkipToken(char **buf, char *tok, char *msg = NULL);
	int GetTokenInt(char **buf);
	float GetTokenFloat(char **buf);
	char *GetToken(char **buf);
	char *GetAssignmentText(char **buf);
	char *GetSubText(char **buf, char open, char close);
	void SkipCharacters(char **buf, const char *toSkip);
	long GetCommand(char **buf, TokenDesc *tokens, char **params);
	long GetObject(char **buf, TokenDesc *tokens, char **name, char **data);
	int _parserLine;
	char _lastOffender[255];
	CBParser(CBGame *inGame = NULL);
	virtual ~CBParser();
	char *_whiteSpace;
};

} // end of namespace WinterMute

#endif
