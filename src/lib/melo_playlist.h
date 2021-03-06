/*
 * melo_playlist.h: Playlist base class
 *
 * Copyright (C) 2016 Alexandre Dilly <dillya@sparod.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#ifndef __MELO_PLAYLIST_H__
#define __MELO_PLAYLIST_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define MELO_TYPE_PLAYLIST             (melo_playlist_get_type ())
#define MELO_PLAYLIST(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MELO_TYPE_PLAYLIST, MeloPlaylist))
#define MELO_IS_PLAYLIST(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MELO_TYPE_PLAYLIST))
#define MELO_PLAYLIST_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MELO_TYPE_PLAYLIST, MeloPlaylistClass))
#define MELO_IS_PLAYLIST_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MELO_TYPE_PLAYLIST))
#define MELO_PLAYLIST_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MELO_TYPE_PLAYLIST, MeloPlaylistClass))

typedef struct _MeloPlayer MeloPlayer;
typedef struct _MeloPlaylist MeloPlaylist;
typedef struct _MeloPlaylistClass MeloPlaylistClass;
typedef struct _MeloPlaylistPrivate MeloPlaylistPrivate;

typedef struct _MeloPlaylistItem MeloPlaylistItem;

struct _MeloPlaylist {
  GObject parent_instance;

  /*< protected */
  MeloPlayer *player;

  /*< private >*/
  MeloPlaylistPrivate *priv;
};

struct _MeloPlaylistClass {
  GObjectClass parent_class;

  GList *(*get_list) (MeloPlaylist *playlist, gchar **current);
  gboolean (*add) (MeloPlaylist *playlist, const gchar *name,
                   const gchar *full_name, const gchar *path,
                   gboolean is_current);
  gchar *(*get_prev) (MeloPlaylist *playlist, gboolean set);
  gchar *(*get_next) (MeloPlaylist *playlist, gboolean set);
  gboolean (*play) (MeloPlaylist *playlist, const gchar *name);
  gboolean (*remove) (MeloPlaylist *playlist, const gchar *name);
  void (*empty) (MeloPlaylist *playlist);
};

struct _MeloPlaylistItem {
  gchar *name;
  gchar *full_name;
  gchar *path;
  gboolean can_play;
  gboolean can_remove;

  gint ref_count;
};

GType melo_playlist_get_type (void);

MeloPlaylist *melo_playlist_new (GType type, const gchar *id);
const gchar *melo_playlist_get_id (MeloPlaylist *playlist);
MeloPlaylist *melo_playlist_get_playlist_by_id (const gchar *id);

void melo_playlist_set_player (MeloPlaylist *playlist, MeloPlayer *player);
MeloPlayer *melo_playlist_get_player (MeloPlaylist *playlist);

GList *melo_playlist_get_list (MeloPlaylist *playlist, gchar **current);
gboolean melo_playlist_add (MeloPlaylist *playlist, const gchar *name,
                            const gchar *full_name, const gchar *path,
                            gboolean is_current);
gchar *melo_playlist_get_prev (MeloPlaylist *playlist, gboolean set);
gchar *melo_playlist_get_next (MeloPlaylist *playlist, gboolean set);
gboolean melo_playlist_play (MeloPlaylist *playlist, const gchar *name);
gboolean melo_playlist_remove (MeloPlaylist *playlist, const gchar *name);
void melo_playlist_empty (MeloPlaylist *playlist);

MeloPlaylistItem *melo_playlist_item_new (const gchar *name,
                                          const gchar *full_name,
                                          const gchar *path);
MeloPlaylistItem *melo_playlist_item_ref (MeloPlaylistItem *item);
void melo_playlist_item_unref (MeloPlaylistItem *item);

G_END_DECLS

#endif /* __MELO_PLAYLIST_H__ */
