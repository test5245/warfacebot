/**
 * WarfaceBot, a blind XMPP client for Warface (FPS)
 * Copyright (C) 2015 Levak Borok <levak92@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <wb_xmpp.h>
#include <wb_room.h>
#include <wb_session.h>
#include <wb_list.h>

#include <stdlib.h>
#include <string.h>

static int room_cmp(const struct room *r, const char *jid)
{
    return strcmp(r->jid, jid);
}

inline static void room_free_fields_(struct room *r)
{
    free(r->jid);
}

inline static void room_set_fields_(struct room *r,
                                    const char *jid)
{
    r->jid = jid && *jid ? strdup(jid) : NULL;
    r->type = ROOM_OTHER;

    if (strstr(r->jid, "pve"))
        r->type = ROOM_PVE;
    else if (strstr(r->jid, "pvp"))
        r->type = ROOM_PVP;
    else if (strstr(r->jid, "clan"))
        r->type = ROOM_CLAN;
    else if (strstr(r->jid, "global"))
        r->type = ROOM_GLOBAL;
}

static void room_free(struct room *r)
{
    room_free_fields_(r);
    free(r);
}

void room_list_add(const char *jid)
{
    if (list_get(session.rooms, jid) != NULL)
        return;

    struct room *r = calloc(1, sizeof (struct room));

    room_set_fields_(r, jid);

    list_add(session.rooms, r);
}

void room_list_remove(const char *jid)
{
    list_remove(session.rooms, jid);
}

static void leave_room_cb_(void *r, void *args)
{
    xmpp_presence(((struct room *) r)->jid, 1);
}

void room_list_empty(void)
{
    list_foreach(session.rooms, &leave_room_cb_, NULL);
    list_empty(session.rooms);
}

void room_list_init(void)
{
    session.rooms = list_new((f_list_cmp) room_cmp,
                             (f_list_free) room_free);
}

void room_list_free(void)
{
    list_free(session.rooms);
    session.rooms = NULL;
}
