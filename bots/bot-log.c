#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libdiscord.h"


void on_ready(struct discord_client *client, const struct discord_user_dati *me) {
  fprintf(stderr, "\n\nLog-Bot succesfully connected to Discord as %s#%s!\n\n",
      me->username, me->discriminator);
}

void on_guild_member_add(
  struct discord_client *client,
  const struct discord_user_dati *me,
  const uint64_t guild_id, 
  const struct discord_guild_member_dati *member)
{
  printf("%s#%s joined guild %" PRIu64".\n", member->user->username, member->user->discriminator, guild_id);
}

void on_guild_member_update(
  struct discord_client *client,
  const struct discord_user_dati *me,
  const uint64_t guild_id, 
  const struct discord_guild_member_dati *member)
{
  printf("%s#%s ", member->user->username, member->user->discriminator);
  if(!IS_EMPTY_STRING(member->nick)) {
    printf("(%s) ", member->nick);
  }
  printf("updated (guild %" PRIu64")\n", guild_id);
}

void on_guild_member_remove(
  struct discord_client *client,
  const struct discord_user_dati *me,
  const uint64_t guild_id, 
  const struct discord_user_dati *user)
{
  printf("%s#%s left guild %" PRIu64".\n", user->username, user->discriminator, guild_id);
}

int main(int argc, char *argv[])
{
  const char *config_file;
  if (argc > 1)
    config_file = argv[1];
  else
    config_file = "bot.config";

  discord_global_init();

  struct discord_client *client = discord_config_init(config_file);
  assert(NULL != client);

  discord_setcb(client, READY, &on_ready);
  discord_setcb(client, GUILD_MEMBER_ADD, &on_guild_member_add);
  discord_setcb(client, GUILD_MEMBER_UPDATE, &on_guild_member_update);
  discord_setcb(client, GUILD_MEMBER_REMOVE, &on_guild_member_remove);

  discord_run(client);

  discord_cleanup(client);

  discord_global_cleanup();
}