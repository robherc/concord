#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "discord.h"
#include "discord-internal.h"
#include "discord-request.h"

CCORDcode
discord_get_guild_template(struct discord *client,
                           char *code,
                           struct discord_ret_guild_template *ret)
{
    struct discord_request req = { 0 };

    CCORD_EXPECT(client, NOT_EMPTY_STR(code), CCORD_BAD_PARAMETER, "");

    DISCORD_REQ_INIT(req, discord_guild_template, ret);

    return discord_adapter_run(&client->adapter, &req, NULL, HTTP_GET,
                               "/guilds/templates/%s", code);
}

CCORDcode
discord_create_guild_template(struct discord *client,
                              u64snowflake guild_id,
                              struct discord_create_guild_template *params,
                              struct discord_ret_guild_template *ret)
{
    struct discord_request req = { 0 };
    struct sized_buffer body;
    char buf[256];

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    body.size =
        discord_create_guild_template_to_json(buf, sizeof(buf), params);
    body.start = buf;

    DISCORD_REQ_INIT(req, discord_guild_template, ret);

    return discord_adapter_run(&client->adapter, &req, &body, HTTP_POST,
                               "/guilds/%" PRIu64 "/templates", guild_id);
}

CCORDcode
discord_sync_guild_template(struct discord *client,
                            u64snowflake guild_id,
                            char *code,
                            struct discord_ret_guild_template *ret)
{
    struct discord_request req = { 0 };

    CCORD_EXPECT(client, guild_id != 0, CCORD_BAD_PARAMETER, "");

    DISCORD_REQ_INIT(req, discord_guild_template, ret);

    return discord_adapter_run(&client->adapter, &req, NULL, HTTP_PUT,
                               "/guilds/%" PRIu64 "/templates/%s", guild_id,
                               code);
}
