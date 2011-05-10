//system includes
#include <glib/gi18n.h>
#include <accountopt.h>
#include <xmlnode.h>

//purple includes
#include <notify.h>
#include <prpl.h>
#include <version.h>

//local includes
#include "message.h"

void campfire_room_query(CampfireConn *campfire);

gboolean plugin_load(PurplePlugin *plugin)
{
	return TRUE;
}

gboolean plugin_unload(PurplePlugin *plugin)
{
	return TRUE;
}

static void campfire_login(PurpleAccount *acct)
{
	//campfire is stateless, so we might just leave this empty
}

static void campfire_close(PurpleConnection *gc)
{
}

static void campfire_buddy_free(PurpleBuddy * buddy)
{
}

static gchar * campfire_status_text(PurpleBuddy *buddy)
{
	return NULL;
}

static void campfire_set_status(PurpleAccount *acct, PurpleStatus *status)
{
}

static GHashTable * campfire_get_account_text_table(PurpleAccount *account)
{
	GHashTable *table;
	table = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(table, "login_label", (gpointer)_("API token"));
	return table;
}

static GList * campfire_statuses(PurpleAccount *acct)
{
	GList *types = NULL;
	PurpleStatusType *status;
	
	//Online people have a status message and also a date when it was set	
	status = purple_status_type_new_full(PURPLE_STATUS_AVAILABLE, NULL, _("Online"), TRUE, TRUE, FALSE);
	types = g_list_append(types, status);
	
	//Offline people dont have messages
	status = purple_status_type_new_full(PURPLE_STATUS_OFFLINE, NULL, _("Offline"), TRUE, TRUE, FALSE);
	types = g_list_append(types, status);
	
	return types;

}

GList *campfire_chat_info(PurpleConnection *gc)
{
	GList *m = NULL;
	struct proto_chat_entry *pce;

	pce = g_new0(struct proto_chat_entry, 1);
	pce->label = _("_Room:");
	pce->identifier = "room";
	pce->required = TRUE;
	m = g_list_append(m, pce);

	return m;
}

PurpleRoomlist *campfire_roomlist_get_list(PurpleConnection *gc)
{	
	CampfireConn *campfire = gc->proto_data;
	GList *fields = NULL;
	PurpleRoomlistField *f;

	if (campfire->roomlist)
		purple_roomlist_unref(campfire->roomlist);
	
	campfire->roomlist = purple_roomlist_new(purple_connection_get_account(gc));

	f = purple_roomlist_field_new(PURPLE_ROOMLIST_FIELD_STRING, "", "room", TRUE);
	fields = g_list_append(fields, f);

	f = purple_roomlist_field_new(PURPLE_ROOMLIST_FIELD_STRING, _("Topic"), "topic", FALSE);
	fields = g_list_append(fields, f);

	purple_roomlist_set_fields(campfire->roomlist, fields);

	campfire_room_query(campfire);
	
	return campfire->roomlist;
}

void campfire_roomlist_cancel(PurpleRoomlist *list)
{
	PurpleConnection *gc = purple_account_get_connection(list->account);

	if (gc == NULL)
		return;

	CampfireConn *campfire = gc->proto_data;

	purple_roomlist_set_in_progress(list, FALSE);

	if (campfire->roomlist == list) {
		campfire->roomlist = NULL;
		purple_roomlist_unref(list);
	}
}

void campfire_room_query(CampfireConn *campfire)
{
	purple_roomlist_set_in_progress(campfire->roomlist, TRUE);
	//@TODO do some curl/xml stuff here
	//see
	//protocols/jabber/chat.c:864 roomlist_ok_cb() AND
	//protocols/jabber/chat.c:800 roomlist_disco_result_cb()
	purple_roomlist_set_in_progress(campfire->roomlist, FALSE);
	purple_roomlist_unref(campfire->roomlist);
	campfire->roomlist = NULL;
}

const char *campfireim_list_icon(PurpleAccount *account, PurpleBuddy *buddy)
{
	return "campfire";
}

