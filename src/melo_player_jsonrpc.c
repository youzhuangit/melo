/*
 * melo_player_jsonrpc.c: Player base JSON-RPC interface
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

#include "melo_player_jsonrpc.h"

typedef enum {
  MELO_PLAYER_JSONRPC_FIELDS_NONE = 0,
  MELO_PLAYER_JSONRPC_FIELDS_STATE = 1,
  MELO_PLAYER_JSONRPC_FIELDS_NAME = 2,
  MELO_PLAYER_JSONRPC_FIELDS_POS = 4,
  MELO_PLAYER_JSONRPC_FIELDS_DURATION = 8,

  MELO_PLAYER_JSONRPC_FIELDS_FULL = 255,
} MeloPlayerJSONRPCFields;

static MeloPlayer *
melo_player_jsonrpc_get_player (JsonObject *obj, JsonNode **error)
{
  MeloPlayer *play;
  const gchar *id;

  /* Get player by id */
  id = json_object_get_string_member (obj, "id");
  play = melo_player_get_player_by_id (id);
  if (play)
    return play;

  /* No player found */
  *error = melo_jsonrpc_build_error_node (MELO_JSONRPC_ERROR_INVALID_PARAMS,
                                          "No player found!");
  return NULL;
}

static MeloPlayerJSONRPCFields
melo_player_jsonrpc_get_fields (JsonObject *obj)
{
  MeloPlayerJSONRPCFields fields = MELO_PLAYER_JSONRPC_FIELDS_NONE;
  const gchar *field;
  JsonArray *array;
  guint count, i;

  /* Check if fields is available */
  if (!json_object_has_member (obj, "fields"))
    return MELO_PLAYER_JSONRPC_FIELDS_NONE;

  /* Get fields array */
  array = json_object_get_array_member (obj, "fields");
  if (!array)
    return MELO_PLAYER_JSONRPC_FIELDS_NONE;

  /* Parse array */
  count = json_array_get_length (array);
  for (i = 0; i < count; i++) {
    field = json_array_get_string_element (array, i);
    if (!field)
      break;
    if (!g_strcmp0 (field, "none")) {
      fields = MELO_PLAYER_JSONRPC_FIELDS_NONE;
      break;
    } else if (!g_strcmp0 (field, "full")) {
      fields = MELO_PLAYER_JSONRPC_FIELDS_FULL;
      break;
    } else if (!g_strcmp0 (field, "state"))
      fields |= MELO_PLAYER_JSONRPC_FIELDS_STATE;
    else if (!g_strcmp0 (field, "name"))
      fields |= MELO_PLAYER_JSONRPC_FIELDS_NAME;
    else if (!g_strcmp0 (field, "pos"))
      fields |= MELO_PLAYER_JSONRPC_FIELDS_POS;
    else if (!g_strcmp0 (field, "duration"))
      fields |= MELO_PLAYER_JSONRPC_FIELDS_DURATION;
  }

  return fields;
}

static JsonObject *
melo_player_jsonrpc_status_to_object (const MeloPlayerStatus *status,
                                      MeloPlayerJSONRPCFields fields)
{
  static gchar *state_str[] = {
    [MELO_PLAYER_STATE_NONE] = "none",
    [MELO_PLAYER_STATE_PLAYING] = "playing",
    [MELO_PLAYER_STATE_PAUSED] = "paused",
    [MELO_PLAYER_STATE_STOPPED] = "stopped",
    [MELO_PLAYER_STATE_ERROR] = "error",
  };
  JsonObject *obj = json_object_new ();
  if (fields & MELO_PLAYER_JSONRPC_FIELDS_STATE) {
    json_object_set_string_member (obj, "state", state_str[status->state]);
    if (status->state == MELO_PLAYER_STATE_ERROR)
      json_object_set_string_member (obj, "error", status->error);
  }
  if (fields & MELO_PLAYER_JSONRPC_FIELDS_NAME)
    json_object_set_string_member (obj, "name", status->name);
  if (fields & MELO_PLAYER_JSONRPC_FIELDS_POS)
    json_object_set_int_member (obj, "pos", status->pos);
  if (fields & MELO_PLAYER_JSONRPC_FIELDS_DURATION)
    json_object_set_int_member (obj, "duration", status->duration);
  return obj;
}

/* Method callbacks */
static void
melo_player_jsonrpc_get_status (const gchar *method,
                                JsonArray *s_params, JsonNode *params,
                                JsonNode **result, JsonNode **error,
                                gpointer user_data)
{
  MeloPlayerJSONRPCFields fields = MELO_PLAYER_JSONRPC_FIELDS_NONE;
  MeloPlayerStatus *status = NULL;
  MeloPlayer *play;
  JsonObject *obj;

  /* Get parameters */
  obj = melo_jsonrpc_get_object (s_params, params, error);
  if (!obj)
    return;

  /* Get player from id */
  play = melo_player_jsonrpc_get_player (obj, error);
  if (!play) {
    json_object_unref (obj);
    return;
  }

  /* Get fields */
  fields = melo_player_jsonrpc_get_fields (obj);
  json_object_unref (obj);

  /* Get status */
  status = melo_player_get_status (play);
  g_object_unref (play);
  if (!status)
    return;

  /* Generate status */
  obj = melo_player_jsonrpc_status_to_object (status, fields);
  melo_player_status_free (status);

  /* Return result */
  *result = json_node_new (JSON_NODE_OBJECT);
  json_node_take_object (*result, obj);
}

/* List of methods */
static MeloJSONRPCMethod melo_player_jsonrpc_methods[] = {
  {
    .method = "get_status",
    .params = "["
              "  {\"name\": \"id\", \"type\": \"string\"},"
              "  {"
              "    \"name\": \"fields\", \"type\": \"array\","
              "    \"required\": false"
              "  }"
              "]",
    .result = "{\"type\":\"object\"}",
    .callback = melo_player_jsonrpc_get_status,
    .user_data = NULL,
  },
};

/* Register / Unregister methods */
void
melo_player_register_methods (void)
{
  melo_jsonrpc_register_methods ("player", melo_player_jsonrpc_methods,
                                 G_N_ELEMENTS (melo_player_jsonrpc_methods));
}

void
melo_player_unregister_methods (void)
{
  melo_jsonrpc_unregister_methods ("player", melo_player_jsonrpc_methods,
                                   G_N_ELEMENTS (melo_player_jsonrpc_methods));
}
