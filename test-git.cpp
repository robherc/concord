#include "github-v3-user-agent.hpp"
#include "settings.h"
#include "ntl.h"


namespace ua = github::v3::user_agent;

struct file {
  char * path;
  char * content;
  char * sha;
};

void load_object_sha(char * str, size_t len, void * ptr) {
  fprintf (stderr, "%.*s\n", len, str);
  json_scanf(str, len, "[object][sha]%?s", ptr);
}

void load_sha(char *str, size_t len, void *ptr) {
  fprintf (stderr, "%.*s\n", len, str);
  json_scanf(str, len, "[sha]%?s", ptr);
}

void log(char * str, size_t len, void * ptr) {
  fprintf (stderr, "%.*s\n", len, str);
}

static struct api_resbody_s  body = { 0, 0 };

static struct resp_handle handle = {
  .ok_cb = NULL, .ok_obj = NULL,
  .err_cb = log, .err_obj = NULL
};

int commit (ua::dati *data, char * owner, char * repo,
            char * branch, char * filename, char * content) {
  //1. get the head of the master branch
  char * last_sha = NULL;
  handle.ok_cb = load_object_sha;
  handle.ok_obj = &last_sha;
  ua::run(data, &handle, NULL,
      GET, "/repos/%s/%s/git/refs/heads/master",  owner, repo);

  //2. create a new branch from last commit
  body.size = json_asprintf(&body.str,
                            "{ |ref|: |refs/heads/%s|, |sha|:|%s| }",
                            branch, last_sha);

  fprintf(stderr, "%.*s\n", body.size, body.str);
  handle.ok_cb = log;
  handle.ok_obj = NULL;
  ua::run(data, &handle, &body,
                  POST, "/repos/%s/%s/git/refs", owner, repo);

  //3. get sha of file be replaced
  char * file_sha = NULL;
  handle.ok_cb = load_sha;
  handle.ok_obj = &file_sha;
  ua::run(data, &handle, NULL,
      GET, "/repos/%s/%s/contents/%s", owner, repo, filename);

  //4. update a file
  body.size = json_asprintf(&body.str,
                            "{"
                                    "|message|:|update file|,"
                                    "|content|:|%s|,"
                                    "|branch|:|%s|,"
                                    "|sha|:|%s|"
                            "}",
                            content, branch, file_sha);

  fprintf(stderr, "%.*s\n", body.size, body.str);
  handle.ok_cb = log;
  handle.ok_obj = NULL;
  ua::run(data, &handle, &body,
      PUT, "/repos/%s/%s/contents/%s", owner, repo, filename);


  // 5. create a pull request
  body.size = json_asprintf(&body.str,
                            "{"
                               "|title|:|%s|,"
                               "|body|:|please pull this in|,"
                               "|head|:|%s|,"
                               "|base|:|master|"
                            "}",
                            branch, branch);

  handle.ok_cb = log;
  handle.ok_obj = NULL;
  ua::run(data, &handle, &body,
                  POST, "/repos/%s/%s/pulls", owner, repo);
  curl_global_cleanup();
  return 0;
}

char *
get_last_commit (ua::dati * data,
                 char * owner, char * repo)
{
  char * sha = NULL;
  handle.ok_cb = load_object_sha;
  handle.ok_obj = &sha;
  ua::run(data, &handle, NULL,
                  GET, "/repos/%s/%s/git/refs/heads/master",  owner, repo);
  return sha;
}

char *
get_tree_sha(ua::dati * data,
             char * owner, char * repo, char * commit_sha)
{
  fprintf(stderr, "===get-tree-sha==\n");
  char * sha = NULL;
  handle.ok_cb = load_sha;
  handle.ok_obj = &sha;
  ua::run(data, &handle, NULL,
                  GET, "/repos/%s/%s/git/trees/%s", owner, repo, commit_sha);

  fprintf(stderr, "tree-sha:%s\n", sha);
  return sha;
}

void
create_blobs (ua::dati * data, char * owner, char * repo,
              struct file ** files) {
  int i;
  char * file_sha = NULL;
  handle.ok_cb = load_sha;
  handle.ok_obj = &file_sha;

  for (i = 0; files[i]; i++) {
    fprintf(stderr, "===creating blob for %s===\n", files[i]->path);
    body.size = json_asprintf(&body.str,
                              "{ |content|:|%s|, |encoding|:|utf-8| }",
                              files[i]->content);
    ua::run(data, &handle, &body,
                    POST, "/repos/%s/%s/git/blobs", owner, repo);
    fprintf(stderr, "file-sha %s\n", file_sha);
    files[i]->sha = file_sha;
    free(body.str);
  }
}

static int
to_tree_node(char * str, size_t size, void *p)
{
  struct file * f = (struct file *)p;
  return json_snprintf(str, size,
                "{"
                  "|path|:|%s|,"
                  "|mode|:|100644|,"
                  "|type|:|blob|,"
                  "|sha|:|%s|"
                "}", f->path, f->sha);
}

