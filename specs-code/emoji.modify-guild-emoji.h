/* This file is generated from specs/emoji.modify-guild-emoji.json, Please don't edit it. */
/*

*/

/* https://discord.com/developers/docs/resources/emoji#create-guild-emoji */
/* This is defined at specs/emoji.modify-guild-emoji.json:8:22 */
struct discord_emoji_modify_guild_emoji_params {
  /* specs/emoji.modify-guild-emoji.json:11:20
     '{ "name": "name", "type":{ "base":"char", "dec":"*"}}'
  */
  char *name;

  /* specs/emoji.modify-guild-emoji.json:12:20
     '{ "name": "roles", "type":{ "base":"ja_u64", "dec":"ntl" }, "comment":"roles for which this emoji will be whitelisted"}'
  */
  ja_u64 **roles; // roles for which this emoji will be whitelisted

  // The following is metadata used to 
  // 1. control which field should be extracted/injected
  // 2. record which field is presented(defined) in JSON
  // 3. record which field is null in JSON
  struct {
    bool enable_arg_switches;
    bool enable_record_defined;
    bool enable_record_null;
    void *arg_switches[2];
    void *record_defined[2];
    void *record_null[2];
  } __M; // metadata
};
extern void discord_emoji_modify_guild_emoji_params_cleanup_v(void *p);
extern void discord_emoji_modify_guild_emoji_params_cleanup(struct discord_emoji_modify_guild_emoji_params *p);
extern void discord_emoji_modify_guild_emoji_params_init_v(void *p);
extern void discord_emoji_modify_guild_emoji_params_init(struct discord_emoji_modify_guild_emoji_params *p);
extern struct discord_emoji_modify_guild_emoji_params * discord_emoji_modify_guild_emoji_params_alloc();
extern void discord_emoji_modify_guild_emoji_params_free_v(void *p);
extern void discord_emoji_modify_guild_emoji_params_free(struct discord_emoji_modify_guild_emoji_params *p);
extern void discord_emoji_modify_guild_emoji_params_from_json_v(char *json, size_t len, void *p);
extern void discord_emoji_modify_guild_emoji_params_from_json(char *json, size_t len, struct discord_emoji_modify_guild_emoji_params *p);
extern size_t discord_emoji_modify_guild_emoji_params_to_json_v(char *json, size_t len, void *p);
extern size_t discord_emoji_modify_guild_emoji_params_to_json(char *json, size_t len, struct discord_emoji_modify_guild_emoji_params *p);
extern size_t discord_emoji_modify_guild_emoji_params_to_query_v(char *json, size_t len, void *p);
extern size_t discord_emoji_modify_guild_emoji_params_to_query(char *json, size_t len, struct discord_emoji_modify_guild_emoji_params *p);
extern void discord_emoji_modify_guild_emoji_params_list_free_v(void **p);
extern void discord_emoji_modify_guild_emoji_params_list_free(struct discord_emoji_modify_guild_emoji_params **p);
extern void discord_emoji_modify_guild_emoji_params_list_from_json_v(char *str, size_t len, void *p);
extern void discord_emoji_modify_guild_emoji_params_list_from_json(char *str, size_t len, struct discord_emoji_modify_guild_emoji_params ***p);
extern size_t discord_emoji_modify_guild_emoji_params_list_to_json_v(char *str, size_t len, void *p);
extern size_t discord_emoji_modify_guild_emoji_params_list_to_json(char *str, size_t len, struct discord_emoji_modify_guild_emoji_params **p);
