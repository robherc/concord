#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "discord.h"
#include "discord-internal.h"
#include "discord-request.h"

CCORDcode
discord_get_invite(struct discord *client,
                   char *invite_code,
                   struct discord_get_invite *params,
                   struct discord_ret_invite *ret)
{
    struct discord_request req = { 0 };
    struct sized_buffer body;
    char buf[1024];

    CCORD_EXPECT(client, NOT_EMPTY_STR(invite_code), CCORD_BAD_PARAMETER, "");
    CCORD_EXPECT(client, params != NULL, CCORD_BAD_PARAMETER, "");

    body.size = discord_get_invite_to_json(buf, sizeof(buf), params);
    body.start = buf;

    DISCORD_REQ_INIT(req, discord_invite, ret);

    return discord_adapter_run(&client->adapter, &req, &body, HTTP_GET,
                               "/invites/%s", invite_code);
}

CCORDcode
discord_delete_invite(struct discord *client,
                      char *invite_code,
                      struct discord_ret_invite *ret)
{
    struct discord_request req = { 0 };

    CCORD_EXPECT(client, NOT_EMPTY_STR(invite_code), CCORD_BAD_PARAMETER, "");

    DISCORD_REQ_INIT(req, discord_invite, ret);

    return discord_adapter_run(&client->adapter, &req, NULL, HTTP_DELETE,
                               "/invites/%s", invite_code);
}