static int
nodes_to_json (char * str, size_t size, void *p)
{
  return ntl_sn2str(str, size, (void **)p, NULL, to_tree_node);
}

static char *
create_tree (ua::dati * data,
             char * owner, char * repo,
             char * tree_sha,
             struct file ** files)
{
  fprintf(stderr, "==create-tree==\n");
  body.size = json_asprintf(&body.str,
                            "{"
                            "|tree|:%F,"
                            "|base_tree|:|%s|"
                            "}", nodes_to_json, files, tree_sha);
  char * new_tree_sha = NULL;
  handle.ok_cb = load_sha;
  handle.ok_obj = &new_tree_sha;
  fprintf(stderr, "%s\n", body.str);
  ua::run(data, &handle, &body,
                  POST, "/repos/%s/%s/git/trees", owner, repo);
  free(body.str);
  fprintf(stderr, "new-tree-sha:%s\n", new_tree_sha);
  return new_tree_sha;
}

static char *
create_a_commit (ua::dati * data,
                 char * owner, char * repo,
                 char * tree_sha,
                 char * last_cmmit_sha, char * message)
{
  fprintf(stderr, "===create-a-commit===\n");
  char * new_commit_sha = NULL;
  handle.ok_cb = load_sha;
  handle.ok_obj = &new_commit_sha;
  body.size = json_asprintf(&body.str,
                            "{"
                              " |message|:|%s|,"
                              " |tree|:|%s|,"
                              " |parents|: [ |%s| ]"
                            "}",
                            message, tree_sha, last_cmmit_sha);
  ua::run(data, &handle, &body,
                  POST, "/repos/%s/%s/git/commits", owner, repo);
  free(body.str);
  fprintf(stderr, "commit-sha:%s\n", new_commit_sha);
  return new_commit_sha;
}

static void
create_a_branch (ua::dati * data, char * owner, char * repo,
                 char * last_sha, char * branch) {

  fprintf(stderr, "===create-a-branch===\n");
  body.size = json_asprintf(&body.str,
                            "{ |ref|: |refs/heads/%s|, |sha|:|%s| }",
                            branch, last_sha);

  fprintf(stderr, "%.*s\n", body.size, body.str);
  handle.ok_cb = log;
  handle.ok_obj = NULL;
  ua::run(data, &handle, &body,
                  POST, "/repos/%s/%s/git/refs", owner, repo);
}

static void
update_a_commit (ua::dati * data,
                 char * owner, char * repo,
                 char * branch, char * commit_sha)
{
  fprintf(stderr, "===update-a-commit===\n");
  handle.ok_cb = log;
  body.size = json_asprintf(&body.str, "{|sha|:|%s|}", commit_sha);
  fprintf(stderr, "PATCH: %s\n", body.str);
  ua::run(data, &handle, &body,
                  PATCH, "/repos/%s/%s/git/refs/heads/%s", owner, repo, branch);
}

static void
create_a_pull_request (ua::dati * data,
                       char * owner, char * repo, char * branch) {
  // 5. create a pull request
  fprintf(stderr, "===create-a-pull-request===\n");
  body.size = json_asprintf(&body.str,
                            "{"
                              "|title|:|%s|,"
                              "|body|:|please pull this in|,"
                              "|head|:|%s|,"
                              "|base|:|master|"
                              "}",
                            branch, branch);
  handle.ok_cb = log;
  ua::run(data, &handle, &body,
                  POST, "/repos/%s/%s/pulls", owner, repo);
}

int main (int argc, char ** argv)
{
  const char *config_file;
  if (argc > 1)
    config_file = argv[1];
  else
    config_file = "bot.config";

  struct bot_settings settings;
  bot_settings_init (&settings, config_file);

  ua::dati data = {0};
  curl_global_init(CURL_GLOBAL_ALL);
  ua::init (&data, settings.github.username, settings.github.token);
  char * repo = "test_repo";
  char * owner = settings.github.username;

  if (0)
    commit(&data, settings.github.username,
           repo, "test_2", "x/test.c", "LypuZXcgY29kZSovCg==");
  else {
    struct file files [] = {
      {.path = "test/f.c", .content = "the contentxx of f.c"},
      {.path = "test/g.c", .content = "the contentxx of g.c"}
    };
    struct file * fptrs [] = { &files[0], &files[1], NULL};

    char * head_commit_sha =
      get_last_commit(&data, owner, repo);
    char * tree_sha = get_tree_sha(&data, owner, repo, head_commit_sha);

    create_blobs(&data, owner, repo, fptrs);
    char * new_tree_sha =
      create_tree(&data, owner, repo, tree_sha, fptrs);
    char * new_commit_sha =
      create_a_commit(&data, owner, repo, new_tree_sha,
                      head_commit_sha, "committed thru github API");

    char new_branch[1024];
    sprintf(new_branch, "n%ld", time(NULL));
    create_a_branch(&data, owner, repo,
                    head_commit_sha, new_branch);
    update_a_commit(&data, owner, repo,
                    new_branch, new_commit_sha);
    create_a_pull_request(&data, owner, repo, new_branch);
  }
  return 0;
}