static PurplePluginProtocolInfo campfire_protocol_info = {
	/* options */
	OPT_PROTO_NO_PASSWORD | OPT_PROTO_SLASH_COMMANDS_NATIVE,
	NULL,                   /* user_splits */
	NULL,                   /* protocol_options */
	//NO_BUDDY_ICONS          /* icon_spec */
	{   /* icon_spec, a PurpleBuddyIconSpec */
		"png,jpg,gif",                   /* format */
		0,                               /* min_width */
		0,                               /* min_height */
		50,                             /* max_width */
		50,                             /* max_height */
		10000,                           /* max_filesize */
		PURPLE_ICON_SCALE_DISPLAY,       /* scale_rules */
	},
	campfireim_list_icon,   /* list_icon */
	NULL,                   /* list_emblems */
	campfire_status_text,   /* status_text */
	NULL,
	campfire_statuses,      /* status_types */
	NULL,                   /* blist_node_menu */
	campfire_chat_info,     /* chat_info */
	NULL,                   /* chat_info_defaults */
	campfire_login,       	/* login */ //can we make this null?
	campfire_close,       	/* close */
	NULL,     		        /* send_im */
	NULL,                   /* set_info */
	NULL,
	NULL,
	campfire_set_status,    /* set_status */
	NULL,                   /* set_idle */
	NULL,                   /* change_passwd */
	NULL,
	NULL,                   /* add_buddies */
	NULL,
	NULL,                   /* remove_buddies */
	NULL,                   /* add_permit */
	NULL,                   /* add_deny */
	NULL,                   /* rem_permit */
	NULL,                   /* rem_deny */
	NULL,                   /* set_permit_deny */
	NULL,                   /* join_chat */
	NULL,                   /* reject chat invite */
	NULL,                   /* get_chat_name */
	NULL,                   /* chat_invite */
	NULL,                   /* chat_leave */
	NULL,                   /* chat_whisper */
	NULL,                   /* chat_send */
	NULL,                   /* keepalive */
	NULL,                   /* register_user */
	NULL,                   /* get_cb_info */
	NULL,                   /* get_cb_away */
	NULL,                   /* alias_buddy */
	NULL,                   /* group_buddy */
	NULL,                   /* rename_group */
	campfire_buddy_free,	/* buddy_free */
	NULL,                   /* convo_closed */
	purple_normalize_nocase,/* normalize */
	NULL,                   /* set_buddy_icon */
	NULL,                   /* remove_group */
	NULL,                   /* get_cb_real_name */
	NULL,                   /* set_chat_topic */
	NULL,                   /* find_blist_chat */
	campfire_roomlist_get_list,/* roomlist_get_list */
	campfire_roomlist_cancel,/* roomlist_cancel */
	NULL,                   /* roomlist_expand_category */
	NULL,                   /* can_receive_file */
	NULL,                   /* send_file */
	NULL,                   /* new_xfer */
	NULL,                   /* offline_message */
	NULL,                   /* whiteboard_prpl_ops */
	NULL,                   /* send_raw */
	NULL,                   /* roomlist_room_serialize */
	NULL,                   /* unregister_user */
	NULL,                   /* send_attention */
	NULL,                   /* attention_types */
	sizeof(PurplePluginProtocolInfo), /* struct_size */
	campfire_get_account_text_table /* get_account_text_table */	
};

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_PROTOCOL, /* type */
	NULL, /* ui_requirement */
	0, /* flags */
	NULL, /* dependencies */
	PURPLE_PRIORITY_DEFAULT, /* priority */
	"prpl-campfire", /* id */
	"Campfire", /* name */
	"0.1", /* version */
	"Campfire Chat", /* summary */
	"Campfire Chat Protocol Plugin", /* description */
	"Jake Foell <jfoell@gmail.com>", /* author */
	"https://github.com/jrfoell/campfire-libpurple", /* homepage */
	plugin_load, /* load */
	plugin_unload, /* unload */
	NULL, /* destroy */
	NULL, /* ui_info */
	&campfire_protocol_info, /* extra_info */
	NULL, /* prefs_info */
	NULL, /* actions */
	NULL, /* padding */
	NULL,
	NULL,
	NULL
};

static void plugin_init(PurplePlugin *plugin)
{
	PurpleAccountUserSplit *split;
	
	split = purple_account_user_split_new(_("Hostname"), NULL, '@');
	campfire_protocol_info.user_splits = g_list_append(campfire_protocol_info.user_splits, split);
}

PURPLE_INIT_PLUGIN(campfire, plugin_init, info);

